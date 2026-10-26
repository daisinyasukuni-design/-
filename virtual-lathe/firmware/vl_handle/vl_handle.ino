// バーチャル旋盤 ハンドルユニット テスト用プログラム（ESP32-DevKitC）
//
// ・3つのハンドル（往復台・横送り・刃物送り台）のエンコーダー、主軸回転数つまみ、
//   起動・停止・非常停止ボタンを読み、20ms ごとに Bluetooth と USB の両方へ送る
// ・タブレットからの指令で振動させる（TCA9548A ＋ DRV2605L ×3、LRA）
// ・通信データの形式は docs/virtual-lathe/design.md の 4章、
//   ピン割り当ては 3.3 と同じ
//
// 必要なライブラリ（Arduino IDE のライブラリマネージャーから入れる）
//   ・ESP32Encoder（Kevin Harrington 作、0.11 以降）
//   ・Adafruit DRV2605 Library
// ボードは「ESP32 Dev Module」を選ぶ

#include <Wire.h>
#include <ESP32Encoder.h>
#include <Adafruit_DRV2605.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

// ---------- ユニットごとに変える ----------
#define DEVICE_NAME "VL-01"   // 本体のラベルと同じ番号にする

// ---------- ピン割り当て（設計書 3.3） ----------
const int PIN_CARRIAGE_A = 32, PIN_CARRIAGE_B = 33;  // 往復台
const int PIN_CROSS_A    = 25, PIN_CROSS_B    = 26;  // 横送り
const int PIN_COMPOUND_A = 34, PIN_COMPOUND_B = 35;  // 刃物送り台（外付けプルアップ）
const int PIN_SDA = 21, PIN_SCL = 22;
const int PIN_START = 18, PIN_STOP = 19, PIN_ESTOP = 23;
const int PIN_RPM = 36;
const int PIN_LED = 2;

// ---------- 通信の決まり（設計書 4章） ----------
#define SERVICE_UUID "a5f0c000-1d2e-4b7a-9c31-56c7e8a4f001"
#define STATE_UUID   "a5f0c000-1d2e-4b7a-9c31-56c7e8a4f002"
#define VIB_UUID     "a5f0c000-1d2e-4b7a-9c31-56c7e8a4f003"
const uint8_t USB_HEAD_OUT = 0xA5;  // ESP32 → タブレット
const uint8_t USB_HEAD_IN  = 0x5A;  // タブレット → ESP32
const uint32_t SEND_INTERVAL_MS = 20;

// ---------- 振動の安全処理（設計書 4.3） ----------
const uint32_t WATCHDOG_MS     = 500;   // 指令が途切れたら止める
const uint32_t MAX_ON_MS       = 3000;  // 連続して振動できる上限
const uint32_t COOLDOWN_MS     = 1000;  // 上限に達したあと休む時間
const uint8_t  TCA_ADDR        = 0x70;
const int      NUM_VIB         = 3;     // 0:往復台 1:横送り 2:刃物送り台

ESP32Encoder encoders[3];
Adafruit_DRV2605 drv[NUM_VIB];
bool drvPresent[NUM_VIB] = {false, false, false};

struct Vib {
  uint8_t  strength = 0;    // 0〜255
  uint8_t  pattern  = 0;    // 0:連続 1:びびり 2:衝撃 3:脈動
  uint32_t startMs  = 0;
  uint32_t endMs    = 0;    // 0 = 次の指令まで
  uint32_t onSince  = 0;    // 連続で振動し始めた時刻（0 = 止まっている）
  uint32_t coolUntil = 0;
  uint8_t  lastOut  = 0;
};
Vib vib[NUM_VIB];
uint32_t lastCmdMs = 0;

// Bluetooth の受信は別のタスクで動くので、受け渡し用の場所を用意する
portMUX_TYPE cmdMux = portMUX_INITIALIZER_UNLOCKED;
volatile bool cmdPending = false;
uint8_t cmdBuf[5];

BLECharacteristic* stateChar = nullptr;
volatile bool bleConnected = false;
uint8_t seq = 0;
float rpmFiltered = 0;

// ---------- Bluetooth ----------
class ServerCB : public BLEServerCallbacks {
  void onConnect(BLEServer*) override { bleConnected = true; }
  void onDisconnect(BLEServer*) override {
    bleConnected = false;
    BLEDevice::startAdvertising();  // また見つけてもらえるようにする
  }
};

class VibCB : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic* c) override {
    auto v = c->getValue();
    if (v.length() < 5) return;
    const uint8_t* d = (const uint8_t*)v.c_str();
    portENTER_CRITICAL(&cmdMux);
    memcpy(cmdBuf, d, 5);
    cmdPending = true;
    portEXIT_CRITICAL(&cmdMux);
  }
};

// ---------- 振動ドライバ ----------
bool tcaSelect(int ch) {
  Wire.beginTransmission(TCA_ADDR);
  Wire.write(1 << ch);
  return Wire.endTransmission() == 0;
}

void setupVibration() {
  Wire.begin(PIN_SDA, PIN_SCL);
  Wire.beginTransmission(TCA_ADDR);
  if (Wire.endTransmission() != 0) return;  // 切替器がない → 振動なしで動かす
  for (int i = 0; i < NUM_VIB; i++) {
    if (!tcaSelect(i)) continue;
    if (!drv[i].begin(&Wire)) continue;
    drv[i].useLRA();
    // オープンループで LRA を駆動する（自動調整なしでも振動させるため）
    drv[i].writeRegister8(DRV2605_REG_CONTROL3, drv[i].readRegister8(DRV2605_REG_CONTROL3) | 0x01);
    drv[i].setMode(DRV2605_MODE_REALTIME);
    drv[i].setRealtimeValue(0);
    drvPresent[i] = true;
  }
}

void writeVib(int i, uint8_t value) {
  if (!drvPresent[i] || vib[i].lastOut == value) return;
  if (!tcaSelect(i)) return;
  drv[i].setRealtimeValue(value >> 1);  // 0〜127
  vib[i].lastOut = value;
}

void applyCommand(const uint8_t* c, uint32_t now) {
  uint8_t target = c[0], strength = c[1], pattern = c[2];
  uint16_t duration = c[3] | (c[4] << 8);
  if (duration > MAX_ON_MS) duration = MAX_ON_MS;
  lastCmdMs = now;
  for (int i = 0; i < NUM_VIB; i++) {
    if (target != 0 && target != i + 1) continue;  // 4（台座）はこの試作では使わない
    vib[i].strength = strength;
    vib[i].pattern = pattern;
    vib[i].startMs = now;
    vib[i].endMs = duration ? now + duration : 0;
  }
}

uint8_t patternValue(const Vib& v, uint32_t now) {
  uint32_t t = now - v.startMs;
  switch (v.pattern) {
    case 1:  return ((t / 15) % 2) ? v.strength : v.strength / 4;  // びびり：細かく強弱
    case 2:  return t < 80 ? v.strength : 0;                       // 衝撃：一発だけ短く
    case 3:  return ((t / 200) % 2) ? 0 : v.strength;              // 脈動
    default: return v.strength;                                    // 連続
  }
}

void updateVibration(uint32_t now, bool estop) {
  for (int i = 0; i < NUM_VIB; i++) {
    Vib& v = vib[i];
    bool expired = v.endMs ? (int32_t)(now - v.endMs) >= 0
                           : (now - lastCmdMs) > WATCHDOG_MS;
    if (estop || expired) v.strength = 0;
    uint8_t out = (v.strength && (int32_t)(now - v.coolUntil) >= 0) ? patternValue(v, now) : 0;
    if (out) {
      if (!v.onSince) v.onSince = now;
      if (now - v.onSince > MAX_ON_MS) {  // 長く振動し続けたら強制的に休ませる
        v.strength = 0;
        v.coolUntil = now + COOLDOWN_MS;
        out = 0;
      }
    } else if (v.strength == 0) {
      v.onSince = 0;
    }
    writeVib(i, out);
  }
}

// ---------- USB の受信（0x5A ＋ 5バイト ＋ XOR） ----------
void readUsb(uint32_t now) {
  static uint8_t buf[7];
  static int len = 0;
  while (Serial.available()) {
    uint8_t b = Serial.read();
    if (len == 0 && b != USB_HEAD_IN) continue;
    buf[len++] = b;
    if (len == 7) {
      uint8_t x = 0;
      for (int i = 1; i < 6; i++) x ^= buf[i];
      if (x == buf[6]) applyCommand(buf + 1, now);
      len = 0;
    }
  }
}

// ---------- ハンドル状態の送信（16バイト） ----------
void sendState(bool estop) {
  uint8_t p[16];
  for (int i = 0; i < 3; i++) {
    int32_t c = (int32_t)encoders[i].getCount();
    memcpy(p + i * 4, &c, 4);  // ESP32 はリトルエンディアン
  }
  uint16_t rpmRaw = (uint16_t)(rpmFiltered + 0.5f);
  p[12] = rpmRaw & 0xFF;
  p[13] = rpmRaw >> 8;
  uint8_t buttons = 0;
  if (digitalRead(PIN_START) == LOW) buttons |= 0x01;
  if (digitalRead(PIN_STOP) == LOW)  buttons |= 0x02;
  if (estop)                         buttons |= 0x04;
  p[14] = buttons;
  p[15] = seq++;

  uint8_t x = 0;
  for (int i = 0; i < 16; i++) x ^= p[i];
  Serial.write(USB_HEAD_OUT);
  Serial.write(p, 16);
  Serial.write(x);

  if (bleConnected && stateChar) {
    stateChar->setValue(p, 16);
    stateChar->notify();
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_START, INPUT_PULLUP);
  pinMode(PIN_STOP, INPUT_PULLUP);
  pinMode(PIN_ESTOP, INPUT_PULLUP);  // b接点：ふだんは GND につながって LOW
  analogReadResolution(12);

  // エンコーダーはすべて外付けの抵抗で 3.3V にプルアップする
  ESP32Encoder::useInternalWeakPullResistors = puType::none;
  encoders[0].attachFullQuad(PIN_CARRIAGE_A, PIN_CARRIAGE_B);
  encoders[1].attachFullQuad(PIN_CROSS_A, PIN_CROSS_B);
  encoders[2].attachFullQuad(PIN_COMPOUND_A, PIN_COMPOUND_B);
  for (auto& e : encoders) e.clearCount();

  setupVibration();

  BLEDevice::init(DEVICE_NAME);
  BLEServer* server = BLEDevice::createServer();
  server->setCallbacks(new ServerCB());
  BLEService* svc = server->createService(SERVICE_UUID);
  stateChar = svc->createCharacteristic(STATE_UUID,
      BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
  stateChar->addDescriptor(new BLE2902());
  BLECharacteristic* vibChar = svc->createCharacteristic(VIB_UUID,
      BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_WRITE_NR);
  vibChar->setCallbacks(new VibCB());
  svc->start();
  BLEAdvertising* adv = BLEDevice::getAdvertising();
  adv->addServiceUUID(SERVICE_UUID);
  adv->setScanResponse(true);
  BLEDevice::startAdvertising();

  rpmFiltered = analogRead(PIN_RPM);
}

void loop() {
  static uint32_t lastSend = 0;
  uint32_t now = millis();

  // 断線しても HIGH になり「非常停止」側になる
  bool estop = digitalRead(PIN_ESTOP) == HIGH;

  if (cmdPending) {
    uint8_t c[5];
    portENTER_CRITICAL(&cmdMux);
    memcpy(c, cmdBuf, 5);
    cmdPending = false;
    portEXIT_CRITICAL(&cmdMux);
    applyCommand(c, now);
  }
  readUsb(now);

  rpmFiltered += (analogRead(PIN_RPM) - rpmFiltered) * 0.2f;  // つまみのちらつきをならす

  updateVibration(now, estop);

  if (now - lastSend >= SEND_INTERVAL_MS) {
    lastSend = now;
    sendState(estop);
  }

  // LED：Bluetooth 接続中は点灯、非常停止中は点滅
  digitalWrite(PIN_LED, estop ? ((now / 250) % 2) : bleConnected);
  delay(2);
}
