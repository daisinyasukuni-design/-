# USIT（Unified Structured Inventive Thinking）

TRIZを**簡素化**した派生プロセス。多数のツール・データベースを暗記せずに、
問題を **オブジェクト（Object）/ 属性（Attribute）/ 機能（Function）** の言葉だけで扱い、短時間で一気通貫に回す。
「TRIZの道具立てが多すぎて重い」ときの軽量な入口。

## 出自（要確認）
- 系譜: イスラエルの **SIT（Roni Horowitz ら）** を源流に、**Ed Sickafus** が Ford で自動車開発向けに適合（1990年代前半）。
- 日本では **中川徹** が「**6箱スキーム（Six-Box Scheme）**」として再整理・普及。
- 人名・年代・役割分担は資料により差があり **要ファクトチェック**。

## 特徴
- **DB・ソフト不要。** 40原理表やマトリクスを引かずに進められる（背後の思想はTRIZと同じ）。
- 問題を **object / attribute / function** の3語彙で記述する。属性を動かして機能を作り替えるのが発想の軸。
- 「**パズルへの昇華（elevation to a puzzle）**」— 解に不可欠でない要素を削ぎ落とし、問題を最小のパズルにする。

## 進め方（6箱スキームの流れ）

1. **問題の定義（Problem）**: 望ましくない現象と、達成したい機能を、専門用語を避けて平易に書く。
2. **現状分析（Analysis / 情報収集）**: 関与するオブジェクト、その属性、機能を洗い出し、
   **矛盾が起きている場所（conflict zone）と時間**を特定する。
3. **理想を描く（Ideal）**: この問題が理想的に解けたら何が起きるか（TRIZのIFRに相当）。
4. **解の生成（Solutions）**: 3系統の発想オペレータを使う —
   - **Object 法**: オブジェクトを増やす/減らす/置き換える。
   - **Attribute 法**: 属性を変える（強める/弱める/反転/新規追加）。
   - **Function 法**: 機能を分配・移動・複合させる。
5. **解の具体化・評価**: 出た概念を実装可能な案に翻訳し、実現性で絞る。

## いつ使う / 注意
- **使う**: 時間が限られ、TRIZフル装備は重い／チームで速く一巡したい／技術問題を平易な言葉で共有したいとき。
- **注意**: 簡素な分、深い矛盾解消は古典TRIZ（物理的矛盾＋分離原理、ARIZ）に劣ることがある。
  難問は USIT で当たりを付け、必要に応じて `principles-40.md`・`contradiction-matrix.md`・`su-field.md` に接続する。

出典: Wikipedia "Unified structured inventive thinking" https://en.wikipedia.org/wiki/Unified_structured_inventive_thinking ／
Nakagawa, T. "USIT: A Concise Process ... Six-Box Scheme" https://www.osaka-gu.ac.jp/php/nakagawa/TRIZ/eTRIZ/eSickafusMemorial/eSickafus-TextBooks-Tutorials/USITOverView-030214.pdf ／
The TRIZ Journal "Book Review: Unified Structured Inventive Thinking" https://the-trizjournal.com/book-review-unified-structured-inventive-thinking-invent/
