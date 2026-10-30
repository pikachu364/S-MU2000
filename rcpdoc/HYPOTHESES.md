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


## 2026-10-29 — sample-02 Track 6 の残存順序差分（未確定）

**状態:** 最新比較では4件まで減少。直接原因は未確定。

private sample-check run [40868277263](https://github.com/pikachu364/S-MU2000-private-data/actions/runs/40868277263) の比較レポートでは、sample-02 `FC.RCP` の Track 6 に次の差分が残る。

- tick 5135: parser `96 3C 00 → B6 0B 7F`、reference `B6 0B 7F → 96 3C 00`
- tick 5263: parser `96 3C 00 → B6 0B 17`、reference `B6 0B 17 → 96 3C 00`

`96 3C 00` は Note On velocity 0（Note Off相当）、`B6 0B xx` は CC。両イベントの tick は一致しており、イベント欠落ではなく同一tick内の順序差である。現行の固定優先順位を外した変更により、4サンプル中3サンプルが完全一致し、sample-02の不一致は16件から4件へ減少した。

**次の検証**
1. private runner の raw-record dump から Track 6 / tick 5135, 5263 の各イベントに対応する元RCPレコード、record index、byte offset、step、累積tickを確定する。
2. 期限切れノートの開始tick・gate/remaining lengthと、ValleyBell `CheckRunningNotes()` / `MidiDelayHandler()` が Note Off を出力するタイミングを照合する。
3. 修正案はこの2箇所だけに過剰適合させず、4サンプル全件で再比較する。

**制約:** この差分だけでは RCP2 形式全体の仕様を確定できず、参照コンバーターが常に唯一の正解であることも意味しない。実機再生は未確認。


## 2026-10-30 — RCP2 SysEx 0xFF の仕様照合結果

### H-RCP-SYSEX-FF-20261030-C

**状態:** 「RCP2 の既知テンプレート制御値ではない」ことは参照形式資料で確認。元データの意図と最終的な音源動作は未解決。

**確認できた根拠**
- ValleyBell の `RCPFormat.txt` で `0x98` のペイロードは後続の `0xF7` 継続レコードにあり、既知のテンプレート制御値は `0x80`–`0x84`。今回の `0xFF` を別値へ置換する規則は記載されていない。
- Gearmulator の現在の `rcpTrack.cpp` は `0x98` のペイロードを後続 `0xF7` 継続レコードから組み立てる。従って `0x98` の `p1/p2` をペイロード先頭と誤読する古い経路とは区別が必要。
- ValleyBell と現在の Gearmulator のテンプレート展開は、未知の高ビット値をスキップする。S-MU2000 の `0x98` 展開も同じ基本動作。

**結論の範囲**
- `0xFF` を `0xFE` 等へ置換する根拠はない。
- スキップ処理は既存実装に沿うが、スキップ後の固定チェックサムが有効か、対象のSysExが音源に必要かは別問題。
- ValleyBell の MIDI 出力とイベント列が一致しても、生成された SysEx の意味的正しさや実機での反応までは証明しない。

**次の確認**
1. private 環境で該当3件の `0x98` と後続 `0xF7` 継続レコードを、元ファイルの offset・record index と対応付ける。
2. 同じ入力を S-MU2000 と ValleyBell で展開し、3件の出力 SysEx を private 環境内で比較する。生データは公開リポジトリに転記しない。
3. 元RCP作成元の説明書・別実装に明確な規則が見つからない限り、置換・チェックサム自動修正・警告抑制は行わない。

**制約:** 現時点では形式資料とソースレビューのみ。実機再生の確認なし。


## 2026-10-30 — SysCheck error があっても 88emu で再生できる可能性

### H-RCP-SYSCHECK-20261030-A

**状態:** 一部の parser 処理継続はソースで確認。88emu の実際の SysCheck error 表示と再生継続の関係は未確定。

**仮説**
88emu で SysCheck error が表示されても音楽を聴ける場合、検査エラーと RCP 全体の読み込み・イベント再生が別段階であり、問題のある SysEx のみを無視して後続イベントを処理している可能性がある。

**確認できた範囲**
- Gearmulator の RCP decoder は構造上の致命的エラーでは全体を失敗させる。
- ただし `0x98` SysEx テンプレートの未知高ビット値はスキップされ、テンプレート展開は継続する。これだけでは RCP 全体の読み込み失敗にならない。
- 正常な `0x98` / `0xF7` 継続レコードを確認する unit test はあるが、未知高ビット値を含む回帰テストは今回確認できていない。

**未確認**
- 88emu の「SysCheck error」が RCP parser、SysEx生成、音源ファームウェアの checksum 検査のどの段階の表示か。
- エラー発生後に同じファイルの後続トラック・後続イベントがどこまで処理されるか。
- 音が鳴る場合に、該当 SysEx のみが無視されるのか、別の初期化経路・以前の音源状態で再生されるのか。

**次の検証**
1. 88emu のログ/UI表示の正確な文言と発生タイミングを記録する。
2. 88emu の RCP parse / MIDI event expansion / SysEx dispatch の責務をソース上で分けて追う。
3. 問題の SysEx 前後にあるノート・制御イベントが出力されるか、ログまたは MIDI event dump で確認する。
4. SysCheck が音源ファームウェア側の checksum 判定なら、RCP parser の読み込み成功とは別項目として扱う。

**制約:** 「エラー表示があるのに再生できる」観測だけから、エラーを無視している・SysEx が安全にスキップされるとは断定しない。現時点で S-MU2000 のソース変更は行わない。


### H-RCP-SYSCHECK-20261030-A — 重要な追加根拠

Gearmulator の SC-88 sub-MCU 実装では、Roland DT1 checksum 不一致時に `0xE0 / param1=3` の "Check Sum Error" 通知を発生させ、その DT1 処理だけを拒否して return する。SysEx assembly は `F7` 受信時にクリアされ、後続 MIDI byte の入力処理は別途続く。

このため、ユーザー観測がこの "Check Sum Error" を指すなら、「エラーが出ても 88emu で曲を聴ける」ことは自然に説明できる。エラーは曲全体の再生停止ではなく、個別 DT1 の不受理通知と考えられる。

**まだ確認が必要:** ユーザーが見た表示の正確な文言が "Check Sum Error" かどうか。今回の RCP2 3件の SysEx がこの通知を発生させるかどうか。対象メッセージの checksum が不正と確定するまでは断定しない。



## H-RCP-SYSCHECK-20261030-D — Check Sum Error と今回の短い SysEx

**状態:** Gearmulator のコード上、今回の3件が Check Sum Error を発生させる仮説は否定的。

- SC-88 sub-MCU は DT1 body が9 bytes未満なら checksum 検査前に無通知で return する。
- 今回の3件は F0/F7 を除く出力bodyが各8 bytesのため、この経路では Check Sum Error にならない。
- Check Sum Error は checksum 不一致時の別経路（OpSysExError / param1=3）で発生する。
- ユーザーは過去に Check Sum Error 表示後も演奏を聴けたと報告している。これは当該DT1の拒否と曲全体の停止が別であることと整合するが、今回の3件が表示原因という意味ではない。
- 次は private sample 内の他のDT1 SysEx、または実際に表示された時の入力イベントを調べる。データ修正は根拠が得られるまで行わない。



## H-RCP-SYSCHECK-20261030-E — sample-04 のDT1 checksum棚卸し

**状態:** 保存済みsample-04 event dumpで静的確認済み。

- Roland DT1を120件抽出。うち117件（body長9 bytes以上）はchecksum合計0、3件はbody長8 bytesでchecksum byteなし。
- Gearmulator SC-88 sub-MCUの条件に照らすと、117件はchecksum error条件に該当せず、3件はchecksum検査前に無通知returnする。
- よって、保存済みsample-04 event dump上ではCheck Sum Errorを起こすDT1は確認されなかった。
- この結果は過去にユーザーが別の曲・別の入力で見たエラーの原因を否定するものではない。実再生試験とも区別する。


## H-RCP-SYSCHECK-20261030-F — 4 sample全体のDT1 checksum棚卸し

**状態:** 保存済みsample-01〜04 event dumpの静的検査完了。

- Roland DT1 503件のうち、500件はGearmulatorの最低長条件を満たし、checksum合計0。
- 3件はbody長8 bytesでchecksum byteがなく、Gearmulatorではchecksum検査前に無通知return。
- checksum不一致は0件。したがって、保存済み4 sampleのevent dump内にCheck Sum Error通知条件に該当するDT1は見つからない。
- 過去のエラー表示は別曲・別のSysEx・別の実行条件で発生した可能性が残る。静的検査結果を実機/88emu再生結果と同一視しない。


## H-RCP-SYSCHECK-20261030-G — Check Sum Error 後に演奏・後続SysExが続く理由の追加根拠

**状態:** 後続処理が継続する実装根拠を追加確認。過去にユーザーが見たエラーの発生源は未特定。

- Gearmulator の SC-88系統合テスト sc88_system_dt1_test.cpp は、意図的に checksum を壊した DT1 を送った後、同じパラメータへの正常な DT1 を送信し、前者は反映されず後者は反映されることを確認するテスト構成。
- これは checksum error が当該 DT1 の拒否であり、後続の SysEx 処理まで一律に停止するものではないことを補強する。テストソースの確認であり、今回の実行結果ではない。
- Gearmulator changelog には SC-55mkII の高密度 SysEx 時に「Check Sum Error」「DT1 Data Error」が誤って出るタイミング競合の修正記録がある。sub-MCU が同一 byte を二重に main CPU へ渡す競合であり、実際の送信 checksum 不正とは別原因。
- SC-55mkII の既知不具合を SC-88 / SC-88Pro にそのまま一般化しない。今回のサンプルで発生した証拠もない。

**次の検証**
1. 過去のエラー表示の正確な文言・機種・曲・発生タイミングが分かる場合に、DT1 checksum 不一致と高密度 SysEx のどちらに近いか照合する。
2. 新しい sample の DT1 棚卸しでは、checksum の静的妥当性に加え、同一tick / 短時間内の SysEx 密度も記録できるか検討する。
3. 88emu 実再生試験を行った場合は、静的 dump 検査・ソースレビュー・統合テストソース確認と分けて結果を記録する。

**制約:** 過去の表示原因は未確定。SC-55mkII の changelog 記録は、SC-88系や対象RCPの不具合を示すものではない。


## H-RCP-SYSCHECK-20261030-H — sample-04 の高密度SysExがエラー表示に関係する可能性

**状態:** 時間密度の静的集計で調査対象を絞り込んだ段階。原因は未確定。

- sample-01〜04 の parser event dump を比較すると、100ms窓の最大DT1件数は順に 5 / 4 / 6 / 26 件、同一tickの最大DT1件数は 1 / 1 / 2 / 5 件。
- sample-04 の最大100ms窓には 0x10 系アドレスを含む複数種のDT1が含まれる。Gearmulator changelog にある SC-55mkII の display dot animation 等の高密度SysEx問題と照合する手掛かりにはなる。
- しかし、sample-04 が display dot animation を実行している、またはSC-55mkIIの既知競合を起こす、とまでは確認できていない。
- event dump のtimestampは変換後イベントの時間であり、音源に到達するwire timingやsub-MCU / main CPU間の実行タイミングを再現しない。

**次の検証**
1. sample-04 の高密度区間に含まれる 0x10 系DT1の意味を、Gearmulatorの表示系アドレス処理と照合する。
2. 必要なら private 環境で同区間の実際の送信byte数・wire durationを推定し、イベントtimestampだけの密度と区別する。
3. 他sample・新規sampleでも同じ集計を行い、sample-04固有かどうか比較する。
4. 88emu実再生でエラーが再現するか確認できた場合のみ、原因候補として強める。

**制約:** 密度の相関だけでエラー原因と断定しない。SC-55mkIIの既知不具合をSC-88系へ一般化しない。


## H-RCP-SYSCHECK-20261030-I — sample-04高密度区間はパート受信設定を含む混在SysEx列

**状態:** 一部の用途を公式資料で確認。全体の役割・エラーとの因果は未確定。

- 最大100ms窓の26件中8件はmodel ID 0x56、address 01、data 10のパート受信OFFメッセージとRoland公式資料で照合できる。
- よって、これら8件はSC-88表示ドット描画ではない。パート受信OFF設定が連続することで短時間密度を上げている。
- 同区間にはmodel ID 0x42および0x16のDT1も混在する。model ID 0x16のaddress 0x10系をSC-88表示データとみなす根拠はない。GearmulatorのSC-88表示経路はmodel ID 0x45とaddress 0x10系を条件としている。
- SC-55mkIIの高密度SysExタイミング競合は別機種の既知修正であり、今回のsampleの原因と断定しない。

**次の検証**
1. sample-04のmodel ID 0x16メッセージのアドレス体系・機能を、D-110 / MT-32互換系の一次資料で確認する。
2. model ID 0x56の全メッセージについて、公式のPart Rx On/Off形式と照合し、ON/OFFの区別を集計する。
3. 新規sampleでもmodel IDとcommand/addressを分けて集計し、addressだけで表示系と分類しない。
4. Check Sum Errorとの因果は、対象機種・実再生・実送信条件が確認できるまで未確定のままにする。
