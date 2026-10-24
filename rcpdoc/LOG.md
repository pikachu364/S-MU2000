# 作業ログ

日付順の短い記録です。テストを実行したかどうか、何を根拠に判断したかを必ず区別します。

## 2026-10-15 — RCP2 作業状況の整理

- RCP2 を最初の対象とし、将来の RCP3 を見据えて解析と MIDI イベント展開を分離する方針を記録した。
- 過去のローカル実行ではビルド、基本パーサー、FE 終端テストが OK と記録されていた。
- FE 終端テストのデータ構成に問題がある可能性を見つけたため、合格確定とはせず、修正後の再実行を次の作業とした。
- ローカルに未コミット変更・未追跡ファイルがあると記録されていた。保護してから差分を確認する必要がある。
- 詳細: [2026-10-15 レポート](reports/rcp2-local-progress-2026-10-15.md)

## 2026-10-16 — iPad mini 7 から作業継続

- パソコンを利用できなかったため、ビルド・ローカルテストは実施しなかった。
- iPad mini 7 から GitHub 上のドキュメントを保存できることを確認した。
- src/verify.cpp に FE 後の不完全な断片を扱うテストを追加したコミットを参照し、実行検証は未完了と明記した。
- 詳細: [2026-10-16 作業レポート](reports/rcp2-work-log-2026-10-16.md)

## 2026-10-20 — 引き継ぎ用ドキュメント構造を作成

- Glosa の docs/reverse/ を参考に、事実・仮説・次の作業・日付順ログを分ける構成を採用した。
- rcpdoc/ を新設し、過去レポートを保存したまま参照できる形で収録する。
- 今回はドキュメントの整理のみ。ビルド、テスト、MIDI 再生は実施していない。

## 2026-10-20 — JMPlayer RCP 資料を調査・記録

- JMPlayer の [RCP format notes](https://github.com/jihodaya/jmplayer/blob/main/docs/formats/en/rcp.md) と [rcpfilehandler.cpp](https://github.com/jihodaya/jmplayer/blob/main/rcpfilehandler.cpp) を閲覧した。
- 資料が報告する 44-byte track header、transpose / delay / mute、時間を進めないイベント、0xFC repeat、未解決の 0x98、GSD→SysEx 処理を、S-MU2000 での確認事項として整理した。
- 外部資料の記述は JMPlayer 側の検証報告であり、S-MU2000 の実装・テストで確認済みとは扱わない。
- 今回はドキュメント変更のみ。S-MU2000 のコード変更、ビルド、テスト、MIDI 再生は実施していない。
- 詳細: [JMPlayer RCP 調査ノート](references/jmplayer-rcp-notes.md)

## 2026-10-20 — 実装チェックリストを作成

- JMPlayer資料のヘッダー、イベント、時間制御、ループ、未解決項目を、S-MU2000側のコード確認・テスト追跡用 CHECKLIST.md に整理した。
- JMPlayer資料のチェックマークと、S-MU2000での検証状態を明確に分離した。
- NEXT_STEPS.md と README.md からチェックリストへリンクした。
- 今回はドキュメント変更のみ。ソースコード変更、ビルド、テスト、MIDI再生は実施していない。

## 2026-10-20 — FE 終端テストの入力を強化

- GitHub の feature/rcp2-parser-skeleton で src/verify.cpp の既存トラックテストを確認した。
- FE の後ろに置くデータを、無効な 0x90 レコード風の並びから、ノート番号 61・step 0・gate 48・velocity 100 の有効な4バイトレコードに変更した。テストは、FE までの2イベントだけが返ることを引き続き検査する。
- コミット: [53d8536 — test: verify RCP2 track terminator in loader](https://github.com/pikachu364/S-MU2000/commit/53d8536496e844a9cf38c4619ec3c3959de140b5)
- コード変更は GitHub 上で再取得して確認した。ビルド・テスト実行は未実施であり、合格とは記録しない。

## 2026-10-20 — ローカル make check 成功

- 作業者が R:\\S-MU2000-rcp で `make check` を実行し、その端末出力をこの会話で共有した。
- 出力では DLL と verify.exe のビルド後、`RCP2 basic playback parser: OK`、`RCP2 FE terminator: OK`、`VSTi probe passed: S-MU2000, stereo, MIDI, state, editor` が確認できた。
- FE 終端テストでは、FE の後ろにノート番号 61・step 0・gate 48・velocity 100 の完全なレコードを置き、解析イベント数が2のままであることを検査する。
- これは作業者が共有したローカル実行ログに基づく結果。GitHub の feature/rcp2-parser-skeleton 上のコードを同じ環境でビルド・実行したことや、実際の RCP ファイルでの再生は確認していない。
- ローカル作業ツリーには未コミット変更・未追跡ファイルが残っているため、コミット、リセット、ブランチ統合は別途差分確認後に行う。

## 2026-10-20 — mute flag テストの追加と Actions 再有効化

- トラックヘッダーの mute flag（+7 bit 0）を扱うテストを追加・修正した。
- テスト結果への反映、出力の改行、回帰時に検証プログラムが失敗することを確認できる構成にした。
- 関連コミット:
  - [f8774a0 — test: verify RCP2 mute flag preservation](https://github.com/pikachu364/S-MU2000/commit/f8774a0e86597ca112fecc2eb9f35843a26406bf)
  - [fc1505b — test: fix mute flag test output newlines](https://github.com/pikachu364/S-MU2000/commit/fc1505b5355aaf51dbe2230743e1234431bb99e3)
  - [bc6c98c — test: include mute flag cases in header result](https://github.com/pikachu364/S-MU2000/commit/bc6c98ce2fefc1cd34a633d58ab0df0f9e79d96a)
  - [06dac4f — test: fail verification on mute flag regression](https://github.com/pikachu364/S-MU2000/commit/06dac4ff7b1dfca9cda1aaa61ba67f97b281ed9c)
- GitHub Actions の fork 無効化警告が消えたことを確認し、新しい push による `build` の自動起動を確認した。
- [build #78](https://github.com/pikachu364/S-MU2000/actions/runs/39424636776) は確認時点で実行中。全ジョブの成功・失敗はまだ確定していない。
- テストコードの追加と、CI 上でのテスト成功は区別する。実際の RCP ファイルでの互換性や再生確認も未完了。


## 2026-10-21 — RCP2 時系列展開と署名付きトラック開始オフセット

- RCP2 中間イベント列をトラックごとの時刻付きイベントへ展開し、複数トラックを tick 順に統合する処理を追加した。
- 同じ tick のイベントはトラック index 順で安定化し、各トラック内のイベント順を保持する。入力がトラック内で時刻順でない場合は失敗し、出力を空にする。
- 文書レベルの展開は、現時点で対応しているノートおよび F5/E7 系のサブセットを使い、未対応コマンドを拒否する。段階的なテンポ変化は未対応。
- トラック開始オフセットを符号付き 8-bit tick として適用する。負の tick は 0 にクランプし、32-bit 範囲を超える tick はエラーにする。
- オフセット回帰テストの CI で、期待出力と実際の出力が一致しない失敗を確認した。原因は、テスト fixture がノートの step を「そのノートの開始位置」として誤って配置していたこと。最初のノートの step を 5 に変更し、次のノートがローカル tick 5 になるよう修正した。
- 関連コミット:
  - [stable multi-track timeline merge declaration](https://github.com/pikachu364/S-MU2000/commit/74c9c224820b6e0f8c1988b6d995eafb858470b8)
  - [signed track offset implementation](https://github.com/pikachu364/S-MU2000/commit/623dc8098cd424d3ad8b05708014a7958633f46f)
  - [offset regression test fixture correction](https://github.com/pikachu364/S-MU2000/commit/83d213c42eace8e882446e80d78aeee4c8ab41e0)
- CI の修正後の結果は、この記録更新時点では未確認。最新コミットの Actions を再確認し、結果が確定したら追記する。
- この更新では RCP 楽曲ファイルの実再生や、パソコン上でのローカルビルド・テストを実施していない。


## 2026-10-21 — 署名付きオフセット fixture 修正後の CI 合格

- 作業者が共有した GitHub Actions 一覧で、[build #101](https://github.com/pikachu364/S-MU2000/actions/runs/39571008073) が commit [83d213c](https://github.com/pikachu364/S-MU2000/commit/83d213c42eace8e882446e80d78aeee4c8ab41e0) に対して **completed successfully** と表示されることを確認した。
- これにより、fixture 修正後の CI 成功を確認済みとして記録する。以前の失敗 #99/#100 は履歴として残し、成功結果で上書きしない。
- 後続のドキュメント更新コミットも build #102〜#106 で成功、macOS runner 設定更新 #107〜#111 も共有一覧では成功している。
- この確認はユーザー共有の Actions 一覧に基づく。実際の RCP ファイルの再生、全コマンドの互換性、ローカル未コミット作業との統合は別途未確認。
- 次は tick 0 クランプと 32-bit 上限境界の合成テストを追加し、動作を明示的に検証する。


## 2026-10-21 — 符号付きトラック開始オフセットの端点テスト

- CI 成功を確認した後、signed 8-bit オフセットの両端を独立に検証する合成テストを追加した。
- `0x80`（-128）は tick 0 にクランプされ、`0x7f`（+127）はローカル tick 0 のイベントを tick 127 に移動することを検査する。
- テスト変更: [9d0d183](https://github.com/pikachu364/S-MU2000/commit/9d0d183dacf14f6cf95cf31574dc0eed8dc122de)。期待出力更新: [762ee46](https://github.com/pikachu364/S-MU2000/commit/762ee46ed00adb1d548d150319b89d287dea2bc7)。
- これはコードと期待出力を GitHub に追加した記録であり、テスト実行結果ではない。追加後の Actions は未確認。


## 2026-10-21 — オフセット端点テストの CI 失敗と期待出力修正

- [build #116](https://github.com/pikachu364/S-MU2000/actions/runs/39589682748) は、テスト変更コミット [9d0d183](https://github.com/pikachu364/S-MU2000/commit/9d0d183dacf14f6cf95cf31574dc0eed8dc122de) で失敗した。
- 共有ログでは、コードのコンパイルではなく `verify` の期待出力不一致が原因。実際の出力に「オフセット境界 合」が追加された一方、baseline は旧形式のままだった。
- 期待出力を [762ee46](https://github.com/pikachu364/S-MU2000/commit/762ee46ed00adb1d548d150319b89d287dea2bc7) で更新。後続の build #117 と、ドキュメント更新後の #118〜#120 は、ユーザー共有の Actions 一覧で completed successfully と確認できた。
- したがって #116 は中間コミットでの baseline 不一致、#117 以降で修正後の CI 成功を確認済み。Node.js 20、MinGW、Ubuntu runner の警告はこのテスト不一致とは別件。
- 実際の RCP2 楽曲での再生確認や、ローカル作業ツリーとの差分確認は未実施。


## 2026-10-21 — オフセット適用対象のイベント種別を追加検証

- 文書レベルの時間展開テストに、符号付きトラック開始オフセットがノートだけでなく F5 キー署名・E7 テンポイベントにも適用されることを確認するケースを追加した。
- テスト変更: [2ed0c96](https://github.com/pikachu364/S-MU2000/commit/2ed0c9657b0fc69dda43794233d3539bea0a04d9)。期待出力更新: [d13e84c](https://github.com/pikachu364/S-MU2000/commit/d13e84c08a0003c3cee30d09fc8a7815e0905833)。
- 追加後の Actions は、ユーザー共有の一覧で #124〜#128 が失敗と確認された。原因は `printf` の改行エスケープであり、イベント種別の各 assertion 自体ではなく、次の行と出力が連結されて baseline 比較に失敗していた。
- 調査メモ: RCP2 の宣言トラック長は 16-bit なので、通常のパース済みトラックでは delay だけで 32-bit tick 上限に到達することはできない。上限超過分岐は防御的な API 入力チェックとして残し、数千万レコードを生成する高コストなテストは避ける。


## 2026-10-21 — verifier 出力の改行エスケープを修正

- GitHub Actions #124〜#128 は全て `make test` の期待出力比較で失敗。ログを確認すると、新しい `printf` 行が改行ではなく文字列 `\\n` を出力しており、次の検証行が同じ行に連結されていた。
- `src/verify.cpp` のエスケープを通常の C++ 改行エスケープ `\\n` に修正した。修正コミット: [a9d5a61](https://github.com/pikachu364/S-MU2000/commit/a9d5a61e17223a0428257441f82054122ad30e6b)。
- baseline は各検証を別行に出力する従来形式のまま維持し、不要な baseline 更新は行わなかった。
- ユーザー共有の Actions 一覧で、修正コミットの build #129 と記録更新コミットの build #130 がどちらも completed successfully と確認できた。イベント種別オフセットテストを含む変更後の CI 成功を確認済み。


## 2026-10-22 — 参照実装に基づくノート移調ヘルパー

- ValleyBell/MidiConverters の `rcp2mid.c` を照合し、トラック移調バイトの bit 7 が立つ場合は移調を抑止し、通常トラックでは signed 7-bit トラック値と signed 8-bit 文書値を加算してノート番号を 7 bit に折り返す処理を独立ヘルパー化した。
- 合成テストは負の各移調、リズムチャンネル、折返し、範囲外ノートを対象とする。タイムラインへの適用はまだ行っておらず、今回の変更の CI も未確認。
- 参照: https://github.com/ValleyBell/MidiConverters/blob/master/rcp2mid.c

- 追加段階: ノート移調を `expand_intermediate_timeline` に適用し、トラック移調 + 文書移調、およびリズムチャンネルで移調を抑止する統合テストを追加。CI は未確認。


## 2026-10-22 — ノート gate の参照挙動を調査

- `src/rcp.h` の `play_bias` コメントを、参照実装との照合後の signed 8-bit 文書移調という説明に修正した（[9444ef9](https://github.com/pikachu364/S-MU2000/commit/9444ef913815bcc87b16637892c7b8080019ce88)）。
- ValleyBell/MidiConverters の `rcp2mid.c` を再確認し、RCP2 ノートの `code=音高 / p0=delay / p1=duration / p2=velocity`、duration 0 は発音しないこと、同じチャンネル/音高の再発音時は既存の終了時刻を更新することを確認して FACTS.md F-015 に記録した（[e55a842](https://github.com/pikachu364/S-MU2000/commit/e55a842d8648d57569a4177d49312030152c7710)）。
- この発見により、現在の `note_event.duration` 保持だけでは note-off/gate 展開を実装したことにならない点を明確にした。単純な note-off 追加は同音高の再発音で参照実装と異なる可能性があるため、テスト設計を先に行う。
- NEXT_STEPS.md で note gate と直接写像可能な MIDI コマンドの decoder を次候補として優先付けした（[3d0eae8](https://github.com/pikachu364/S-MU2000/commit/3d0eae8541eedbc207b47e3a2864ea7e55a07cb9)）。
- `e55a842` の Actions build #162 は記録時点で queued。CI 結果はまだ成功扱いにしない。
- 参照コード: https://github.com/ValleyBell/MidiConverters/blob/master/rcp2mid.c

## 2026-10-22 — EB/EC/EE MIDI チャンネルコマンドの小規模展開

- 参照実装の EB/EC/EE handler を照合し、EB Control Change、EC の標準 Program Change（p1 < 0x80）、EE Pitch Bend のデータ形式を追加した。いずれもイベントは現在 tick に出力し、p0 の delay で次イベント位置を進める。
- EC p1 >= 0x80 は MT-32 向け特殊インストゥルメント処理に使われるため、標準 Program Change として誤解釈せず、現段階ではエラーにする。EB/EE の data byte が 7-bit 範囲外の場合も拒否する。
- 実装: [6c9045e](https://github.com/pikachu364/S-MU2000/commit/6c9045ec61af8226d77dffc7abb094e5fba4f660)（イベント表現）、[c4fd8de](https://github.com/pikachu364/S-MU2000/commit/c4fd8de43040cbff2d6bd74b7eb08c7420384a72)（decoder/展開）、[693ed61](https://github.com/pikachu364/S-MU2000/commit/693ed61cbdb6eb15ab869f03362983b4f9b9196f)（テスト）、[4502c5d](https://github.com/pikachu364/S-MU2000/commit/4502c5d31307ef9178b56806e1bc87d0fc78ed65)（verifier 状態と改行修正）、[a8230a5](https://github.com/pikachu364/S-MU2000/commit/a8230a51cc05d16cfbd45e898cc3b0b8c3cb078d)（baseline）。
- 合成テストでは各イベントの tick と値、特殊 EC の拒否、7-bit 範囲外の拒否を検査する。テスト実行結果は GitHub Actions の確定後に追記する。
- 制約: MIDI 出力層への接続、MIDI チャンネルの status byte 化、特殊 MT-32/SysEx 経路は未実装。次は note duration と note-off の running-note 挙動をテスト化する。

## 2026-10-22 — ノート gate / note-off 展開

- GitHub Actions #174 で EB/EC/EE verifier baseline の順序修正後の CI 成功を確認してから実装を開始した。
- 文書レベル時系列で、duration=0 のノートを発音イベントとして出さず、正の duration に対して note-off を開始 tick + duration に追加する処理を実装した。
- ValleyBell/MidiConverters の running-note 挙動に合わせ、同一トラック内で同じ移調後音高がまだ鳴っている場合は新規 note-on を追加せず、既存 note-off tick を現在 tick + duration に更新する。duration=0 の同音高再指定は既存 note-off を現在 tick に移動する。
- note-off は開始イベントの移調後音高を引き継ぐ。gate 展開を符号付きトラック開始オフセットの前に行い、note-on/note-off の tick に同じ offset/clamp 規則を適用する。
- src/verify.cpp と tests/verify.txt に、duration=0、同音高の終了更新、note-off tick/音高、複数トラック統合、オフセットとの相互作用を追加・更新した。
- 今回のコミットと GitHub Actions の最終結果は、このログ更新時点では未確認。実 RCP2 ファイルの再生、SMF 出力への接続は未実施。


## 2026-10-23 — MIDI ポート/チャンネル割り当てと CI #189

- [27b583a — Map RCP2 track headers to MIDI ports and channels](https://github.com/pikachu364/S-MU2000/commit/27b583a981b1f2f474c5b5cebc05d197c495748e) を feature ブランチの最新コードコミットとして確認した。
- RCP2 track header の MIDI channel byte を下位 nibble（channel）と上位 nibble（port/device）に分け、bit 7 が立つ dummy track を無効化する `map_track_midi_outputs(...)` を追加した。対応する MIDI event encoder overload は disabled track のイベントを除外する。
- `src/verify.cpp` と `tests/verify.txt` に port/channel 分離と dummy track 除外の regression case が含まれる。
- 作業者から GitHub Actions #189 合格の報告を受けた：[Actions #189](https://github.com/pikachu364/S-MU2000/actions/runs/39886755808)。本ログでは作業者報告に基づく記録とし、実 RCP2 楽曲の再生確認とは区別する。
- 既存 SMF player のコードを確認した。`smf::event` は秒・MIDI byte 列・port を持ち、`src/ui/player.cpp` は `smf::load` / `smf::load_from_memory` を使用する。RCP2 event の直接接続、SMF serialization、実ファイル再生は未実装/未確認。
- 次は既存 player の時間・port・seek/chase 契約を調べ、RCP2 event adapter と SMF serialization のどちらが安全か決める。CI 合格を実再生互換性と混同しない。


## 2026-10-24 — FC/F8/F9 境界テストと CI 成功

- FC の参照展開で、FC連鎖・参照終端からの復帰・ヘッダー内/未整列/範囲外・FE参照・循環の合成テストが成功。 [Actions #256](https://github.com/pikachu364/S-MU2000/actions/runs/40059724255)。
- FC展開後にF8/F9のカウント付きループが続くケースを追加。テストコードの lambda capture と出力改行、`tests/verify.txt` の期待値を修正した。最終的に [Actions #260](https://github.com/pikachu364/S-MU2000/actions/runs/40061803563) が成功。
- F8の反復回数1の境界テストを追加し、期待出力を更新。[Actions #262](https://github.com/pikachu364/S-MU2000/actions/runs/40064492056) は全7ジョブ成功。
- 失敗した #257/#258 とキャンセルされた #259/#261 は履歴として残す。修正後の成功を確認済み。
- 現 HEAD: [5306f8b](https://github.com/pikachu364/S-MU2000/commit/5306f8b0d345f46b386a05fc8b1ece38a53cbc14)。
- 制約: CIは合成テストとビルドの結果。実RCP2ファイルの再生互換性は未確認。


## 2026-10-24 — Windows verify 修正、player API と展開予算テスト

- [52d7a82](https://github.com/pikachu364/S-MU2000/commit/52d7a8268b2f83022272e8f4449fb7af7446b18d) で Windows の verify リンクに `-lwinmm` を追加。Windows の `timeBeginPeriod` / `timeEndPeriod` 未解決シンボルを修正。
- [b957456](https://github.com/pikachu364/S-MU2000/commit/b957456898fc628b68893ef8badca90a7ac128eb) で `src/ui/player.cpp` の `L'\\\\0'` を正しい wide null 文字 `L'\\0'` に修正。
- ユーザー報告により [Actions #274](https://github.com/pikachu364/S-MU2000/actions/runs/40069003704) 合格を確認。
- [4a13956](https://github.com/pikachu364/S-MU2000/commit/4a1395680da9213f502960f450fe63c5f104a091) で F8/F9 の展開予算テストを追加。3重ネストの126回ループで100万レコード上限を超えさせ、処理失敗・timeline出力クリア・エラー設定を検査する。
- ユーザー報告により [Actions #275](https://github.com/pikachu364/S-MU2000/actions/runs/40072000688) 合格を確認。
- `rcpdoc/README.md`、`NEXT_STEPS.md`、`CHECKLIST.md` を更新し、API テストと展開予算テストの結果、制約、次の作業を記録。
- 制約: API/ループのテストは合成データに対する CI 検証。実 RCP2 ファイルの再生互換性は未確認。次は FC 展開予算の境界と参照チェーンを検証する。


## 2026-10-24 — ドキュメント整合性確認と CI #279

- ユーザー報告を受け、[Actions #279](https://github.com/pikachu364/S-MU2000/actions/runs/40073550348) の状態をGitHub APIで確認。全7ジョブが `success`。
- `rcpdoc/README.md` に残っていた古い HEAD と「player接続未完了」の記述を現行コード・#274のAPI合成テスト結果に合わせて修正。
- `rcpdoc/NEXT_STEPS.md` の HEAD、APIテスト状態、FC固有の予算テスト課題を更新。
- リポジトリのファイルツリーを確認した範囲では、テスト用の `.rcp` / `.mid` ファイルは見当たらない。実ファイル互換性は引き続き未確認で、次の比較には許可されたサンプルまたは独立参照出力が必要。
- #279 は文書変更後のCI成功であり、実曲再生テストの成功ではない。

## 2026-10-24 — 実 RCP2 サンプル調査と旧形式トラック数の修正

- ユーザーがプライベート資料リポジトリ [S-MU2000-private-data/RCP_DATA](https://github.com/pikachu364/S-MU2000-private-data/tree/main/RCP_DATA) に用意した12曲分の `.RCP` と説明書を調査対象として確認した。楽曲データは公開リポジトリへコピーしない。
- 全12ファイルの先頭に `RCM-PC98V2.0(C)COME ON MUSIC` 識別文字列があり、トラック領域を長さフィールドでたどると18個のトラックスロットが並ぶことをバイナリ構造から確認した。
- 多くのサンプルではヘッダーの `0x1E6` トラック数 byte が0。ValleyBell/MidiConverters の `rcp2mid.c` は0の場合に18トラックとして扱うため、`parse_header` に同じ互換フォールバックを追加した。
- 合成テストを拡張し、ヘッダーのトラック数0から18を返すこと、および18個の短いトラック（各トラックはFE終端）を `parse_tracks` が解析することを検査する。
- コミット: [b40c0df](https://github.com/pikachu364/S-MU2000/commit/b40c0dfd90e298352fe186a4d6ec4ed8f77e5687)、[e8a286b](https://github.com/pikachu364/S-MU2000/commit/e8a286b32ab8c6d620245f65066a912979020f1a)、[b00d17c](https://github.com/pikachu364/S-MU2000/commit/b00d17cdb834298ded55b226c228ecf54703dd09)、[9cccd64](https://github.com/pikachu364/S-MU2000/commit/9cccd6412d6932c5709c41e47197b35fe7afe934)。
- この時点で新しいテストの GitHub Actions は実行中/待機中。成功は未確認。プライベート実データを使った `parse_tracks`・イベント展開・再生テストも未実施であり、今後の検証課題として残す。

## 2026-10-24 — 実サンプルのループ構造確認とコメント展開バグ修正

- プライベートの12曲について、バイナリの18トラック境界・各トラックのFE終端を独立スクリプトで照合し、すべてのトラック領域を最後まで走査できることを確認した。これはC++パーサーの実行結果ではない。
- FC参照先の形式を現行実装の規則で検査したところ、12ファイルの全トラックがFC参照展開を通過した。一方、F8/F9展開では MIND track 11 と WINGS_55 track 5 に F9/F8 対応不一致が現れた。参照仕様と実ファイルを照合するまで、この2トラックの互換性は未解決として扱う。
- 実装レビューで、F6コメント処理がループ展開後の `expanded_sequence` ではなく元の `sequence` を参照している問題を発見。ループ展開でインデックスが変わるとF7継続レコードを誤拒否し得るため、`src/rcp.cpp` を修正した。
- `src/verify.cpp` に、F8/F9ループ展開後にF6/F7コメントがある合成回帰テストを追加。コード: [92b6d49](https://github.com/pikachu364/S-MU2000/commit/92b6d49cc013c224e19c49fca1b6d0e995da0dd5)、[9209688](https://github.com/pikachu364/S-MU2000/commit/92096888b24911b40b76fbe5affe696d1ccca945)、[5e35b4e](https://github.com/pikachu364/S-MU2000/commit/5e35b4e70bfc385db1c87cbdce91c791e7cc6575)。
- CI #293 は確認時点で pending。修正後のCI成功と、実ファイルをC++実装に通した結果はまだ未確認。

## 2026-10-24 — FD小節終端の時間処理

- JMPlayer RCP format notes のコマンド表では `FD` は小節終端で、step byte は時間として加算しないと説明されている。この説明を参照仕様候補として採用し、`expand_note_key_signature_tempo_timeline` で `FD` をイベント出力・tick加算なしでスキップするようにした。
- プライベート12曲のコード頻度調査では `FD` が計18,324件出現した。`DD/DE/DF` も合計1,028件あり、`E1/E2/E6` と `0x80–0x98` と合わせて次の未対応コマンド調査候補にした。
- `src/verify.cpp` に、FDのstep byteが大きくても次ノートのtickを進めない合成テストを追加。コミット: [270f7d0](https://github.com/pikachu364/S-MU2000/commit/270f7d097f54174306316abc65cdf3524f068de4)、[c4041c0](https://github.com/pikachu364/S-MU2000/commit/c4041c03eb485871a1e1932ef0fa0a0ab1565acf)。
- 参照資料: [JMPlayer RCP format notes](https://github.com/jihodaya/jmplayer/blob/main/docs/formats/en/rcp.md)。これは外部資料に基づく説明で、S-MU2000での互換性は合成テストと実データ比較を通じて別途検証する。
- 最新CI結果はまだ未確認。


## 2026-10-24 — Actions #299 合格と D0/D2/D3 対応

- ユーザー報告により Actions #299 の合格を確認。これは当該コミット時点のCI成功であり、以降の変更を含む最新CIの結果ではない。
- RCPコマンド表にある D0 / D2 / D3 を追加対応した。既存デコーダーの Pitch Bend / Control Change / Program Change 経路を再利用し、7-bit値の検証と tick 展開を行う。
- `src/verify.cpp` に各コマンドのデコードと、D2→D3→D0 の時刻累積を検査する合成テストを追加した。
- コード変更コミット: [002c796](https://github.com/pikachu364/S-MU2000/commit/002c796f6099dad901c9b52e29b70ce79827d8fe)、テスト: [a546112](https://github.com/pikachu364/S-MU2000/commit/a546112f311e2331324ed1c6688ae5cf68aeabe5)。関連文書も更新。
- 追加変更のCIは未確認。実データ12曲をC++パーサーへ直接通した検証・実再生も未実施。
