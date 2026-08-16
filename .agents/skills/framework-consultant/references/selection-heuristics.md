# selection-heuristics.md — 横断選定の詳細ルール

framework-consultant（SKILL.md）が `references/INDEX.slim.yaml` から2〜4個を選ぶための実務ルール集。

## 目次
- [1. ジョブ → カテゴリ/lens 対応表](#1-ジョブ--カテゴリlens-対応表)
- [1.5 候補絞り込み手続き（スコア式の実行定義）](#15-候補絞り込み手続きスコア式の実行定義)
- [2. lens 分散チェック（多様性ルールの手続き）](#2-lens-分散チェック多様性ルールの手続き)
- [3. 相性グラフ（よく効く組み合わせ）](#3-相性グラフよく効く組み合わせ)
- [4. 典型レシピ集（相談タイプ別の初期セット）](#4-典型レシピ集相談タイプ別の初期セット)
- [5. 揺さぶり枠の運用](#5-揺さぶり枠の運用)
- [5.5 risk枠の使い分けとローテーション](#55-risk枠の使い分けとローテーション)
- [6. アンチパターン](#6-アンチパターン)
- [7. 時間制約の扱い](#7-時間制約の扱い)

## 1. ジョブ → カテゴリ/lens 対応表

診断で分類した中核ジョブから、まず候補カテゴリを引く。**必ず主レンズ以外を1つ混ぜる**。

| 中核ジョブ | 主に見るカテゴリ | 主 lens | 混ぜる副 lens（必須） |
|---|---|---|---|
| 現状分析 | A_strategy / B_marketing / D_logic | analysis | ideation か risk か perspective-shift |
| 意思決定・優先順位づけ | lens=decision を全カタログ横断で検索（主に H_refine / J_organization に散在。A_strategy / C_innovation にも） | decision | risk か analysis |
| アイデア創出 | E_divergent / F_invention / G_perspective | ideation / perspective-shift | analysis（地に足）か risk（検証） |
| 事業構想 | C_innovation / A_strategy | analysis / ideation | risk（プレモータム等） |
| 真因究明 | D_logic / I_operations / G_perspective（効かない対策・真因が見えない場合）。顧客系の真因（解約・離反・売れない）は B_marketing / C_innovation も見る | analysis（I 由来は improvement も可） | perspective-shift（前提を疑う） |
| 業務・生産改善 | I_operations | improvement | analysis か decision（対策が枯渇している場合は ideation か perspective-shift） |
| 組織・人 | J_organization | people | analysis か decision か risk |
| リスク潰し | H_refine ほか（risk lens は A_strategy(scenario-planning)／I_operations(fmea, three-step-method) にも散在。lens横断で引く） | risk | analysis（事実で裏取り） |
| 別視点・セカンドオピニオン | 直前に使ったフレームワークと異なる lens を全カテゴリ横断（G_perspective / H_refine を優先） | perspective-shift / risk | 直前と同一 lens は選ばない |

**カテゴリ列は初期の目安に過ぎない。** INDEX の category 名（A_strategy=戦略, B_marketing=マーケ, C_innovation=新規事業, D_logic=論理, E_divergent=発散, F_invention=発明, G_perspective=視点転換, H_refine=検証, I_operations=業務改善, J_organization=組織・人）と正確に対応づけ、lens・fit_tags の一致を優先する。「意思決定」という単独カテゴリは存在せず、decision lens は複数カテゴリに散在する点に注意。

**fit_tags スキャンの義務化（表よりカテゴリより優先）:** 選定前に必ず INDEX 全体の fit_tags を相談文のキーワードで一度スキャンし、表外カテゴリでも直接一致（相談の固有名詞・論点に直接一致）があれば候補に加える。**fit_tags の強一致はカテゴリ指定に優先する。** 副lens枠は全カテゴリから引いてよく、perspective-shift は主に G_perspective、risk は主に H_refine から調達する。

**問題の性質が不明なとき（cynefin 先行）:** 問題の性質（明白/煩雑/複雑/混沌）自体が不明な相談は `cynefin` を先行適用して域を判定してからFW群を選んでよい（域が決まれば分析で解ける＝煩雑か、試して探る＝複雑かが定まり、後段のレシピ選定が安定する）。

**複合ジョブの合成規則:** 複合相談は主ジョブ1つ＋副ジョブ最大1つに割り切る。利用者が最終的に欲しい出力（いま決めたい・前に進めたいこと）に近い側を主ジョブとし、主lens・副lens必須ルールは主ジョブの行に従う。候補カテゴリは両ジョブの行の和集合とし、主ジョブの行で2〜3個＋副ジョブの主カテゴリから最低1個を補充する。3ジョブ以上に見える場合は診断未確定とみなし、壁打ち質問1〜3問で主ジョブを確定してから選定に進む。
- 例:『動機の診断→進退の決定』は 意思決定を主・組織人を副 とする。
- 『何から手をつけるか』型は、比較すべき選択肢がまだ未列挙なら意思決定ではなく現状分析/真因究明に分類する。

**個人・本人からの相談:** 組織・人行は本人からの相談（キャリア・働き方・モチベーション）にも使ってよい。people 系（herzberg / maslow / grow-model）は自己診断モードで適用する（対象者＝相談者本人と読み替える）。個人相談では fit_tags の字面一致より purpose の構造適合で判定してよい。

## 1.5 候補絞り込み手続き（スコア式の実行定義）

SKILL.md Step 2 のスコア式は設計意図の覚え書きで、**数値計算はしない**。実行はこの貪欲手続きが正。
**優先順位は 適合度 ＞ lens多様性 ＞ 相性。**

1. §1 のジョブ→カテゴリ表＋ fit_tags スキャンで候補化する（87 → 10〜20 個）。
2. 各候補に**適合度を 0〜2 で付与**する（2 = fit_tags のいずれかが相談文の課題にほぼ直撃、1 = purpose が該当、0 = 無関係）。**相談文が明示するデータ・情報を直接活かせる候補は適合度を +1 する（上限2）。** 最高点を**アンカー**として採用する。
3. 2個目以降はアンカーと**異なる lens** の候補から適合度降順に選ぶ。同点時のタイブレークは次の優先順:
   - ① 既選定候補の `pairs_well_with` に載っている
   - ② 適合度の +1 補正（データ直活用）を反映してなお同点の場合、候補の必要入力の充足度で比べる（必要入力は当該 FRAMEWORK.md の「前提と限界」節から判断する。専用の inputs 欄は無い）
   - ③ lens が既選定集合から最も遠い
   - ④ category が異なる
4. 2〜4個そろったら §2 の lens 分散チェックで検収し、不合格なら §2 の差し替えを行う。
5. **必要入力の検収（仮確定後）:** 候補を仮確定したら上位候補の FRAMEWORK.md の「前提と限界」「進め方」節で必要入力を確認し、過半が未充足なら選定を確定せず SKILL.md Step 5 の壁打ち質問へ戻る（専用の inputs 欄は無く slim カタログにも無いため、FRAMEWORK.md を開くこの段で確認する）。

**相性の扱い:** `pairs_well_with` に無い組み合わせは減点せず**中立扱い**とする。どちらか片方向の記載があれば相性ありとみなす（§2 も参照）。相性を掛け算でゼロにして最適候補を落とす読みは採らない。

## 2. lens 分散チェック（多様性ルールの手続き）

選定候補を仮に置いたら、機械的に確認する:

1. 候補の `lens` を列挙する。
2. **ユニークな lens 数 ≥ 2** を必須とする（2個選ぶなら2種、3〜4個なら最低2種、できれば3種）。
3. `analysis` が含まれるなら、`ideation` / `risk` / `perspective-shift` のうち**最低1つ**を含める。
4. 満たさなければ、最も適合度の低い同lens候補を1つ外し、副lensから相性の良いものへ差し替える。
5. 差し替え候補は当該候補の `pairs_well_with` を優先的に見る。**`pairs_well_with` は無向として扱う** — 当該候補が列挙する相手に加え、当該候補を列挙している側（逆参照）も差し替え候補に含める。異lens候補が無い場合は §3 の橋渡し表をフォールバックとして差し替え候補に使う。
6. **同一lens・同一クラスタが2個**になったら（例: `payoff-matrix`＋`decision-matrix` はどちらも decision の
   「絞り込みクラスタ」）、残り1個は**最も遠いlens**（risk / perspective-shift 等）を選び、**その1個に統合の
   緊張（食い違い）を担わせる**。同lens内だけで緊張が閉じると新しい視点が出にくい（Step 4 の統合が羅列化する）。

**適合候補が乏しいときの縮小:** 副lens候補の適合度が全て低い（fit_tags 一致 0）場合は、無理に3個にせず **主lens×1＋異lens×1 の2個選定に縮小してよい**。2個選定はユニーク lens 2種を必須とする。

> lens は INDEX の中核フィールド。ここを機械的に守ることが「新しい視点」を構造的に保証する。

## 3. 相性グラフ（よく効く組み合わせ）

id 単位の代表的な相性（各 FRAMEWORK.md の「相性」および INDEX の `pairs_well_with` が正）:

- `swot` ⇄ `cross-swot` ⇄ `pest` ⇄ `3c` ⇄ `five-forces`（環境分析クラスタ）
- `3c` → `stp` → `4p`/`4c-marketing`（分析からマーケ施策へ）
- `jtbd` ⇄ `value-proposition-canvas` ⇄ `business-model-canvas`/`lean-canvas`（事業設計クラスタ）
- `logic-tree` ⇄ `mece` ⇄ `pyramid-structure`（論理構造クラスタ）
- `fishbone` ⇄ `five-whys` ⇄ `qc-story`（真因究明クラスタ）
- `scamper` ⇄ `osborn-checklist` ⇄ `random-word` ⇄ `synectics`（強制発想クラスタ）
- `payoff-matrix` ⇄ `decision-matrix` ⇄ `pmi` ⇄ `eisenhower-matrix` ⇄ `decision-tree`（絞り込みクラスタ）
- `premortem` ⇄ `devils-advocate` ⇄ `red-team`（揺さぶり・検証クラスタ）
- `toc` ⇄ `value-stream-mapping` ⇄ `ecrs` ⇄ `lean`（工程改善クラスタ）
- `maslow` ⇄ `herzberg` ⇄ `grow-model`（動機づけ・育成クラスタ）
- `mckinsey-7s` ⇄ `kgi-kpi-okr`（組織設計クラスタ）
- `ooda` ⇄ `pdca` ⇄ `lean-startup-mvp`（即応クラスタ）
- `fmea` ⇄ `fta` ⇄ `three-step-method` ⇄ `fishbone`/`five-whys`（安全・信頼性クラスタ）
- `market-sizing` ⇄ `unit-economics` ⇄ `lean-canvas`/`business-model-canvas`（定量検証クラスタ）
- `change-management` ⇄ `stakeholder-analysis` ⇄ `mckinsey-7s`（変革クラスタ）
- `aarrr` ⇄ `unit-economics` ⇄ `kgi-kpi-okr`（グロース指標クラスタ）
- `service-blueprint` ⇄ `customer-journey` ⇄ `value-stream-mapping`（体験×工程クラスタ）

**橋渡し（異lensを繋ぐと洞察が出やすい）:**
- 分析 × 発想: `five-forces` × `jtbd`（競争構造の空白 × 未充足ジョブ）
- 分析 × 検証: `swot` × `premortem`（強み前提の戦略 × 失敗要因）
- 発想 × 意思決定: `scamper` × `payoff-matrix`（大量の案 × 効果×実現性で選別）
- 改善 × 視点転換: `five-whys` × `reverse-assumption`（真因追究 × 前提の反転）

## 4. 典型レシピ集（相談タイプ別の初期セット）

そのまま出さず、相談の具体に合わせて微調整する。lens多様性は各レシピで担保済み。
**レシピ直行とゲートの優先:** 相談タイプが本§4レシピに直接一致し、対象・症状・欲しい出力が揃っている場合は、SKILL.md Step 1 の質問先行ゲートを発火させずレシピで進む。主ジョブが定まらない場合のみ質問を先行する。
**表（§1）とレシピが食い違う場合はレシピ（相談タイプの直接一致）を優先する。**
相談者が既に実施・検討済みの手法に相当するフレームワークは選定から外すか、壁打ち質問（SKILL.md Q3）で
既往対策を確認してから確定する。**「出尽くした」「やり尽くした」の語が出たら、レシピの analysis 枠を
ideation / perspective-shift 枠に1つ差し替える**。
**レシピと §5.5 の使い分けが食い違う場合は §5.5 を優先する** — レシピに `premortem` があっても確定した計画・案が
まだ無い相談では、§5.5 に従い `devils-advocate` 等へ差し替える。レシピ直行する場合も既往対策の確認は
選定確定を止めず、Step 3 の壁打ち質問（最大3問）に組み込む。

| 相談タイプ | レシピ（id / lens） |
|---|---|
| 新規事業が行き詰まる | `jtbd`(ideation) + `five-forces`(analysis) + `premortem`(risk) |
| 複数施策の優先順位 | `decision-matrix`(decision) + `payoff-matrix`(decision) + `premortem`(risk) |
| 競合環境を整理したい | `3c`(analysis) + `five-forces`(analysis) + `jtbd`(ideation) ※発想を1つ混ぜる |
| 既存事業の売上長期低迷 | `logic-tree`(analysis) + `3c`(analysis) + `jtbd`(ideation) ※真因分解×環境×顧客動機。将来像が論点なら `jtbd`→`backcasting`(perspective-shift) |
| 不良/トラブルの真因 | `five-whys`(analysis) + `fishbone`(analysis) + `reverse-assumption`(perspective-shift) |
| 解約・離反・売れないの真因 | `jtbd`(ideation) + `customer-journey`(analysis) + `five-whys`(analysis) または `abduction`(perspective-shift) ※lens3種を優先 |
| 打ち手が思いつかない | `scamper`(ideation) + `osborn-checklist`(ideation) + `payoff-matrix`(decision) |
| 事業の将来像を描く | `backcasting`(perspective-shift) + `ansoff`(decision) + `premortem`(risk) |
| 組織の停滞 | `mckinsey-7s`(people) + `herzberg`(people) + `devils-advocate`(risk) |
| 個人・チームのモチベーション/1on1 | `herzberg`(people) + `grow-model`(people) + `devils-advocate`(risk) ※組織全体の停滞なら `herzberg`→`mckinsey-7s` |
| 意思決定に自信がない | `pmi`(decision) + `six-hats`(perspective-shift) + `premortem`(risk) |
| 競合の攻勢に即応（強い時間制約） | `ooda`(decision) + `five-forces`(analysis) + `premortem`(risk) ※選択肢が未生成なら `ooda` の Orient で粗い対応案を3つ立ててから残り2枠で構造判断とストレステスト。時間が極端に無い場合は analysis 枠（`five-forces`）を落として `ooda`+`premortem` の2個に縮退（§7 も参照） |
| 対象不明の全体不調（質問1巡後も曖昧） | `swot`(analysis) + `logic-tree`(analysis) + `six-hats`(perspective-shift) ※仮置き前提を【診断】に明示して広く当て、Step 5 の壁打ちで絞る（lens は analysis×2＋perspective-shift で §2 充足） |
| 中期計画・大型投資が不確実で決めきれない | `scenario-planning`(risk) + `pest`(analysis) + `ansoff`(decision) |
| 新規事業の儲かる根拠を検証したい | `market-sizing`(analysis) + `unit-economics`(analysis) + `premortem`(risk) ※analysis2枚のため3個目のriskに緊張を担わせる |
| 事故・不良を未然に潰したい | `fmea`(risk) + `fta`(analysis) + `three-step-method`(risk) |
| 確率が読めない投資のGo/No-Go | `decision-tree`(decision) + `scenario-planning`(risk) + `so-what-why-so`(analysis) |
| グロース停滞・KPI迷子 | `aarrr`(analysis) + `unit-economics`(analysis) + `jtbd`(ideation) ※3個目に緊張を担わせる |
| 交渉・合意形成の準備 | `negotiation-batna`(decision) + `stakeholder-analysis`(people) + `premortem`(risk) |

> **decision枠を2個使うレシピの注意（例:「複数施策の優先順位」）:** `decision-matrix` と `payoff-matrix` は
> どちらも decision の「絞り込みクラスタ」で近い。**3個目のフレームワークに必ず risk か perspective-shift を置き**
> （ユニーク lens 数は最低2で可、この3個目に統合の緊張を担わせる）、羅列を防ぐ（§2-6）。
> **選択肢が2〜3個の二者択一型はマトリクスを1枚（`decision-matrix`）に減らし**、空いた枠に
> `lean-startup-mvp`(decision) か（データがあるなら）`so-what-why-so`(analysis) を置く。

## 5. 揺さぶり枠の運用

- Step 5 で常に1つ、`perspective_shift: true` から添えてよい:
  `reverse-assumption` / `devils-advocate` / `premortem` / `six-hats`（黒帽子=リスク, 緑帽子=発想）
  / `red-team` / `backcasting` / `scenario-planning` / `abduction` / `lateral-thinking` / `analogical-thinking` /
  `jtbd` / `equivalent-transformation` / `synectics` / `nm-method` / `cynefin`。
  （このリストは INDEX の `perspective_shift: true` 全15件と一致。`ooda` は適応ループであり
  視点転換枠ではないため除外、アナロジー族の `synectics` / `nm-method` を追加。新規の `scenario-planning`（risk・複数未来）も揺さぶり枠に加える。新規の `cynefin`（問題の性質を見極めるメタ診断）も揺さぶり枠に加える。）
- **揺さぶり枠には Step 2 で選定済みの id を使わない**（同一スキルが選定枠と揺さぶり枠で二重に出るのを防ぐ）。
- **同時適用の上限:** 選定分と合わせて最大4個。既に4個選定している場合、Step 4 の緊張生成での即時適用はせず
  次ターンの候補として提示する（§6「5個以上」の認知過負荷を守る）。
- 選び方: すでに使った lens と最も遠いものを選ぶと、統合の緊張関係が際立つ。fit_tags の適合と lens の遠さが
  割れる場合は lens の遠さを優先する（揺さぶり枠の目的は適合でなく緊張の生成）。risk 系を選ぶ場合は §5.5 に従う。

## 5.5 risk枠の使い分けとローテーション

risk lens は premortem / devils-advocate / red-team / scenario-planning / fmea / three-step-method の6種に増えた。premortem への構造的収束を防ぎ、相談の型で使い分ける:

- **premortem**: 具体的な計画・案が既に存在する相談に限定（失敗要因の事前想定）。
- **devils-advocate**: 案が固まる前の段階で使う（案の反証＝仮説への最強反論）。
- **red-team**: 組織的・敵対的視点が要る戦略/セキュリティ/競合対抗の相談で優先する（敵視点）。
- **scenario-planning**: 外部環境の不確実性が高く、単一失敗想定でなく複数の未来に備える長期・大型判断で使う（環境不確実性）。
- **fmea**: 製品・工程の潜在故障を工学的・網羅的に事前解析し S/O/D で定量格付けする品質リスク潰しで使う（工学的・網羅的な事前解析）。
- **three-step-method**: 特定済みリスクへの対策を本質安全設計→防護→情報の階層で設計する（対策の階層設計）。
- 同一セッションで risk 枠を2回以上使う場合は、前回と異なるスキルを選ぶ。

## 6. アンチパターン

- **同lens3個の羅列**（例: `swot`+`3c`+`five-forces` だけ）→ 新視点が出ない。必ず異lensを混ぜる。
- **診断を飛ばした選定** → まず中核ジョブを確定（不足なら壁打ち）。
- **統合での緊張関係ゼロ** → 各出力の要点を並べただけは失敗。最低1つ食い違いを言語化する。ただし本当に全レンズが
  同方向なら**捏造せず**『全レンズが同方向』を発見として報告し、未使用の揺さぶり枠で人工的に緊張を作る（SKILL.md Step 4-2）。
- **選択肢ゼロのまま decision 系マトリクスを起動する** → 比較対象が無いと空回りする。先に選択肢を粗く生成する（§4 即応レシピ参照）。
- **5個以上の選定** → 認知過負荷。2〜4個に絞る（深掘りは壁打ちループで）。

## 7. 時間制約の扱い

当日〜数日の即応案件（例:「明日までに対応方針を決めたい」）は縮退モードで動く:

- 選定を**2〜3個**に絞る（decision 1 + risk または analysis 1 を基本に、競合対応など構造判断が要る場合は §4 即応レシピの3個を許容。時間が極端に無い場合はレシピから analysis 枠を落として2個に縮退）。`ooda` 等の即応系 fit_tags を持つものを優先する。
- 壁打ち質問は**最大1問**とし、残る不明点は仮置き前提として【診断】に明記して進む。
- Step 3 の適用も**要点モード**に縮退する（フル出力フォーマットを回さない）。
- 競合対応など選択肢が未生成の場合は §4「競合の攻勢に即応」レシピを使う。

## 更新履歴
- 2026-07-08: 新規6スキル統合（§1 に cynefin 先行適用の診断ルール、§3 グロース指標/体験×工程クラスタ追加・絞り込みクラスタに decision-tree、§4 レシピ3件追加=確率投資Go/No-Go・グロース停滞・交渉準備、§5 揺さぶり枠に cynefin 追加=全15件）
- 2026-07-08: 新規10スキル統合（§1 リスク潰し行のlens散在注記、§3 安全・信頼性/定量検証/変革クラスタ追加、§4 レシピ3件追加、§5 揺さぶり枠に scenario-planning 追加=全14件、§5.5 risk枠を6種に拡張・使い分け更新）
- 2026-07-08: レビュー指摘修正（スコア実行定義・複合ジョブ・risk偏り是正）
- 2026-07-08: §5 揺さぶり枠リストを INDEX の最終フラグと同期（synectics / nm-method を追加・ooda を削除）
- 2026-07-08: §1.5 に inputs 検収（step 5）とデータ保有 +1 補正を追加、§4 冒頭にレシピ直行の優先規則、§7・§4 の選定個数を2〜3個へ一本化
- 2026-07-20: スキルパッケージ検証の指摘反映（inputs 表記を「前提と限界」節参照へ修正、§4 にレシピと§5.5 の優先規則、§5 に lens 距離優先）
- 2026-07-20: 揺さぶり枠の同時適用上限（選定分と合わせて最大4個）を明文化
