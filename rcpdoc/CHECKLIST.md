# RCP2 実装チェックリスト

このチェックリストは、[JMPlayer RCP format notes](https://github.com/jihodaya/jmplayer/blob/main/docs/formats/en/rcp.md) の確認項目を、S-MU2000側で段階的に照合できる形に整理したものです。

**重要:** JMPlayer資料内の ✅ は、資料の著者が参照MIDIとの比較などで確認したという意味です。S-MU2000で実装・テスト済みという意味ではありません。各項目のチェックは、S-MU2000のコード確認やテストを実際に終えてから付けます。

状態の使い分け:
- **資料:** JMPlayer資料の記述を把握したか
- **コード:** S-MU2000の実装を読み、処理の有無を確認したか
- **テスト:** 自作の小さな合成データで挙動を確認したか

## 1. 曲ヘッダー

| 項目 | 資料 | コード | テスト | 備考 |
|---|---|---|---|---|
| 先頭シグネチャ | 確認済み | [ ] | [ ] | `RCM-PC98V2.0(C)COME ON MUSIC` |
| 曲名の文字コード | 確認済み | [ ] | [ ] | Shift-JISとして読む |
| 解像度・基本テンポ・拍子 | 確認済み | [ ] | [ ] | `0x1C0`–`0x1C3` |
| 曲全体の transpose | 確認済み | [ ] | [ ] | `0x1C5`、符号付き値 |
| CM6 / GSD ファイル名 | 確認済み | [ ] | 対象外 | 初期スコープではCM6/GSDを読まない。将来用の情報として保持 |

## 2. トラックヘッダー

| 項目 | 資料 | コード | テスト | 備考 |
|---|---|---|---|---|
| ヘッダー長44バイト | 確認済み | [ ] | [ ] | 48バイトと誤認すると先頭イベントを読み飛ばす可能性 |
| トラックサイズ・番号・リズムフラグ・MIDIチャンネル | 確認済み | [ ] | [ ] | オフセット `+0`–`+4` |
| トラック transpose | 確認済み | [ ] | [ ] | `+5`。bit 7を含む符号・特殊値の扱いに注意 |
| トラック開始 delay | 確認済み | [ ] | [ ] | `+6`。資料では符号付きとして説明 |
| mute flag | 確認済み | [ ] | [ ] | `+7` bit 0。その他のbitをmuteと決めつけない |
| トラック名 | 確認済み | [ ] | [ ] | `+8`から36バイト |

## 3. 基本ノートイベント

| 項目 | 資料 | コード | テスト | 備考 |
|---|---|---|---|---|
| 4バイトイベント形式 | 確認済み | [ ] | [ ] | `[command, step, gate, velocity]` |
| ノート番号・step・gate | 確認済み | [ ] | [ ] | stepは次イベントまでの時間、gateはノート長 |
| gateがstepより長いノート | 確認済み | [ ] | [ ] | tieの扱いを確認 |
| 曲・トラック transposeの合成 | 確認済み | [ ] | [ ] | リズム系の例外も含めて別途検証 |
| トラック開始 delay の反映 | 確認済み | [ ] | [ ] | イベント展開時刻にどう反映するか確認 |

## 4. コマンドと時間の扱い

| コマンド | 資料の記述 | コード | テスト |
|---|---|---|---|
| `0x80`–`0x87` | ユーザーSysExを送信 | [ ] | [ ] |
| `0x90`–`0x97` | ヘッダー定義ユーザーSysEx内の置換バイトを p1/p2/channel/checksum 等へ置換 | コードあり | 合成テスト追加 | 8スロット（0x406、各0x30 byte）を解析。F0/F7とdummy抑止を検査。最新CI未確認 |
| `0xDD` / `0xDE` / `0xDF` | GSパラメータ書き込みを構成 | [ ] | [ ] |
| `0xE1` | XG 音色選択（CC32 = p2、Program Change = p1） | コードあり | 合成テスト追加 | CC32 → Program Change に展開。最新 CI は未確認 |
| `0xE2` | GS 音色選択（CC0 = p2、Program Change = p1） | コードあり | 合成テスト追加 | 同 tick で CC0 → Program Change。step は一度だけ加算。最新 CI は未確認 |
| `0xE6` | チャンネル/ポート変更（raw p1 を 1 減算して分解） | コードあり | 合成テストあり | route_change event を出力段で適用。dummy/unrouted 切替も対応。Actions #338 合格（ユーザー報告） |
| `0xE7` | 基本テンポ比によるテンポ変更 | コードあり | 合成テストあり | gradual 曲線は未対応 |
| `0xEA` / `0xEB` / `0xD2` | Control Change | D2 alias implemented | synthetic test added |
| `0xEC` / `0xD3` | Program Change | D3 alias implemented | synthetic test added |
| `0xEE` / `0xD0` | Pitch Bend | D0 alias implemented | synthetic test added |
| `0xF5` | Key signature。時間を進めない | コードあり | 合成テストあり | tick維持と同tick順序を検査 |
| `0xF6` / `0xF7` | コメント。資料では3バイトとも文字で時間を進めない | 実験的コードあり | 合成テストあり | F6後続のF7継続を除外、孤立F7は拒否。完全な仕様確定とは別 |
| `0xF8` | ループ終端。stepは時間ではなく反復回数 | 実装あり | 合成テストあり | 有限反復1〜126。0と0x7f以上は安全側で拒否 |
| `0xF9` | ループ開始。時間を進めない | 実装あり | 合成テストあり | 単純/ネスト/1回境界。対応するF8が必要 |
| `0xFD` | 小節終端。stepを時間として加算しない | [ ] | [ ] |
| `0xFE` | トラック終端 | コードあり | 合成テストあり | FEで終了し、後続レコードを処理しない |

資料では、時間を進めないコマンドのstepを通常の時間として加算すると、時間ずれが蓄積すると報告されています。各コマンドを小さな合成データで単独検証してください。

## 5. ループ・未解決コマンド

| 項目 | 資料の状態 | コード | テスト | 備考 |
|---|---|---|---|---|
| `0xFC` の参照位置 | 資料では確認済み | 実装あり | 合成テストあり | 44-byteヘッダー基準のbyte offsetを解決。実ファイル互換性は未確認 |
| `0xFC` のネスト/連鎖 | 資料では確認済み | 実装あり | 合成テストあり | FC連鎖、終端復帰、FC後のF8/F9連携を検査。仕様比較は継続 |
| `0xFC` の循環防止 | 資料では確認済み | 実装あり | 合成テストあり | 同一チェーンの訪問済み位置で循環を拒否。展開予算も設定 |
| `0x98` | **未解決** | [ ] | [ ] | `0xFC` と同一視しない。根拠なしに意味を決めない |
| `0xF8` のstepが0 | **未解決** | [ ] | [ ] | 資料では参照コンバーター固有の可能性として扱う。形式仕様と混同しない |
| `+7` のbit 0以外 | **未解決** | [ ] | [ ] | mute bitと断定しない |

### 現在のコード・合成テスト確認（2026-10-24）

以下は feature/rcp2-parser-skeleton の src/rcp.cpp、src/verify.cpp、tests/verify.txt を確認した範囲。Actions #338 はユーザー報告により合格確認。これは E6 の動的ルーティング追加までの CI 結果であり、実ファイル再生検証ではない。

| 項目 | コード | 合成テスト | 状態・制約 |
|---|---|---|---|
| 通常ノートのtick展開 | あり | あり | delayを次イベントへ累積 |
| E7基本テンポイベント | あり | あり | gradual値は保持するが、段階的テンポ曲線は未展開 |
| D0/D2/D3 MIDI alias | あり | あり | Pitch Bend / Control Change / Program Change へ対応 |
| E1/E2 bank/program | あり | あり | XG/GS bank select と Program Change を同 tick に出力 |
| DD/DE/DF GS SysEx | あり | あり | DT1 SysEx生成を合成テスト。実機再生は未確認 |
| E6 動的ルート変更 | あり | あり | channel/port/dummy切替を検査。Actions #338 合格（ユーザー報告） |
| 0x90–0x97 ユーザーSysEx | あり | あり | ヘッダーテンプレート抽出、p1/p2/channel、Roland checksum、F0/F7、dummy抑止を合成テスト。最新CI未確認 |
| 0x98 Track SysEx | あり | あり | F7継続レコードの結合、テンプレート置換、checksum、tick、孤立F7拒否を合成テスト追加。Actions #352 はこの変更前、今回の CI 未確認 |
| F5キー署名 | あり | あり | 時間を進めず、同tickの順序を検査 |
| F6/F7コメント | 実験的にあり | あり | F6と直後のF7継続を除外。完全な仕様確定を意味しない |
| EA/EB/EC/ED/EE | 一部あり | あり | 標準MIDI化できるサブセット。特殊ECは拒否 |
| F8/F9/FCループ | あり | あり | F8/F9有限反復1〜126、ネスト、1回境界、不正ペア、FC参照/連鎖/循環、FC後のF8/F9連携。step=0/0x7f以上は拒否 |
| 未対応コマンドの混在 | 拒否処理あり | あり | 有効ノートの後にF9を置いても、失敗時は出力を空にする |
| 実RCP2ファイルでの再生 | 未確認 | 対象外 | CI・合成テストの成功で代替しない |

## 6. 比較と判定の注意

- [x] 資料の記述とS-MU2000のコード確認結果を分けて記録した。
- [ ] 合成データの期待値・実際の結果・実行環境を記録した。
- [x] 既存コンバーターとの比較条件を記録した（sample-02 / sample-04 のイベント数・不一致数・同tick差を記録。全差分の原因特定は未完了）。
- [ ] 参照MIDIと異なる場合、RCPファイル自体のイベントも確認した。参照MIDIが必ず正しいとは限らない。
- [x] 同じtickのイベント順序やセットアップメッセージの配置など、JMPlayerコンバーター固有の選択をRCP形式そのものの仕様と混同していない。

## 7. 初期スコープ外

- [ ] GSDファイル解析とSC-55向けSysEx生成は、初期スコープ外として維持する。
- [ ] CM6の読み込みは初期スコープ外として維持する。
- [ ] RCP3対応はRCP2の基礎が固まってから検討する。

## 8. 完了の基準

項目を完了とする前に、少なくとも次を記録します。

1. コードの該当箇所、または未実装であること。
2. 合成テストの入力と期待結果。
3. 実際にテストを実行したかどうかと、その出力。
4. 外部資料と結果が異なる場合、その差異と未解決事項。

ビルド・実行をしていない項目は、コードを読んだだけで「テスト確認済み」にしません。


## 9. 実装済みの時系列処理（2026-10-24 時点）

以下はコード確認と合成テストの追加を確認した範囲です。署名付きオフセットのイベント種別テストを含む改行修正後の CI は、ユーザー共有の Actions 一覧で成功を確認しています。CI #234 は 2026-10-24 に成功を確認済みです。

| 項目 | 実装状況 | テスト/制約 |
|---|---|---|
| トラック内 tick 順の検査 | 実装あり | 順不同入力は失敗し、出力を空にする |
| 複数トラックの統合 | 実装あり | tick 順。同一 tick は track index 順で安定化し、トラック内順序を維持 |
| 文書レベルのタイムライン展開 | 実装あり | 現在対応しているイベントのサブセットのみ |
| 符号付きトラック開始オフセット | 実装あり | 8-bit 符号付き値として解釈 |
| 負の絶対 tick | 実装あり | tick 0 にクランプ。これが RCP2 の仕様と完全に一致するかは別途確認が必要 |
| 32-bit tick 範囲超過 | 実装あり | エラーとして展開を拒否 |
| ノート移調 | 実装あり | 参照コンバーター式を独立テストし、文書タイムラインへの適用も合成テスト |
| 未対応イベント・段階的テンポ変化 | 未対応/制約あり | 完全な RCP2 互換性を意味しない |

**最新の確認事項（2026-10-23）:** ノート移調、note gate/note-off、EB/EC/EE 部分対応、tick→秒変換、MIDI メッセージ化補助、トラック MIDI ポート/チャンネル割り当てが追加されている。Actions #189 は作業者から合格報告あり。下の項目は「コードの存在」「合成テスト」「CI合格」「実際の楽曲再生」を混同せず追跡する。

## 10. MIDI 出力接続・ポート割り当て（2026-10-23）

| 項目 | 実装状況 | テスト/制約 |
|---|---|---|
| トラックの MIDI channel/port byte | 実装あり | raw byte の下位 nibble を channel、上位 nibble を port/device として分離 |
| dummy/unrouted track | 実装あり | bit 7 が立つトラックを disabled とし、イベント出力から除外 |
| tick→秒変換 | 実装あり | PPQ/BPM/tempo event を検証。E7 の段階的テンポ変化は未対応 |
| MIDI channel message のバイト列化 | 部分実装あり | Note On/Off、CC、Program Change、Pitch Bend の範囲検証あり。実プレイヤー接続とは別 |
| MIDI Port Prefix / SMF track serialization | 未完了 | 実際の SMF ファイルを生成して読み込む処理は未実装 |
| 既存 player への RCP2 接続 | 実装あり | `src/ui/player.cpp` で RCP2 を識別し、中間表現→時系列→秒変換→port/channel割当→MIDIイベント化を経て `smf::event` へ接続。合成パイプラインのテストあり。実ファイル再生は未確認 |
| 実 RCP2 楽曲の再生検証 | 未実施 | CI 合格だけでは実再生互換性を保証しない |

参照実装の MIDI channel/port 解釈は ValleyBell/MidiConverters の `rcp2mid.c` と照合した。これは S-MU2000 実機での再生試験を意味しない。

**ループ対応状況（2026-10-24）:** F8/F9の有限反復1〜126・ネスト・1回境界、FCの参照・連鎖・終端復帰・不正参照・循環、およびFC後のF8/F9連携を実装・合成テスト済み。CI #262 成功。F8 step=0 / 0x7f以上は仕様未確定のため拒否。実RCP2ファイルでの互換性は未確認。設計・制約は [`LOOP_EXPANSION_DESIGN.md`](LOOP_EXPANSION_DESIGN.md) を参照。


## 2026-10-24 追記 — API 経路と展開予算

- [Actions #274](https://github.com/pikachu364/S-MU2000/actions/runs/40069003704) 成功確認後、`player::add_from_memory` の合成 API テストを追加。正常データが追加されること、未対応コマンドが拒否されること、拒否時に曲リストへ部分追加されないことを確認した。
- [Actions #275](https://github.com/pikachu364/S-MU2000/actions/runs/40072000688) 成功確認。F8/F9 の3重ネスト・各126回のケースで展開レコード予算を超過させ、失敗時に timeline 出力が空でエラーが設定されることを検査する回帰テストを追加した。
- [Actions #279](https://github.com/pikachu364/S-MU2000/actions/runs/40073550348) は README / NEXT_STEPS / LOG 等の文書更新後に全7ジョブ成功。
- リポジトリのファイルツリー確認では、テスト用 `.rcp` / `.mid` サンプルは見当たらない。許可された実データまたは独立参照出力が必要。
- これらは合成データの回帰テストであり、実 RCP2 ファイルの読み込み・再生互換性の証明ではない。
- 次のテスト候補: FC 展開予算超過、参照チェーンの深さ/循環境界、実データまたは独立参照出力との比較。


## 2026-10-24 追記 — D0/D2/D3 別名対応

- `D0` / `D2` / `D3` を既存の Pitch Bend / Control Change / Program Change のデコーダーへ接続した。
- `src/verify.cpp` にデコードと時間展開の合成テストを追加。CI 実行結果は次の Actions を確認してから記録する。
- 実 RCP2 ファイルでのコマンド別名の出現・再生互換性は未確認。


## 2026-10-24 追記 — E1/E2

- `E1` は XG 音色選択（CC32=p2、Program Change=p1）、`E2` は GS 音色選択（CC0=p2、Program Change=p1）として同 tick に展開する。
- `DD` は GS アドレス、`DF` はデバイス/モデル ID を保持し、`DE` は Roland DT1 SysEx（チェックサムを含む）を出力する。DD/DF は状態更新のみで、各コマンドの p0 は時間へ一度だけ加算する。
- E1/E2 の改訂と DD/DE/DF の合成テストを追加。最新の CI は未確認。実ファイルとの比較・再生も未確認。

- [x] E6 の 1-based MIDI route 値を port/channel に分解し、トラック内の後続 MIDI イベントへ動的に適用する。dummy 切替と tick 進行を合成テスト。
- [ ] E6 追加変更の GitHub Actions を確認する。
- [ ] 実 RCP2 サンプルを C++ パーサー／出力経路で検証し、チャンネル切替・SysEx の実機再生を確認する。


## 11. 2026-10-24 追記 — E6 動的ルーティングと #338

- ユーザー報告により Actions #338 の合格を確認。E6 の route-change timeline event、channel/port 切替、dummy/unrouted 切替の合成テストを含む変更までの CI 成功として記録する。
- 実 RCP2 ファイルの C++ 実行・再生互換性は未確認。
- 0x90–0x97 のヘッダー定義ユーザー SysEx を実装し、合成テストを追加した（最新CI未確認）。`0x98` は可変長データを伴うため、固定4-byteレコード前提の `parse_sequence` と FC/F8/F9 展開の相互作用を先に設計する。


## 2026-10-25 追記 — Track SysEx 0x98

- [x] 0x98 の直後に続く F7 レコードの p1/p2 を順に連結し、末尾 F7 padding を除去する。
- [x] ユーザー SysEx と共通の置換規則、checksum、E6 dummy route、p0 の tick 進行を実装。
- [x] 複数継続レコード、置換、checksum、次イベント tick、孤立 F7 の合成テストを追加。
- [ ] Actions の CI を確認する（#352 は変更前）。
- [ ] Gearmulator PR #303 と比較し、FC/F8/F9 ループ境界にまたがる F7 継続ケースを検証する。
- [ ] 実 RCP2 ファイルで C++ 実行・再生比較を行う。


### 2026-10-25 — 0x98 loop regression follow-up

- [x] F8/F9 loop body containing `0x98` and its F7 continuation emits one SysEx per expanded iteration at the expected tick.
- [x] Invalid high-bit continuation data rejects the conversion atomically.
- [ ] CI after this follow-up is pending; real RCP2 playback remains unverified.


### 2026-10-25 — 0x98 empty-payload follow-up

- [x] F7 padding-only continuation data emits no empty SysEx.
- [x] The 0x98 step still advances the following event's tick.
- [ ] CI after this follow-up is pending; real RCP2 playback remains unverified.


### 2026-10-25 — #370 failure correction

- [x] Correct the empty-payload test's expected note start tick from 5 to 2: `p0` advances time after the current record, so the note immediately after `0x98 p0=2` starts at tick 2.
- [ ] Confirm CI after the correction; actual RCP2 parsing/playback remains unverified.


### 2026-10-25 — 0x98 continuation loop-boundary follow-up

- [x] Expand `0x98` and its immediately following `F7` continuation records as one group, so continuations outside an `F8` loop cannot attach to the last repeated event.
- [x] Match reference behavior by skipping an orphan `F7` continuation.
- [x] Add a regression test for an `F7` record located after the loop terminator.
- [ ] Confirm Actions after this follow-up; actual RCP2 parsing/playback remains unverified.
- [x] Implement `0x99` external-command handling: group immediately following `F7` continuation records during F8/F9 loop expansion, emit no MIDI event, and preserve `p0` tick advancement. Synthetic regression tests are present; real-sample occurrence and payload semantics remain unverified.


## 2026-10-29 追記 — 同一tickイベント順序の参照比較

この節は、現行コード・参照コンバーター・sample-28 の比較結果を確認した範囲を記録する。**イベント順序の差は確認済みだが、根本原因は未確定。固定優先順位の追加・変更はまだ行わない。**

| 確認項目 | コード | 比較・テスト | 状態・制約 |
|---|---|---|---|
| `expand_note_gates()` による Note Off 相当イベントの展開 | 確認済み | 実RCPサンプルの比較ログと照合 | Note Off相当イベントを先に配列へ追加し、tickのみで `stable_sort`。同tickの基本順序は元配列順に依存 |
| 同一tickの優先順位条件 | 確認済み | sample-04 の差分に適用可否を照合 | CC・Note Off・新規Noteの3種類がすべてある場合のみ CC → Note Off → Note。3種類がそろわないグループには適用されない |
| Note Off相当のMIDIバイト列 | 確認済み | 現行 `src/rcp.cpp` の `encode_midi_message()` を確認 | 現行ソースは `0x9n + note + velocity 0`。以前の記録に `0x8n` とあれば現行コードの説明としては訂正が必要 |
| sample-04 / `L_WING88.RCP` | — | 22,536 events / mismatch_count=3 / exact_match=false | track 12, tick 3648：parserは CC7 → CC32 → Note On velocity 0相当、referenceは Note On velocity 0相当 → CC7 → CC32。新規Noteがないため現行の3種類条件は発動しない |
| sample-02 / `FC.RCP` | — | 20,299 events / mismatch_count=16 / exact_match=false | track 6, ticks 5135・5263などで Note On velocity 0相当とCCの順序差。他のtick/trackにも差があり、全16件は代表例だけでは説明できない |
| 参照側 running-note 期限処理 | 参照ソース確認済み | `rcp2mid.c` と `midi_utils.h` を確認 | 各RCPレコード処理前の `CheckRunningNotes()` が期限到達ノートのNote Off相当イベントを先に出力し得る。現行の事前挿入＋安定ソートとは処理タイミングが異なる |
| 問題tickの実行時状態 | 未確認 | 未取得 | `*delay`、running noteの `remLen`、処理順を問題イベントと対応付ける必要がある |
| 実RCPレコードへの完全な対応付け | 未完了 | parser-event-dumpの取得を試みたが、該当本文は空で返った | ファイルが存在しないとは断定しない。元RCPレコードのindex・byte offset・4-byte record・累積tickを別手段で対応付ける |
| 実RCP2での完全一致 | 未達 | sample-02 / sample-04 とも exact_match=false | CI/合成テストの成功と実サンプル完全一致を別々に判定する |

### この調査で確認できたこと
- [x] 現行 `src/rcp.cpp` の同一tickソート条件を確認した。
- [x] sample-02 / sample-04 のイベント数・不一致数・代表的な同tick順序差を記録した。
- [x] 参照コンバーターの `CheckRunningNotes()` / `FlushRunningNotes()` の期限処理方式を確認した。
- [x] 比較コンバーター固有のイベント出力タイミングをRCP2形式仕様と断定せず、未確定事項として区別した。
- [ ] `MidiDelayHandler` の定義とdelay更新の全経路を追跡する。
- [ ] 問題tickでの参照側 `*delay`・running note `remLen` を採取し、イベント順序差に対応付ける。
- [ ] 不一致イベントを元RCPレコードへ対応付け、原因を確定する。
- [ ] 根本原因を説明できる最小の合成回帰テストを追加し、CIを実行する。
- [ ] 修正後に sample-02 / sample-04 の比較を再実行し、不一致数と完全一致状態を確認する。

**合格条件:** 原因を元レコードと参照側の期限状態まで説明できること。修正を行う場合は、根拠に対応した最小変更と回帰テストを用意し、合成テスト・CI・実サンプル比較を別々に記録する。原因確定前に「CC優先」「Note Off優先」などの固定ルールを追加しない。


## 2026-10-29 — 同一tick順序差分の追跡項目

- [x] private Action #42 artifact のZIP整合性を検査し、31ファイルを確認。
- [x] sample-01〜04 の parser/reference イベント数が一致し、不一致数が 627 / 16 / 598 / 3 であることを比較レポートで確認。
- [x] sample-04 Track 12 / tick 3648 の順序差を特定: parser `BC 07 61 → BC 20 00 → 9C 36 00`、reference `9C 36 00 → BC 07 61 → BC 20 00`。
- [x] 現行ソースを確認し、同tickの特別な優先順位処理は CC、Note Off、新規 Note が全て同居する場合に限定されることを確認。
- [ ] sample-04 の3イベントを元 RCP のレコード index / byte offset / 累積 tick に対応付ける。
- [ ] 対象 Note Off の生成位置・残り duration と、2つの CC の元レコード順を参照コンバーターの処理と照合する。
- [ ] sample-02 の代表差分でも同じ原因かを確認する。
- [ ] 原因に対応する最小修正と回帰テストを追加し、CIとprivate sample-checkを実行する。
- [ ] 実際の再生確認は別途実施した場合のみ記録する。

制約: 上記の比較・ソース確認は、元レコードへの完全な対応付けや原因確定を意味しない。今回コード修正・CI再実行・実再生は行っていない。


## 2026-10-29 follow-up — SysEx 0xFF behavior in S-MU2000

- [x] Source review: src/rcp.cpp 0x98 expansion explicitly skips unknown high-bit template bytes (example 0xFF), matching reference converter behavior.
- [ ] Restore full SysEx sequences and evaluate original-data intent.
- [ ] Verify any relation to event-order mismatch before considering changes.

**Status:** No code change. This is currently a reference-converter warning for behavior S-MU2000 already appears to mirror.


### 2026-10-30 追記 — 0x98 SysEx 内の未知高ビット値

- ValleyBell の `RCPFormat.txt` で確認できるテンプレート制御値は `0x80`–`0x84`。今回の `0xFF` を別値へ置換する規則は確認できていない。
- `0x98` のペイロードはコマンドレコードの `p1/p2` ではなく、後続 `0xF7` 継続レコードから読む。Gearmulator の現行実装にもこの規則が明示されている。
- S-MU2000 と ValleyBell は未知の高ビット値をスキップする。これはソース上の挙動一致であり、元データの意図、チェックサムの妥当性、実機での動作を証明しない。
- **状態:** 形式資料とコードの照合は完了。private サンプルの出力 SysEx 比較と実機再生は未実施。根拠が見つかるまでは値の置換・チェックサム自動修正を行わない。


### 2026-10-30 — SysCheck error と後続イベント処理の切り分け

- [x] Gearmulator の RCP構造解析における致命的エラー条件を確認。
- [x] `0x98` SysExテンプレート中の未知高ビット値がスキップされ、展開ループが継続することを確認。
- [x] 空の SysEx展開結果が track expansion 全体の失敗にならない経路を確認。
- [x] 正常な `0x98` と `0xF7` 継続レコードの既存 unit test を確認。
- [ ] 88emu の SysCheck error の正確な表示文言・発生段階を特定。
- [ ] エラー表示後も後続 MIDI イベントが出力されるか確認。
- [ ] 未知高ビット値を含む最小合成テストを追加する必要性を判断。
- [ ] SysCheck の結果と RCP parser の成功/失敗を別々に記録する。

**状態:** parser 内の継続経路はソース確認済み。88emu の実際の SysCheck error と同一原因か、実再生でどう振る舞うかは未確認。


### 2026-10-30 追記 — Roland DT1 "Check Sum Error" と後続再生

- [x] Gearmulator SC-88 sub-MCU の Roland DT1 checksum 検査を確認。
- [x] checksum 不一致時に `0xE0 / param1=3` ("Check Sum Error") 通知を発生させ、当該 DT1 の転送処理だけを拒否することを確認。
- [x] SysEx 終端後に組み立て状態をクリアし、後続 MIDI 入力を処理できることを確認。
- [ ] ユーザーが見たエラーの正確な表示文言を確認。
- [ ] 今回の3件の SysEx が当該 checksum error を起こすか private 環境で照合。
- [ ] エラー後の後続ノートが 88emu で再生されることを今回の対象データで確認。

**状態:** 「checksum error が出ても曲を聴ける」動作を説明するソース根拠を確認。対象データとの対応は未確認。
