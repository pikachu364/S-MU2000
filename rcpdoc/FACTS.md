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
