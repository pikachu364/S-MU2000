# 仮説・未解決事項

確認済み事実と混ぜず、現時点で未解決の点、確認方法、完了条件を記録します。

## H-001 — 0xFE 終端後の不完全な断片を無視できるか

**状態:** 未検証。テストデータの再確認が必要。

`0xFE` がトラック終端として処理され、その後ろに不完全なデータがあっても解析が終端で停止することを検証する。

**確認方法:**
- テスト配列で終端レコードと後続断片が重なっていないか確認する。
- 終端の後ろに独立した不完全な断片を配置する。
- ビルドして `verify.exe` を実行し、イベント数・消費バイト数・エラー状態を確認する。

**完了条件:** テストデータの配置が正しいことをコード上で確認し、実行結果をログに記録する。

## H-002 — ローカル実装と GitHub ブランチを安全に統合できるか

**状態:** 未確認。

過去のローカル作業には GitHub 側と異なる `src/rcp.cpp`、`src/rcp.h`、ビルド設定、テスト変更が含まれていた。内容を比較するまでは同じ実装だと仮定しない。

**確認方法:**
- パソコンが使えるとき、現在のブランチ・作業ツリー・差分を確認する。
- 未コミット変更を保持したまま、ローカルと GitHub の各ファイルを比較する。
- 必要な変更だけを選び、統合前に差分をレビューする。

**制約:** 未コミットの作業を破棄、上書き、リセットしない。許可なく `main` を変更しない。

## H-003 — RCP2 イベント仕様の未確認部分

**状態:** 調査・テストが必要。

ノート、テンポ変更、ループ、ユーザー SysEx などについて、既存ツールとの挙動比較が必要。ここでは未確認の仕様を確定事項として扱わない。

**確認方法:**
- 各イベントの小さな合成データを作る。
- 既存変換ツールの出力と比較する。
- 比較条件・入力・観測結果・差分を記録する。

## H-004 — RCP3 拡張時の境界

**状態:** 設計方針のみ。実装設計は未完了。

RCP2 を優先しつつ、将来 RCP3 を追加するときにファイル形式の違いを解析層で扱い、MIDI イベントへの展開処理との結合を避ける。

**完了条件:** RCP2 の基本解析を固めてから、RCP3 との差分と共通インターフェースを具体化する。

## H-005 — JMPlayer の 44-byte track header を S-MU2000 に照合する

**状態:** 未検証。

JMPlayer の資料は RCP track header を44バイトと記述している。S-MU2000 の現行パーサーが同じ前提か、実装とテストを確認する。

**確認方法:**
- 現行コードで track header の読み取り長とイベント開始位置を確認する。
- 合成データで最初のイベントが欠落・誤読されないか検証する。
- 参照資料の記述と実装結果を別々に記録する。

## H-006 — トランスポーズ、トラック開始遅延、ミュートの扱い

**状態:** 未検証。

JMPlayer 資料では song transpose (`0x1C5`)、track transpose (`+5`)、track delay (`+6`)、mute flag (`+7` bit 0) を説明している。これらが S-MU2000 の目標範囲にどう関係するか、現行実装が処理しているかを確認する。

**確認方法:**
- ヘッダー読取とイベント展開の責務を分けてコードを確認する。
- 自作の最小データで各値を単独に変え、期待するノート番号・時刻・ミュート挙動を確認する。
- 未確認のフィールド値について、外部資料だけで確定扱いにしない。

## H-007 — 時間を進めないコマンドと 0xFC repeat

**状態:** 未検証。優先度高。

JMPlayer 資料では `0xF5`、`0xF6`/`0xF7`、`0xF8`/`0xF9`、`0xFD` などについて、通常の step 加算とは異なる扱いを報告している。`0xFC` はトラック内のバイト位置を参照する複雑な repeat と説明されている。`0x98` は `0xFC` と同じものではなく、意味は未確定とされる。

**確認方法:**
- まずイベントを読み取る層と時間・ループを展開する層を分けて確認する。
- 各コマンドの最小合成テストを作る。
- `0xFC` は単純な measure number として解釈せず、資料の説明を独立に検証する。
- `0x98` は意味を推測して `0xFC` と同じ処理にしない。

**完了条件:** 各コマンドの観測・期待結果・根拠が記録され、未解決の部分は未解決のまま残されている。


## H-008 — RCP2 のイベント列を既存 player へ接続する方式

**状態:** 設計検討中。実装未完了。

`src/ui/player.cpp` は現在 SMF を読み込む経路で、RCP2 の document/timeline event を直接再生する経路は確認できていない。一方、RCP2 側には秒単位への変換と MIDI message byte 化、track port/channel mapping の補助処理が存在する。

**検討する案:**
- RCP2 event を `smf::event` 相当の内部列に直接変換して player の再生ループに渡す。
- SMF serializer を実装し、既存の SMF loader に戻して再生する。

**判断基準:**
- 現在の `player` / `bridge` の所有権・タイミング・port mapping を壊さない。
- 同 tick のイベント順、tempo event、port/channel mapping、dummy track の除外を維持する。
- 未対応イベントを黙って落とさず、明示的に拒否する。
- 合成テストと CI を通し、実 RCP2 ファイルでの再生確認とは別に記録する。


## 2026-10-29 — sample-04 同一tick順序差分の仮説（未確定）

### 仮説 H-RCP-ORDER-20261029-A

sample-04 Track 12 / tick 3648 の CC7、CC32、Note On velocity 0 の順序差は、Note Off相当イベントを先に生成して並べ替える参照コンバーターと、現行S-MU2000の Note Gate 展開後に tick のみで stable sort する方式の差に関係する可能性がある。

**根拠**
- parser: `BC 07 61 → BC 20 00 → 9C 36 00`
- reference: `9C 36 00 → BC 07 61 → BC 20 00`
- 現行の特別な順序変更条件には、この「CC2件 + Note Off相当」だけの組み合わせは含まれない。
- ValleyBell参照コードの `CheckRunningNotes()` はレコード処理・イベント出力の経路に結び付いているため、Note Off発行時点の違いが順序差につながる可能性がある。

**未確認**
- 問題イベントの元 RCP レコード index / byte offset と同tickの生レコード列。
- 該当ノートの実際の開始 tick、duration / remaining length、期限判定の値。
- この仮説が sample-02 の16件にも適用できるか。

**検証方法**
1. private runnerで event dump に元RCPの track、record index、byte offset、raw 4-byte record、累積 tick を関連付ける。
2. sample-04の該当レコードについて、参照側の running-note期限判定と出力順を追う。
3. sample-02でも同じ順序生成経路かを照合する。
4. 証拠が揃ってから修正案・回帰テストを決める。

**制約:** これは仮説であり、直接原因として確定していない。現時点でイベント種別に基づく固定優先順位を追加する根拠にはしない。


## 2026-10-29 follow-up — source behavior for SysEx 0xFF

### Hypothesis H-RCP-SYSEX-FF-20261029-B

The sample-04 warning may be informational rather than evidence of an S-MU2000 incompatibility: S-MU2000's current 0x98 expansion loop explicitly skips unknown high-bit template bytes such as 0xFF, matching the reference converter's ProcessRcpSysEx() behavior.

**Evidence:** source review of S-MU2000 src/rcp.cpp (0x98 expansion loop) and reference converter rcp2mid.c at commit a73c748960d57bd194fb64931ce07d2c1fa1bd01.

**Not established:** original author intent for these 0xFF bytes, runtime byte-for-byte output equivalence for these exact sequences, and any causal link to event ordering differences.

**Next check:** reconstruct the complete three SysEx payloads and only add a test if there is a concrete expected behavior not covered by the current source.
