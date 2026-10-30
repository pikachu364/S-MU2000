# 確認済み事実

ここには、根拠が追跡できる情報だけを記録します。履歴上の観測と、現在の実行で再確認した結果を区別します。

## F-001 — 開発方針

**状態:** 作者との Discussion に基づく方針

- まず RCP2 系（.rcp）を対象にする。
- 将来の RCP3 対応を考慮し、ファイル解析と MIDI イベントへの展開を分離する。
- CM6/GSD の読み込みは初期段階では対象外とする。
- 既存の変換ツールは挙動比較の参考にし、コードを流用しない。
- テストには小さな自作の合成データを使い、第三者の楽曲データをリポジトリに追加しない。
- 調査結果と未確認事項をドキュメントに記録する。

**根拠:** [作者との Discussion #150](https://github.com/tarboh/S-MU2000/discussions/150)

## F-002 — GitHub 上の作業先

**状態:** リポジトリ URL と作業履歴で確認

- リポジトリ: pikachu364/S-MU2000
- 作業ブランチ: feature/rcp2-parser-skeleton
- 本ドキュメント群の保存先: rcpdoc/

## F-003 — 2026-10-15 のレポートで記録されたローカルテスト

**状態:** 過去レポートに記録あり。今回の実行では未再確認。

過去レポートには次の結果が記録されている。

- make build/verify.exe：成功
- RCP2 basic playback parser：OK
- RCP2 FE terminator：OK

ただし、同じレポートで 0xFE 終端テストのデータ構成に問題がある可能性が指摘され、合格を確定扱いにしないよう記録されている。

**根拠:** [2026-10-15 作業レポート](reports/rcp2-local-progress-2026-10-15.md)

## F-004 — 2026-10-16 にテストコードを GitHub へ追加

**状態:** コミット記録あり。ビルド・実行は未確認。

- コミット: [ace8ce8](https://github.com/pikachu364/S-MU2000/commit/ace8ce8f648d81fc3b726a02894fd744b2168125)
- 内容: src/verify.cpp に、0xFE の後ろに不完全な断片がある場合のテストを追加した。
- テストコードが存在することと、そのテストが実行時に正しく動くことは別である。

## F-005 — 作業レポートを GitHub に保存済み

**状態:** ファイル作成・再取得で確認済み。

- [2026-10-15 レポート](reports/rcp2-local-progress-2026-10-15.md)
- [2026-10-16 レポート](reports/rcp2-work-log-2026-10-16.md)

## F-006 — 現在の作業環境に関する制約

**状態:** 作業者からの申告に基づく。

- iPad mini 7 から GitHub 上の資料確認・編集を進められる。
- ビルドやローカルテストは、パソコンの開発環境が使えるときに行う。
- 過去に確認されたローカルの未コミット変更は GitHub の内容と同一とは限らない。ローカル側を上書き・リセットしない。

## F-007 — JMPlayer の RCP 形式資料を調査した

**状態:** 外部資料の内容を閲覧して確認。S-MU2000 の実装・テストでの再現は未確認。

- [JMPlayer RCP format notes](https://github.com/jihodaya/jmplayer/blob/main/docs/formats/en/rcp.md) は、RCP のヘッダー、44-byte track header、4-byte event、イベントコマンド、0xFC repeat、0xFE end-of-track などを説明している。
- 同資料は、参照 MIDI との比較対象303曲のうち、225曲が全チャンネルでノート単位一致、66曲は参照 MIDI の末尾に追加素材がある形で冒頭が一致、12曲は差異が残ると報告している。これは資料の著者による報告であり、S-MU2000 の検証結果ではない。
- 同資料では、0x98 の意味、トラックヘッダー +7 の bit 0 以外、いくつかの曲の差異などが未解決として挙げられている。
- [JMPlayer rcpfilehandler.cpp](https://github.com/jihodaya/jmplayer/blob/main/rcpfilehandler.cpp) には、RCP イベント処理だけでなく、GSD から GS SysEx を組み立てる処理も含まれている。
- この資料を仕様候補・比較対象として利用し、コードをコピーしない。S-MU2000 側で確認するまでは、記述を自プロジェクトの確定事実に昇格させない。

詳細: [JMPlayer RCP 調査ノート](references/jmplayer-rcp-notes.md)

## F-008 — RCP2 FE 終端テストを強化し、ローカル実行で成功を確認

**状態:** GitHub 上のテストコード構造は確認済み。2026-10-20 のローカル実行成功は作業者共有ログで確認。GitHub ブランチ上での同一ビルド・実行は未確認。

- テストコードの変更コミット: [53d8536](https://github.com/pikachu364/S-MU2000/commit/53d8536496e844a9cf38c4619ec3c3959de140b5)
- テストでは FE の後ろに、有効なノートレコード（61, 0, 48, 100）を宣言済みトラック範囲内に置き、FE までの2イベントのみが解析結果に含まれることを検査する。
- 作業者が共有した `make check` の端末出力には、`RCP2 basic playback parser: OK` と `RCP2 FE terminator: OK` が表示されていた。
- 同じ出力には DLL / verify.exe のビルドと `VSTi probe passed: S-MU2000, stereo, MIDI, state, editor` も含まれていた。
- 根拠はユーザー共有のローカル端末ログであり、実行環境の完全な再現情報や終了コードを独立に確認したものではない。実際の RCP2 楽曲ファイルでの互換性や MIDI 再生は未確認。

## 更新ルール

新しい事実を追加するときは、確認方法・根拠リンク・制約を記録します。過去のテスト結果を現在の検証結果として再利用しません。

## F-009 — mute flag の回帰テストを追加

**状態:** GitHub 上のコミット履歴でテスト変更を確認。CI の最終結果は未確定。

- トラックヘッダーの mute flag（+7 bit 0）を対象とするテストを追加・修正した。
- テスト結果への反映と、回帰時に検証プログラムが失敗する構成にした。
- 関連コミット: [06dac4f](https://github.com/pikachu364/S-MU2000/commit/06dac4ff7b1dfca9cda1aaa61ba67f97b281ed9c)
- コミットが存在することは、CI でテストが成功したことや、実際の楽曲での mute 動作を確認したことを意味しない。

## F-010 — GitHub Actions の再有効化後に自動実行を確認

**状態:** 自動起動を確認。実行全体の結果は確認時点で未確定。

- コミット: [dfd7917](https://github.com/pikachu364/S-MU2000/commit/dfd79177305791855085708521373695ab299314)
- 実行: [build #78](https://github.com/pikachu364/S-MU2000/actions/runs/39424636776)
- `feature/rcp2-parser-skeleton` への新しい push で `build` が自動起動した。
- 確認時点では一部ジョブが実行中だったため、全体の成功・失敗は未確定。


## F-011 — 複数トラックの時系列統合と文書レベル展開

**状態:** GitHub 上のコミット履歴で宣言・実装・テスト・期待出力更新を確認。fixture 修正後の GitHub Actions build #101 が成功。

- merge_timed_tracks は、各トラックの tick 順を検査し、tick 順でイベントを統合する。同一 tick では track index 順を使い、トラック内の順序を保持する。不正な入力では失敗し出力を空にする。
- 文書レベルの展開処理は、現在対応しているイベントの範囲を各トラックで展開し、トラック開始オフセットを適用してから統合する。未対応コマンドや段階的テンポ変化など、対象外の範囲が残っている。
- トラック開始オフセットは符号付き 8-bit tick として解釈する。負の tick は 0 にクランプし、32-bit tick 範囲超過はエラーとして扱う。
- CI では、オフセットテスト追加後に「複数トラック統合/トラック識別」と「符号付き開始オフセット」が NG になる期待出力不一致を観測した。テスト fixture の最初のノートの step を 5 に直し、次のノートのローカル時刻が tick 5 になるよう修正した。
- 最新の fixture 修正コミット: [83d213c](https://github.com/pikachu364/S-MU2000/commit/83d213c42eace8e882446e80d78aeee4c8ab41e0)
- CI 根拠: [build #101](https://github.com/pikachu364/S-MU2000/actions/runs/39571008073) は `83d213c` で completed successfully。ユーザー共有の Actions 一覧に基づく記録。
- **制約:** CI 成功は、合成テストと対象ブランチのビルドが通ったことを示すが、実際の RCP ファイルでの再生互換性や全コマンド対応を証明しない。


## F-012 — 符号付きトラック開始オフセットの端点テストを追加

**状態:** テストコードと期待出力を GitHub に追加し、baseline 更新後の CI 成功をユーザー共有の Actions 一覧で確認。

- `0x80` を signed 8-bit の -128 として適用した場合、tick 0 未満のイベントが 0 にクランプされることを検査する。
- `0x7f` を +127 として適用した場合、ローカル tick 0 のイベントが tick 127 になることを検査する。
- テスト: [9d0d183](https://github.com/pikachu364/S-MU2000/commit/9d0d183dacf14f6cf95cf31574dc0eed8dc122de)。期待出力: [762ee46](https://github.com/pikachu364/S-MU2000/commit/762ee46ed00adb1d548d150319b89d287dea2bc7)。
- CI [build #116](https://github.com/pikachu364/S-MU2000/actions/runs/39589682748) は古い期待出力との不一致で失敗した。実際の出力には新しい「オフセット境界 合」が含まれていたが、baseline が未更新だった。
- 期待出力更新 [762ee46](https://github.com/pikachu364/S-MU2000/commit/762ee46ed00adb1d548d150319b89d287dea2bc7) 後の [build #117](https://github.com/pikachu364/S-MU2000/actions/runs/39589690157) と後続 build #118〜#120 は completed successfully。
- **制約:** CI 成功はこのブランチの合成テスト・ビルド結果であり、実際の RCP2 ファイルでの再生互換性や全コマンド対応を証明しない。


## F-013 — トラック開始オフセットを全イベント種別で検査するテスト

**状態:** テストコード・期待出力・修正後の CI 成功をユーザー共有の Actions 一覧で確認。

- 文書レベル展開の回帰テストで、正のトラック開始オフセットがノート、F5 キー署名、E7 テンポの各イベントの tick に適用されることを検査する。
- テスト変更: [2ed0c96](https://github.com/pikachu364/S-MU2000/commit/2ed0c9657b0fc69dda43794233d3539bea0a04d9)。期待出力更新: [d13e84c](https://github.com/pikachu364/S-MU2000/commit/d13e84c08a0003c3cee30d09fc8a7815e0905833)。
- 32-bit tick 上限チェックは実装に存在する。RCP2 のトラック長フィールドが 16-bit であるため、通常のパース済みトラック長からは delay 累積だけで 32-bit 上限を超えない。巨大な合成入力を作るテストは費用対効果が低く、未実施。
- 改行エスケープ修正: [a9d5a61](https://github.com/pikachu364/S-MU2000/commit/a9d5a61e17223a0428257441f82054122ad30e6b)。ユーザー共有の Actions 一覧では修正後の build #129 と後続 build #130 が completed successfully。
- **制約:** CI 成功は対象ブランチでのビルド・合成テスト成功を示すが、実際の RCP2 ファイルの再生互換性や全コマンド対応を証明しない。


## F-014 — ノート移調の参照実装準拠ヘルパー

**状態:** 実装・合成テスト・文書レベル展開への統合を追加。GitHub Actions build #160 の成功を確認。

- ValleyBell/MidiConverters の RCP2 ハンドラーを参照し、トラック移調バイトの bit 7 が立つ場合はリズムチャンネル扱いとして移調を抑止する。通常トラックでは bit 6 を符号ビットとする signed 7-bit トラック移調に、文書ヘッダーの signed 8-bit 移調を加算する。
- 最終ノート番号は参照実装と同様に `& 0x7f` で 7-bit 範囲へ折り返す。入力ノート自体が 0x7f を超える場合は拒否する。
- テストでは負のトラック移調、負の文書移調、リズムチャンネルでの抑止、7-bit 折返し、範囲外ノート拒否を検査する。
- 参照: [ValleyBell/MidiConverters rcp2mid.c](https://github.com/ValleyBell/MidiConverters/blob/master/rcp2mid.c)。文書レベルのタイムライン展開でも、ノートイベントにこの移調処理を適用する。

## F-015 — ノート duration と gate の参照実装上の扱い

**状態:** 参照実装を確認し、文書レベルの note-off 展開と合成テストを追加。CI の確定結果は未確認。

- RCP2 の通常ノートレコードは、`code` をノート番号、`p0` を開始までの delay、`p1` を duration、`p2` を velocity として参照実装が読む。
- duration が 0 のノートは参照実装では発音イベントとして出力しない。
- 発音するノートはノートオンを出し、duration に基づいて running-note 管理からノートオフを発行する。同じチャンネル/ノートが既に鳴っている場合は既存ノートの終了時刻を更新する特別扱いがあるため、単純に各ノートへ独立した note-off を足すだけでは完全互換にならない。
- 根拠: [ValleyBell/MidiConverters rcp2mid.c](https://github.com/ValleyBell/MidiConverters/blob/master/rcp2mid.c) の通常ノート処理と `AddRunningNote` / `CheckRunningNotes` 呼び出し。
- 実装では duration=0 を発音イベントとして出さず、duration が正なら開始 tick + duration に note-off を置く。同じトラック内で同じ移調後音高が既に鳴っている場合は新しい note-on を出さず、既存 note-off の tick を現在 tick + 新 duration に更新する。
- note gate はトラック内で展開してから符号付き開始オフセットを適用するため、開始・終了 tick はそれぞれ 0 未満なら 0 にクランプされる。note-off は note-on と同じ移調済み音高を持ち、velocity=0 として表現する。
- 制約: 実際の SMF/MIDI 出力へのシリアライズ、実ファイル再生、極端な重複ノート数の参照互換性は未確認。

## F-016 — EB/EC/EE の標準 MIDI チャンネルイベント展開

**状態:** 参照実装を照合して decoder と文書レベル時系列展開・合成テストを追加。今回のコード変更後の Actions は実行中/待機中で、最終結果は未確認。

- EB は Control Change として p1=controller、p2=value を使い、p0 はイベント後に進める delay として扱う。
- EC は p1 が 0x80 未満の場合に標準 MIDI Program Change として出力データを構成する。0x80 以上は参照実装で特殊な MT-32 インストゥルメント処理に使われるため、標準 Program Change に誤変換せず、現段階では明示的に拒否する。
- EE は Pitch Bend として p1/p2 を 7-bit データバイトとして保持し、p0 はイベント後に進める delay として扱う。
- EB/EE のデータバイトが 7-bit 範囲外の場合は拒否する。文書レベル展開で各イベントは現在 tick に配置し、p0 分だけ次イベントの tick を進める。
- 根拠: [ValleyBell/MidiConverters rcp2mid.c](https://github.com/ValleyBell/MidiConverters/blob/master/rcp2mid.c) の EB/EC/EE case handlers。
- 制約: MIDI status byte のチャンネル付与や実際の MIDI 出力層への接続、EC の特殊 MT-32/SysEx 経路は未実装。

## F-017 — トラック MIDI ポート・チャンネル割り当てと CI #189

**状態:** 実装コミットを GitHub で確認。CI 合格は作業者報告に基づく。実 RCP2 楽曲の再生は未確認。

- 最新コードコミット: [27b583a — Map RCP2 track headers to MIDI ports and channels](https://github.com/pikachu364/S-MU2000/commit/27b583a981b1f2f474c5b5cebc05d197c495748e)。
- `track_midi_mapping` と `map_track_midi_outputs(...)` を追加し、RCP2 track header の MIDI channel byte を分解する。
- ValleyBell/MidiConverters `rcp2mid.c` の参照挙動に合わせ、raw byte の下位 nibble を MIDI channel、上位 nibble を MIDI device/port として扱い、bit 7 が立つ dummy track は disabled とする。
- マッピングを受け取る MIDI event encoder overload を追加し、disabled track のイベントを除外する。既存 overload は維持する。
- `src/verify.cpp` と `tests/verify.txt` に port/channel 分離と dummy track 除外の回帰ケースを追加した。
- [GitHub Actions #189](https://github.com/pikachu364/S-MU2000/actions/runs/39886755808) は作業者が合格と報告。詳細な全ジョブログを独立再取得したわけではないため、根拠の種類を区別して記録する。
- 制約: 実際の SMF serialization、既存 player への RCP2 直接接続、実ファイルでの音出し確認は未実施。

## F-018 — 既存 SMF player の接続境界と RCP2 経路

**状態:** GitHub 上のコード確認済み。合成パイプラインのテストがあり、Actions #225 は全ジョブ成功。実 RCP2 ファイルでの再生は未確認。

- `src/smf.h` / `src/smf.cpp` は SMF format 0/1 の読み込みを行い、`smf::event` を秒単位の時刻・MIDI byte 列・port として表現する。
- `src/ui/player.cpp` の `load_events_from_memory(...)` は、先頭が `MThd` なら SMF 読み込みへ、RCP2 シグネチャなら `rcp::build_intermediate` → `rcp::expand_intermediate_timeline` → `rcp::convert_timeline_to_seconds` → `rcp::map_track_midi_outputs` → `rcp::encode_timeline_to_midi_events` の順で処理し、結果を `smf::event` へ詰め替える。
- RCP2 のタイトル、PPQ、初期テンポ、対応する E7 テンポイベントを `smf::song_meta` に反映する。既存 player の秒単位イベント再生経路を利用するため、RCP2 専用の SMF ファイル書き出しは不要。
- `src/verify.cpp` の `check_rcp2_player_event_pipeline()` は合成データで解析から MIDI イベント化までを検査する。Actions #225 では Linux、Windows MinGW 64/32-bit、MSVC x64/Win32、macOS、README の全7ジョブが成功した。
- **制約:** このテストは内部変換パイプラインの検査であり、`player::add_from_memory` の API レベル動作や実際の RCP2 ファイルの音出しを証明しない。E7 gradual tempo、特殊 EC、ループ、未対応コマンドなどの制約も残る。


## F-016 — FC/F8/F9 展開の回帰テストと CI #262

**状態:** GitHub 上のコード・テスト・期待出力と、Actions #256/#260/#262 の結果を確認。

- 現在のブランチ HEAD は [5306f8b](https://github.com/pikachu364/S-MU2000/commit/5306f8b0d345f46b386a05fc8b1ece38a53cbc14)。
- FC展開コードは `src/rcp.cpp` の `expand_fc_repeats`。FC参照位置の解決、FC連鎖、参照終端からの復帰、循環検出、展開レコード予算を扱う。
- F8/F9展開は `expand_f8_f9_loops`。有限反復数1〜126、ネストループ、反復回数1の境界を合成テストする。0と0x7f以上は仕様未確定のため拒否。
- `src/verify.cpp` と `tests/verify.txt` に、FC展開後にF8/F9ループが続く回帰ケースがある。
- [Actions #256](https://github.com/pikachu364/S-MU2000/actions/runs/40059724255)、[#260](https://github.com/pikachu364/S-MU2000/actions/runs/40061803563)、[#262](https://github.com/pikachu364/S-MU2000/actions/runs/40064492056) は成功。#262 はLinux、Windows、Windows 32-bit、MSVC Win32/x64、README、macOSの全7ジョブ成功。
- #257/#258 はテスト追加時の問題で失敗、#259/#261はキャンセル。履歴を消さず、後続の修正後成功と区別する。
- **制約:** CI成功は対象コードのビルドと合成テストの成功であり、実RCP2ファイルの再生互換性やすべてのループ端点の仕様を保証しない。


## 2026-10-29 — Action #42 と現行コードで確認した事実

- private Action #42 artifact `rcp2-sample-validation-reports` はGitHubから取得でき、ZIP整合性検査に合格。31ファイルを収録。Artifact URL: https://github.com/pikachu364/S-MU2000-private-data/actions/runs/40792718702
- 比較レポートでは parser/reference のイベント総数が sample-01〜04 全て一致。不一致数は sample-01=627、sample-02=16、sample-03=598、sample-04=3。
- sample-04 Track 12 / tick 3648 のイベント順は parser `BC 07 61 → BC 20 00 → 9C 36 00`、reference `9C 36 00 → BC 07 61 → BC 20 00`。MIDI bytes の意味は CC7、CC32、チャンネル12の Note On velocity 0（Note Off 相当）。
- 現行 public branch `feature/rcp2-parser-skeleton` の `src/rcp.cpp`（取得時 blob SHA `6b04fa6a82c694fc39c9c1441921db57de9a1533`）では `expand_note_gates()` が tick を比較キーに stable sort する。追加の種別優先順位は同tickに CC、Note Off、新規 Note が全て存在する場合だけ適用される。
- 現行 `encode_midi_message()` の Note Off 出力は `0x9n, note, 0` 表現。古い記録の「0x8n出力」という説明は現行コードの説明として使わない。
- 確認方法: GitHub private workflow artifactの取得・ZIP検査、各比較レポートの読解、GitHub connectorによる現行ソース確認。
- 制約: sample-04 の問題イベントを元RCPの4-byte record index / byte offsetに対応付けるところまでは未実施。原因確定、修正後CI、実再生の根拠ではない。


## F-020 — S-MU2000 handling of unknown high-bit bytes in 0x98 SysEx templates

**Status:** Confirmed by source review; no runtime test added for this finding.

- In src/rcp.cpp at the 0x98 SysEx expansion loop (current blob SHA at investigation time: 6b04fa6a82c694fc39c9c1441921db57de9a1533), bytes 0x80–0x84 and 0xF7 are handled specially. Other bytes with the high bit set are skipped; the source comment explicitly cites 0xFF and says this matches the reference RCP2 converter behavior.
- Reference converter ProcessRcpSysEx() at ValleyBell/MidiConverters commit a73c748960d57bd194fb64931ce07d2c1fa1bd01 likewise warns for unknown high-bit template bytes and does not append them to output.
- Therefore, source inspection suggests the warning does not by itself indicate a behavior difference between the two converters for these bytes.
- Verification method: source-code review only, plus Action #42 raw dump/log inspection. No playback or new automated test was performed.
- Limitation: the intended meaning/origin of 0xFF in this file and any relation to event-order mismatches remain unverified.


## F-021 — RCP2 Track Exclusive のテンプレート規則と 0xFF

**状態:** 形式資料と複数実装のソース照合で確認。今回の 0xFF の元データ作成意図、および実機での影響は未確定。

- ValleyBell/MidiConverters の `RCPFormat.txt` は、RCP2 コマンド `0x98` の SysEx データが後続の `0xF7` 継続イベントに格納されること、`p1/p2` がテンプレート内で使われるデータ値であることを説明している。
- 同資料が列挙するテンプレート制御値は `0x80`（p1）、`0x81`（p2）、`0x82`（MIDI channel）、`0x83`（Roland checksum 初期化）、`0x84`（checksum 出力）であり、`0xFF` の置換規則は記載されていない。
- ValleyBell の `ProcessRcpSysEx()` は未知の高ビット付きテンプレート値を警告してスキップする。S-MU2000 の `src/rcp.cpp` の `0x98` 展開も、`0x80`–`0x84` と `0xF7` 以外の高ビット値をスキップする。
- Gearmulator の `rcpTrack.cpp` は `0x98` 本体の `p1/p2` を SysEx データの先頭として扱わず、後続 `0xF7` 継続レコードからペイロードを組み立てる。その後のテンプレート展開でも未知の高ビット値をスキップする。参照: https://github.com/dsp56300/gearmulator/blob/main/source/framework/synthLib/midi/rcpTrack.cpp
- 以上から、現時点で最も根拠のある扱いは「`0xFF` は確認できた RCP2 テンプレート制御値ではなく、既存の参照規則では未知の高ビット値としてスキップされる」。これは、ファイル作者が `0xFF` を何の目的で記録したか、またはスキップが楽曲の意図に合うかを証明しない。
- 今回の3件について、`0xFF` 除去後に既存チェックサムが不一致となる計算結果はあるが、置換先を特定する証拠にはならない。

**根拠:** [ValleyBell RCPFormat.txt](https://github.com/ValleyBell/MidiConverters/blob/master/RCPFormat.txt)、[ValleyBell rcp2mid.c](https://github.com/ValleyBell/MidiConverters/blob/master/rcp2mid.c)、[Gearmulator rcpTrack.cpp](https://github.com/dsp56300/gearmulator/blob/main/source/framework/synthLib/midi/rcpTrack.cpp)、[S-MU2000 src/rcp.cpp](https://github.com/pikachu364/S-MU2000/blob/feature/rcp2-parser-skeleton/src/rcp.cpp)。

**制約:** ソースコード・形式資料の照合であり、実機再生・今回のSysExの意味的正しさ・置換値は未確認。公開文書には private sample の生バイト列を記載しない。


## F-022 — SysEx テンプレートの異常と RCP 読み込み継続は別判定

**状態:** Gearmulator 現行ソースの処理経路を確認。88emu の画面に表示される個別の「SysCheck error」との対応関係は未確認。

- Gearmulator の `rcpDecoder.cpp` は、ヘッダー切れ、G36トラックの不正・切り詰め、イベント安全上限、利用可能トラックなし等を RCP 文書読み込み失敗として扱う。
- 一方、`rcpTrack.cpp` の `expandSysExTemplate()` は、既知のテンプレート値以外の上位ビット付きバイトを `continue` でスキップする。これ自体ではエラー文字列を設定せず、テンプレート展開ループを継続する。
- `0x98` は後続の `0xF7` 継続レコードから payload を組み立て、テンプレート展開後にその継続レコード群を読み飛ばす。未知値を含む SysEx だけが省略されても、後続の RCP イベント処理は続く構造。
- 展開結果が空の場合も `addSysExTemplate()` は成功扱いで戻る。従って未知高ビット値の存在だけで、RCP 全体の読み込み失敗にはならない。
- `readRcp()` は構造解析失敗や track expansion の明示的失敗では全体を失敗させるが、上記の未知テンプレート値スキップはその失敗条件に含まれていない。
- 現行の Gearmulator unit test は `0x98` の F7 継続データを使う正常系を確認している。ただし、未知高ビット値を含むケースを直接テストする回帰ケースは今回確認できていない。

**重要な区別:** これは「RCP parser が未知テンプレート値を無視して後続イベントを処理できる」ことのソース確認であり、88emu UI の SysCheck error が同じ検査を指すことや、音源ファームウェア側の SysEx checksum error が非致命であることを証明しない。

**根拠:** [Gearmulator rcpDecoder.cpp](https://github.com/dsp56300/gearmulator/blob/main/source/framework/synthLib/midi/rcpDecoder.cpp)、[Gearmulator rcpTrack.cpp](https://github.com/dsp56300/gearmulator/blob/main/source/framework/synthLib/midi/rcpTrack.cpp)、[Gearmulator midiFile_test.cpp](https://github.com/dsp56300/gearmulator/blob/main/source/framework/juce/jucePlayerLib/tests/midiFile_test.cpp)。


## F-023 — 88emu の Roland DT1 checksum error は個別 SysEx の拒否通知

**状態:** Gearmulator の SC-88 sub-MCU 実装で処理を確認。ユーザーの「syscheck error」が画面上の "Check Sum Error" を指す場合に対応する有力な説明。ただし実際の表示文言は未照合。

- `Sc88SubMcu::midiIn()` は SysEx の終端 `F7` を受けると `endOfSysEx()` を呼び、組み立てた SysEx をクリアしてそのメッセージ処理を終了する。
- Roland DT1 の `decodedTransfer()` は、アドレス・データ・checksum の合計下位7bitが0でない場合、`OpSysExError`（`0xE0`）通知を生成し、`param1=3`（"Check Sum Error"）を設定して、その DT1 の転送処理だけを `return` する。
- コメントには、音源側は checksum 不一致のメッセージを無視し、sub-MCU がパネル通知を出す、と明記されている。
- `midiIn()` の SysEx 終端処理は当該メッセージの assembly をクリアして戻る。次に到着する MIDI byte は通常の入力処理へ進むため、この経路は RCP ファイル全体や後続の MIDI 演奏を中止する処理ではない。
- よって、Roland DT1 checksum error が出ても、問題の DT1 設定だけが反映されず、その後のノート等は引き続き処理される、という挙動が実装上成立する。

**重要な区別:** これは RCP parser のエラーではなく、完成した Roland SysEx を音源側 sub-MCU が検査した段階の通知。ユーザーの言う "SysCheck error" が別の表示・別機種のエラーなら、同一視しない。

**根拠:** [Gearmulator sc88_submcu.cpp](https://github.com/dsp56300/gearmulator/blob/main/source/ronaldo/88emu/88lib/mcu/sc88_submcu.cpp)、[sc88_submcu.h](https://github.com/dsp56300/gearmulator/blob/main/source/ronaldo/88emu/88lib/mcu/sc88_submcu.h)。



## F-025 — Gearmulator の SC-88「Check Sum Error」と短い DT1 の扱い

**状態:** Gearmulator の現行ソース確認済み。今回の private sample の3件との対応は未確認。

- `Sc88SubMcu::decodedTransfer()` は、DT1 のアドレス先頭位置を 5 とし、SysEx body が9 bytes未満の場合、チェックサム計算より前に何も通知せず return する。
- チェックサム不一致の場合は `OpSysExError (0xE0)`、`param1=3` を通知する。この通知がファームウェア上の `Check Sum Error` 表示に対応する。
- よって、private sample の対象3件（F0/F7を除いた出力bodyが各8 bytes）は、このコード経路では checksum mismatch の通知を発生させない。短すぎるため検査前に return する。
- チェックサム不一致時も拒否対象は当該 DT1 転送であり、RCP ファイル全体の解析失敗を意味しない。ユーザーはエラー表示後も演奏を聴けた経験があると報告。
- 根拠: [Gearmulator sc88_submcu.cpp](https://github.com/dsp56300/gearmulator/blob/main/source/ronaldo/88emu/88lib/mcu/sc88_submcu.cpp)、[sc88.h](https://github.com/dsp56300/gearmulator/blob/main/source/ronaldo/88emu/88lib/boards/sc88.h)。
- **制約:** 88emuでユーザーが過去に見た表示の個別発生源、今回の3件以外の SysEx の checksum、実機相当の再生挙動は未確認。



## F-026 — L_WING88 の Roland DT1 checksum 棚卸し

**対象:** private sample validation artifact の sample-04 event dump（2026-10-29生成）。生RCPの内容・位置・raw bytesは公開しない。

- event dump 内の Roland DT1（Roland ID 41 / DT1 command 12）を120件確認。
- SysEx body長（F0/F7除外）は、9 bytes: 114件、12 bytes: 1件、264 bytes: 1件、92 bytes: 1件、8 bytes: 3件。
- Gearmulator の SC-88 sub-MCU が checksum 検査する最低長9 bytes以上の117件は、アドレス先頭からchecksumまでの7-bit合計がすべて0。checksum不一致は0件。
- 残る3件はbody長8 bytesでchecksum byteを含まず、Gearmulatorは最低長条件でchecksum検査前に無通知returnする。
- したがって、このsample-04の変換済みDT1イベント120件の中に、Gearmulatorの `Check Sum Error` 通知条件に該当するDT1は見つからなかった。
- sample-04の既存比較結果は parser/reference とも22,536 events、exact_match=true、mismatch_count=0。
- **制約:** これは保存済みsample-04 event dumpの静的照合であり、88emuの実再生試験ではない。ユーザーが過去に見たCheck Sum Errorの発生源を特定したものではない。
