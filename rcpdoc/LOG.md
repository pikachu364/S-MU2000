## 2026-10-29 — Actions #556 合格、4サンプル比較で不一致を4件まで削減

- ユーザー報告により公開リポジトリ Actions #556 の合格を記録。
- private sample-check run [40868277263](https://github.com/pikachu364/S-MU2000-private-data/actions/runs/40868277263) の成果物 [artifact 12894966176](https://github.com/pikachu364/S-MU2000-private-data/actions/runs/40868277263/artifacts/12894966176) を取得し、`event-compare.txt` 4件を確認した。
- ValleyBell参照 commit は `a73c748960d57bd194fb64931ce07d2c1fa1bd01`。sample-01 BUTTFLY は 27,974 events / mismatch 0、sample-02 FC は 20,299 / mismatch 4、sample-03 WINGS_55 は 23,745 / mismatch 0、sample-04 L_WING88 は 22,536 / mismatch 0。
- 全4曲で parser/reference の総イベント数とトラック別イベント数は一致。比較対象は合計 94,554 events、不一致は4件。
- 残る4件は sample-02 Track 6 の tick 5135 と 5263。parser は `96 3C 00` → `B6 0B 7F` / `B6 0B 17`、reference は CC → `96 3C 00` の順。tickやイベント数の差ではなく、同一tick内の順序差。
- `src/rcp.cpp` では固定 Note Off 優先順位を外して安定順を保持する変更を反映済み。この結果、sample-01 / 03 / 04 は完全一致し、sample-02 は16件から4件へ減少した。原因の完全解明や実機再生確認とは区別する。
- 次は sample-02 Track 6 の元RCPレコードと ValleyBell の running-note 期限処理を照合する。RCPデータと成果物はプライベートリポジトリに保持する。

## 2026-10-26 — Actions #460 / private sample-check #10 合格、sample-9 ダンプ確認

- ユーザー報告により公開リポジトリ Actions #460 と private sample-check #10 の合格を確認。
- プライベートの `Sample report files/rcp2-sample-report/sample-9/` に要約レポート、イベントダンプ、選択サンプル情報の3ファイルが存在することを GitHub 上で確認した。
- `rcp2-event-dump-info.txt` は代表曲を `private-data/RCP_DATA/BUTTFLY/BUTTFLY.RCP` と記録している。要約レポートの BUTTFLY ブロックは timeline 28008 events、seconds 28008 events、MIDI encoding 27974 events で各段階 OK。
- これで代表曲のイベントダンプを生成する工程まで確認できた。ダンプの内部 consistency 行、先頭/末尾イベントと複数トラック順の精査、参照コンバーターとのイベント単位比較は未完了。CI 合格を出力互換性の証明とは扱わない。
- 次はプライベート側でダンプの整合性を精査し、参照コンバーターと同条件で比較する。RCP 曲データとイベントダンプは公開リポジトリへコピーしない。

# 作業ログ

日付順の短い記録です。テストを実行したかどうか、何を根拠に判断したかを必ず区別します。

## 2026-10-26 — #456 合格後、イベントダンプ失敗を終了コードに反映

- ユーザー報告で Actions #456 合格を確認。今回の変更はその後に追加するため、この変更自体の CI は未確認。
- `--rcp2-event-dump` のイベント数不一致や診断用 MIDI エンコード失敗を stderr だけでなく終了コード `1` に反映するよう修正。通常の `--rcp2-sample` の挙動は変えない。
- これにより、バッチ実行や後続スクリプトがダンプ失敗を見逃しにくくなる。実サンプルの consistency と参照コンバーター比較はまだ未確認。

## 2026-10-26 — #452 合格報告後、イベントダンプの自己整合性を追加

- #452 は作業者から合格報告を受けた。今回の変更はそれ以後の新しいコミットなので、今回分の CI 合格はまだ未確認。
- `verify --rcp2-event-dump` にイベントごとの 0-based index を追加し、ダンプ件数と MIDI encoder が生成した件数を比較する診断を追加した。
- これにより、行の欠落や診断側のエンコード失敗を比較作業の前に検出しやすくする。これは MIDI 内容の参照コンバーター一致や実再生を証明するものではない。
- 次は CI でコンパイル・回帰テストを確認し、その後 private sample の代表曲で `consistency=OK` を確認する。曲データ・ダンプは公開リポジトリに保存しない。

## 2026-10-26 — Actions #447 合格、比較用イベントダンプを次工程に設定

- GitHub Actions API で run #447 の `conclusion=success` を確認。head commit は `d43b376a69e75e13bb68f5200d96a2a927481a12`（#445 の合格記録を追加したドキュメントコミット）。
- 現行 `verify --rcp2-sample` は timeline / seconds / MIDI encoding の成功とイベント件数を出すが、イベント単位の正規化ダンプや SMF ファイルを生成しないことをコード確認した。
- 参照 MIDI との比較の前提として、次は `track index / absolute time / port / MIDI bytes` を出力する診断モードを設ける。プライベートの RCP データは公開リポジトリに追加しない。
- #447 はドキュメント更新に対する CI 成功であり、新しい RCP 機能や実曲の出力一致を検証した結果ではない。

## 2026-10-26 — Actions #445 合格と次の比較作業

- ユーザー報告の Actions #445 合格を確認し、GitHub Actions API でも run #445 が `success`、head commit が `5a53b6c` であることを確認した。
- #445 は E7 gradual tempo の調査記録を含むコミットの CI 成功であり、実 RCP2 の参照 MIDI との出力一致や実機再生を検証した結果ではない。
- `rcpdoc/NEXT_STEPS.md` の最新 CI 表記を #445 に更新し、重複文を整理した。次の実作業は T-006 の代表曲比較。公開コードリポジトリに private sample の曲データは追加しない。
- 比較では、参照コンバーターの未実装・警告対象（特に E7 gradual tempo）を含む差分を自動的に不具合扱いせず、コマンドごとに根拠を確認する。

## 2026-10-25 — Actions #390 合格、0x99 外部コマンド対応

- ユーザー報告により Actions #390 の合格を確認。これは今回の 0x99 対応コミットより前の状態であり、今回追加したテストの CI 結果ではない。
- Glosa の `RcpConverter.cs` で、0x98 / 0x99 / F6 が後続 F7 レコードを集約してからループ展開されることを確認。
- Glosa は 0x99 ExternalCommand の payload を MIDI 出力せず、外部コマンド指示として無視する。S-MU2000 でも F7 継続を同一グループとして保持し、MIDI イベントは生成せず、p0 による tick 進行を維持する処理を追加。
- 合成テストを追加: 0x99 + F7 が MIDI 出力を生成しないこと、次のノートが p0 分だけ進んだ tick に置かれること、F8/F9 ループ内でグループが保持されること。
- ValleyBell の `rcp2mid.c` と QB64 Recomposer の `RCP.cpp` は 0x99 の変換 case がコメントアウトされている。実際の動作根拠は Glosa の実装。
- 参照: https://github.com/hsshss/glosa/blob/main/src/Glosa.Core/Rcp/RcpConverter.cs
- 今回のソース・テスト変更後の CI は未確認。実 RCP2 ファイル中の 0x99 出現確認も未完了。

## 2026-10-25 — Actions #388 合格、0x99 外部コマンド調査

- 作業者から Actions #388 合格の報告を受けた。直近に追加した FC 展開予算超過テストを含む変更の CI 合格として記録した。
- ValleyBell の `rcp2mid.c` では `0x98` を `GetMultiCmdDataSize` / `ReadMultiCmdData` で読み取る一方、`0x99` の external command case はコメントアウトされている。
- QB64-Phoenix-Edition の Recomposer `RCP.cpp` でも `case 0x99: // Execute external command` はコメントのまま。これらは 0x99 の意味や F7 継続規則を確定する根拠にはならない。
- よって 0x99 を 0x98 と同じ SysEx として実装することは保留。次は Glosa / Gearmulator のソースを直接確認し、実サンプルに 0x99 があるかを調査する。
- 参照: [ValleyBell/MidiConverters rcp2mid.c](https://github.com/ValleyBell/MidiConverters/blob/master/rcp2mid.c)、[QB64-Phoenix-Edition Recomposer RCP.cpp](https://github.com/QB64-Phoenix-Edition/QB64pe/blob/442faf03972acdb88780ffaf8bbfbbeb344ae2fa/internal/c/parts/audio/extras/libmidi/Recomposer/RCP.cpp)
- 今回は引き継ぎ文書の更新のみ。ソースコード変更や追加テストは行っていない。

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


## 2026-10-24 — #304 verify スナップショット不一致の修正

- Actions #304 は `make test` の ROM 不要 `verify` 出力比較で失敗。新しく追加した D0/D2/D3 の合成テスト行が `tests/verify.txt` に無かったことが直接原因で、画面文言テストは合格していた。
- 出力を読みやすくするため、D0/D2/D3 テストの末尾が文字列 `\\n` を表示していた誤りも改め、通常の改行に修正した。
- `tests/verify.txt` に新しいテスト行を追加し、意図した回帰テスト出力として指紋を更新した。コード: [8124802](https://github.com/pikachu364/S-MU2000/commit/81248022b9ce75a47da6b5146ac2389e371b3b12)、スナップショット: [6ff6972](https://github.com/pikachu364/S-MU2000/commit/6ff69722024be7d4245106bf1077afa140187005)。
- 修正後の CI は未確認。`verify` の期待値を無条件で再生成したのではなく、今回追加した行だけを追加した。実 RCP2 曲の C++ パーサー実行・再生は未確認。


## 2026-10-24 — Actions #307 合格後の E1/E2 対応

- ユーザー報告により Actions #307 の合格を確認。新しい E1/E2 変更の CI 合格を意味するものではない。
- JMPlayer の RCP2 コマンド表を照合し、E1 は CC0=velocity、E2 は CC0=velocity・CC32=0・Program Change=gate として展開する処理を追加した。E2 の3イベントは同 tick にファイル記述順で出力し、RCP step は一度だけ加算する。
- 7-bit 範囲外の値を拒否する合成テストを追加。期待出力 `tests/verify.txt` も同期した。
- 関連コミット: [実装](https://github.com/pikachu364/S-MU2000/commit/7788e6fa63e7623843595cdc078d528ce54e2e56)、[テスト](https://github.com/pikachu364/S-MU2000/commit/aa74cf217de259f50dc339bd40bc37f2d90be43c)、[期待出力](https://github.com/pikachu364/S-MU2000/commit/e5478490ddf1d286e2016e2d447c078642ee3508)。
- 参照仕様: [JMPlayer RCP format notes](https://github.com/jihodaya/jmplayer/blob/main/docs/formats/en/rcp.md)。この変更は実 RCP ファイルでの実行・再生を検証したものではない。


## 2026-10-24 — E1/E2 参照実装との照合修正

- ValleyBell/MidiConverters の `rcp2mid.c` の `case 0xE1` / `case 0xE2` を確認した結果、先行実装の E1/E2 の意味付けが誤っていたため修正した。
- E1 は XG 音色選択（CC32=`p2`、Program Change=`p1`）、E2 は GS 音色選択（CC0=`p2`、Program Change=`p1`）。両イベントは同 tick に出し、`p0` の時間進行は一度だけとする。
- 合成テストと期待出力、NEXT_STEPS/CHECKLIST の説明を訂正した。#313 はこの修正より前の成功であり、修正後 CI は未確認。
- 参照: https://github.com/ValleyBell/MidiConverters/blob/master/rcp2mid.c


## 2026-10-24 — Actions #320 合格後の DD/DE/DF 対応

- ユーザー報告により Actions #320 の合格を確認。E1/E2 改訂後の CI 成功として記録する。
- `DD` / `DF` の状態をトラック内の時間展開中に保持し、`DE` で Roland GS DT1 SysEx を生成する処理を追加。初期 device/model/address は参照コード同様にゼロ。チェックサムはアドレスとデータの4 byteを加算して 7-bit 化する。
- `timeline_event_kind::sysex` を追加し、秒ベース MIDI 出力経路で F0...F7 のバイト列をそのまま送出する。合成テストと `tests/verify.txt` を同期。
- 追加変更の CI は未確認。実データ比較・再生は未確認。参照: https://github.com/ValleyBell/MidiConverters/blob/master/rcp2mid.c


## 2026-10-24 — Actions #328 合格後の E6 対応

- ユーザー報告により Actions #328 の合格を確認。直前の DD/DE/DF 対応まで成功。
- E6 を route-change timeline event として追加。参照実装に従い raw p1 を 1 減らし、bit 7 が立つ値（減算後）は dummy/unrouted、その他は上位 nibble を port、下位 nibble を channel として扱う。
- MIDI 出力でトラック別の有効ルートを更新し、E6 後のイベントに適用する。合成テストは通常ルート切替、dummy 切替、tick 進行を対象とする。
- この追加変更の CI は未確認。実 RCP2 ファイルの C++ 実行・実機再生は未確認。参照: https://github.com/ValleyBell/MidiConverters/blob/master/rcp2mid.c


## 2026-10-24 — Actions #338 合格と次の SysEx 調査

- ユーザー報告により Actions #338 の合格を確認。E6 の動的 MIDI ルーティング（port/channel 切替、dummy/unrouted 切替、tick 進行）を含む変更までの CI 成功として記録する。
- `rcpdoc/NEXT_STEPS.md` と `rcpdoc/CHECKLIST.md` を現コードに合わせて更新し、E6 を実装・合成テスト済みとして記録した。今回の文書変更自体の CI は未確認。
- 次の候補を `0x90–0x97` のヘッダー定義ユーザー SysEx に定めた。参照実装では `0x90–0x97` が8個のユーザー SysEx スロットを呼び出し、テンプレート中の置換値を処理する。ヘッダー内のスロット配置・テンプレート終端・置換規則をコード化する前に確認する。
- `0x98` は可変長 SysEx データを読み込むため、現行の固定4-byte `parse_sequence` では直接処理できない。単純な4-byte commandとして実装せず、可変長レコードの境界と FC/F8/F9 ループ展開との関係を設計課題として残す。
- E5 は参照実装で明示的な command handler を確認できていないため、根拠なく意味を割り当てない。
- 実 RCP2 ファイルを C++ パーサー／出力経路に通した検証と実機再生は引き続き未実施。


## 2026-10-25 — #341 合格後の 0x90–0x97 ユーザー SysEx 実装

- ユーザー報告により Actions #341 の合格を確認。E6 までの変更に対する CI 成功として記録する。
- ValleyBell/MidiConverters の `RCPFormat.txt` と `rcp2mid.c` を照合。RCP2 固定ヘッダーの `0x406` から8個の `0x30` byte スロットを読み、各スロットは `0x18` byte の名前と `0x18` byte の SysEx テンプレートから成る。末尾 `F7` はパディングとして除去し、コマンド展開時に必要な終端を補う。
- `0x90–0x97` を対応するテンプレートスロットへ接続。テンプレート内の `0x80`=p1、`0x81`=p2、`0x82`=現在の MIDI channel、`0x83`=Roland checksum 初期化、`0x84`=checksum 挿入を処理する。MIDI SysEx の `F0` / `F7` を含め、イベント時刻は現在 tick、コマンドの p0 は次イベント位置へ一度だけ加算する。E6 の dummy route 中は SysEx を出さない。
- `src/verify.cpp` と `tests/verify.txt` に、ヘッダーテンプレート抽出、置換、チェックサム、E6後のchannel、tick、dummy抑止、7-bit範囲外拒否の合成テストを追加した。
- 今回の実装・テスト・文書更新後の CI は未確認。実 RCP2 12曲の C++ パーサー実行と実機再生も未実施。
- 次は `0x98` の可変長 SysEx を設計する。固定4-byte parser と同時に無理に処理せず、F7 継続レコードの消費位置、FC/F8/F9 展開、FE終端、切断入力をまとめてテストする。
- 参照: https://github.com/ValleyBell/MidiConverters/blob/master/RCPFormat.txt と https://github.com/ValleyBell/MidiConverters/blob/master/rcp2mid.c


## 2026-10-25 — #352 合格後の 0x98 Track SysEx

- ユーザー報告により Actions #352 の合格を確認。#352 は今回の 0x98 追加前の状態として記録する。
- Glosa の `RcpConverter.cs` を確認し、Track Exclusive (0x98) はイベント自身の p1/p2 ではなく、後続する F7 レコードの p1/p2 を連結してデータ化する実装を参考にした。末尾の F7 はパディングとして除去する。
- S-MU2000 のタイムライン展開で、FC/F8/F9 展開後に 0x98 と連続 F7 レコードをまとめ、0x80=p1、0x81=p2、0x82=channel、0x83=checksum reset、0x84=checksum insert を処理して SysEx イベント化するようにした。コマンド p0 は一度だけ tick に加算する。
- `src/verify.cpp` と `tests/verify.txt` に、複数 F7 の結合、置換、checksum、tick、孤立 F7 拒否の合成テストを追加。
- 今回の変更後の CI と実 RCP2 データの C++ 実行は未確認。Gearmulator PR #303 との詳細比較も継続する。
- 参照: https://github.com/hsshss/glosa/blob/main/src/Glosa.Core/Rcp/RcpConverter.cs と https://github.com/dsp56300/gearmulator/pull/303


## 2026-10-25 — #358 合格後の 0x98 ループ境界テスト

- ユーザー報告により Actions #358 の合格を確認。
- `0x98` + `F7` 継続レコードの組が F8/F9 のループ本体にある場合、展開後の各反復で独立した SysEx として出力され、イベント tick が一回分ずつ進む回帰テストを追加。
- 不正な high-bit 継続データに対し、失敗時に出力タイムラインが空になることも追加検査。
- この変更後の CI は未確認。実 RCP2 ファイルでの C++ 実行・再生互換性も引き続き未確認。


## 2026-10-25 — #364 合格後の空 Track SysEx テスト

- ユーザー報告により Actions #364 の合格を確認。
- Glosa の `TrackExclusive` 処理との比較を踏まえ、F7 パディングだけの継続データでは空 SysEx を出力せず、p0 の tick 進行だけは反映する合成テストを追加。
- このテスト追加後の CI は未確認。実 RCP2 データでの C++ 実行・再生互換性も引き続き未確認。


## 2026-10-25 — #370 の空 payload テスト修正

- ユーザー報告の Actions #370 で空 Track SysEx の回帰テストだけが NG。実装失敗ではなく、テストがイベント開始 tick と p0 による次イベントへの時間進行を混同していた。
- `0x98 p0=2` の次にある音符は tick 2 で始まるため、`src/verify.cpp` の期待値を tick 5 から tick 2 に修正した。
- 修正後の CI は未確認。合格扱いにしない。


## 2026-10-25 — #374 合格後の 0x98 継続レコードとループ境界

- Glosa のループ展開処理を再確認。0x98 / 0x99 / F6 は直後の F7 継続レコードを同じイベントとして先に集約してからループ展開する。F8 の外にある孤立 F7 は先行するループ内 0x98 には接続しない。
- `expand_f8_f9_loops` で 0x98 / F6 と直後の F7 を一組として複製するよう修正。時間展開時の孤立 F7 はスキップし、参照実装の挙動に合わせた。
- `src/verify.cpp` と `tests/verify.txt` に、F8 の外側の F7 が最後のループ反復の SysEx payload へ誤接続しないテストを追加。
- この変更後の Actions は未確認。0x99 外部コマンドの意味処理と実 RCP2 データの C++ 実行・再生比較は未完了。


## 2026-10-25 — #384 合格後の FC 展開予算テスト

- ユーザー報告により Actions #384 の合格を確認。これは孤立 F7 テストの変数名参照修正までの成功であり、今回追加した FC 展開予算テストの CI 結果ではない。
- `src/verify.cpp` に FC の走査予算超過テストを追加し、100万件を超える入力で文書レベル展開が失敗し、部分タイムラインを残さず、エラーを返すことを検査する。`tests/verify.txt` の期待出力も同期した。
- 関連コミット: [FC budget test](https://github.com/pikachu364/S-MU2000/commit/366a0aec278258d54b10afa6c5c4e067f1411b76)、[baseline update](https://github.com/pikachu364/S-MU2000/commit/b79d7b8ec4fe302dc0333e8af90eeaf0969267aa)。
- 追加変更後の CI は未確認。実 RCP2 データの C++ 実行・実曲再生も未確認。
- 次は `0x99` 外部コマンドの後続 F7 継続レコードの意味を参照実装と照合する。`0x98` と同一視せず、確認済み挙動と仮説を分けて記録する。


## 2026-10-25 — Actions #395 合格と 0x99 調査の整理

- GitHub Actions API で [Actions #395](https://github.com/pikachu364/S-MU2000/actions/runs/40150355286) を確認。Linux、Windows、macOS、MSVC x64、README、Windows 32-bit、MSVC Win32 の全7ジョブが success。
- #395 の対象コミットは `4bb789a`。この結果は同コミット時点の CI 成功を示し、後続の文書更新コミットの CI 成功を意味しない。
- `src/rcp.cpp` を再確認し、`parse_tracks` がトラック長と `FE` 終端を検証し、`build_intermediate` が解析済みレコードを中間表現へ移すことを確認した。今回、プライベート12曲をこの C++ 関数に直接実行したわけではない。
- 独立したレコード境界スキャンでは、12曲のトラック内コマンド位置に `0x99` は見つからなかった。ヘッダー等に存在する生バイトの `0x99` はコマンドとして数えない。よって実曲から外部コマンド payload の具体例・用途は確認できていない。
- `rcpdoc/CHECKLIST.md` の 0x99 項目を、実装・合成テスト済み、実データでの確認は未完了と区別して更新。 `rcpdoc/NEXT_STEPS.md` に Actions #395 と12曲スキャン結果を反映。
- 次は現行 C++ `parse_tracks` / `build_intermediate` で12曲を直接処理し、各曲の成功・失敗とエラーを記録する。続けて未対応コマンドの実データ頻度と展開失敗を整理する。


## 2026-10-25 — Actions #406 合格と実 RCP2 サンプル診断

- ユーザー報告により Actions #406 合格を確認。添付 ZIP の `rcp2-sample-report.txt` を確認し、12曲すべてが `OK; parsed_tracks=18; intermediate_tracks=18` であることを確認した。
- F6/F7 診断は計3,245件。大半が10レコードの連鎖として報告される。これは連鎖候補を観測した結果であり、コメント等の内容を復元できたことや、すべての連鎖が意味的に正しいことを示すものではない。
- 孤立 F7 は1件: `CHISA140.RCP` track[0] record 994、p0=00 p1=41 p2=10。前後のレコードや参照実装の根拠を確認するまでは、異常として扱わない。
- F8/F9 の数が一致しない実例として、DINA track[10] の F9=1 / F8=0（record 35: F9 00 00 00）と、WINGS_55 track[4] の F8=44 / F9=45 を確認。単純な個数差だけで不正と判断せず、ループ展開後の実際の対応関係を調べる。
- 実データを使った構造解析の成功であり、MIDI 出力の比較・実曲再生互換性は未確認。
- `rcpdoc/NEXT_STEPS.md` に調査結果と次の作業を記録。ソースコードの意味論はこのレポートだけを根拠に変更していない。


## 2026-10-25 — F7 / F8-F9 診断の精度向上

- `src/verify.cpp` のサンプル診断を拡張した。F8/F9 数が異なる場合、単純なスタック照合で unmatched F8 / unmatched F9 の位置を追加表示する。
- F7 継続の診断対象を F6 に限定せず、0x98 / 0x99 の直後に続く F7 もグループとして報告する。直前が F6、0x98、0x99、F7 の F7 を誤って孤立扱いしない。
- 孤立 F7 が残る場合、前後最大3レコードの code/p0/p1/p2 を出力する。これで CHISA140.RCP の record 994 の周辺データを次のレポートから確認できる。
- コミット: [11809a7 — Improve RCP2 loop and F7 sample diagnostics](https://github.com/pikachu364/S-MU2000/commit/11809a7bfdf80fd3d6a91148121dd17dbd801403)。
- この変更は診断表示のみで、RCP パーサーの意味論やサンプル受理条件を変えていない。変更後の CI と更新レポートは未確認。


## 2026-10-25 — sample-4 F8/F9 診断の再照合

- private sample report のブロック境界を再確認し、F8/F9 不一致のサンプル名を訂正した。実例は MIND.RCP track[10] record 35 の F9 単独と、WINGS_55.RCP track[4] record 2184 を未対応として残すネスト列。
- 以前の「DINA track[10]」という記録は誤り。DINA track[10] は `MELO1`、F8/F9 は 0/0。
- ValleyBell の参照コードでは F9 がループスタック開始、F8 がその処理を行い、トラック全体のループ拡張用 `loopOfs` も別に扱われる。現状の材料だけで孤立 F9 の仕様は断定しない。
- 現時点でソースコードの意味論は変更していない。MIND の record 35 と WINGS の record 2122–2210 の生レコードを確認した。MIND の F9 は `F9 00 00 00`、WINGS の record 2184 は `F9 01 00 00` で、後者の内側にある record 2194 の F9 は record 2207 の F8 と対応する。よって record 2184 は未対応の外側マーカーとして残る。
- 次は参照実装の事前走査が unclosed F9 をどう扱うかを確認し、有限ループとの区別を合成テストで固定してから実装を変える。
- 参照: https://github.com/ValleyBell/MidiConverters/blob/master/rcp2mid.c


## 2026-10-25 — Actions #416 合格と F8/F9 テスト修正

- ユーザー報告により Actions #416 の合格を確認。全ジョブの詳細はこの会話では取得していない。
- E7 の「未対応コマンド混在拒否」テストが、実際には対応済みの F5 キー署名を入力していたため、未対応コマンド F4 に変更。これにより、対応コマンドを誤って拒否テストに使わないようにした。
- F8/F9 について、対応する F8 のない F9 をトラック全体ループ候補として許容し、有限 F8/F9 ペアだけを展開対象にする実装と、未閉鎖の外側 F9 の内側に有限ネストループがある合成テストを維持。
- #416 合格は CI と合成テストの成功を示す。MIND / WINGS の実サンプルでの timeline 展開、MIDI 出力比較、実曲再生は未確認。
- 次は実サンプル12曲について構造解析から先へ進み、timeline 展開と MIDI 出力の成功/失敗および未対応コマンドを曲ごとに記録する。


## 2026-10-25 — 実 RCP2 サンプルの後段パイプライン診断を追加

- `src/verify.cpp` の `--rcp2-sample` 出力に、`expand_intermediate_timeline`、秒変換、トラック MIDI マッピング、MIDI イベント符号化の段階別診断を追加（commit `91c5ed053a53dd21980e93290831494f417891b8`）。
- 各段階は `OK` / `FAIL` とイベント件数またはエラーを表示する。生データの構造解析結果と後段の対応状況を混同しないよう、後段の失敗は診断として出力し、サンプルの構造解析チェック自体は失敗扱いにしない。
- 次は public branch の CI でコンパイル/テストを確認後、private sample-check を再実行し、12曲の timeline / seconds / MIDI 結果を回収する。現時点ではこの新診断を使った実サンプル結果は未取得。


## 2026-10-25 — Actions #421 合格後のトラック別パイプライン診断

- ユーザー報告により Actions #421 の合格を確認。これは今回追加する診断変更より前の CI 結果として扱う。
- `--rcp2-sample` の全体パイプライン診断だけでは、最初の未対応トラックで処理が止まり、他トラックの状態を確認しにくい。
- 各中間トラックを単独の文書として検査し、timeline 展開、秒変換、MIDI マッピング、MIDI 符号化の各段階を個別に表示する診断を追加。診断専用であり、受理条件や変換意味論は変更しない。
- この追加後の CI と、診断を再実行した12曲のレポートは未確認。次は結果を回収し、失敗したトラックのエラーとコマンド頻度を照合して、対応コマンドを優先順位付けする。


## 2026-10-25 — sample-5 で見つかった E5 Key Scan の未対応

- private repository の `sample-5/rcp2-sample-report.txt` を読み、12曲すべて構造解析は成功、11曲は全体 timeline → seconds → MIDI event encoding まで成功していることを確認。
- `CHISA140.RCP` の track[0]（ドラムス）だけ全体 timeline と単独 track pipeline が失敗。コマンド histogram に `E5=1` があり、エラーは現行の時間展開で許可していないコマンドを示していた。
- 参照実装 `ValleyBell/MidiConverters/rcp2mid.c` の `case 0xE5` は “Key Scan” として警告のみを出し、MIDI イベントを生成しない。これに合わせて `src/rcp.cpp` で E5 を出力イベントなしとして処理し、p0 の tick 進行を維持するよう修正。
- `src/verify.cpp` に E5 が MIDI イベントを増やさず p0 分だけ次イベントを進める合成テストを追加し、`tests/verify.txt` の期待出力を同期。
- 修正コミット: [b7de799](https://github.com/pikachu364/S-MU2000/commit/b7de7999e7c79495638421121c7894d5d65ae036)、[回帰テスト](https://github.com/pikachu364/S-MU2000/commit/d1714da)、[期待出力更新](https://github.com/pikachu364/S-MU2000/commit/e54cc73)。この変更後の CI と private sample-check は未確認。
- 次は Actions でビルド・テストを通し、private sample-check を再実行して CHISA140 の timeline / MIDI 変換が解消したかを確認する。実曲の聴感・完全な互換性までは未確認。


## 2026-10-25 — Actions #431 合格と sample-6 の E5 修正再検証

- ユーザー報告により Actions #431 合格を確認。E5 Key Scan 対応後の CI 成功として記録する。
- private repository の [sample-6 レポート](https://github.com/pikachu364/S-MU2000-private-data/blob/main/Sample%20report%20files/rcp2-sample-report/sample-6/rcp2-sample-report.txt) を確認。12曲すべてが `parsed_tracks=18` / `intermediate_tracks=18`。
- 12曲すべてで全体の `timeline`、`seconds`、`midi_encode` が `OK`。各曲の `track_pipeline` でも失敗行は見つからなかった。
- sample-5 で失敗していた CHISA140.RCP は、今回 `timeline: OK; events=128539`、`seconds: OK; events=128539`、`midi_encode: OK; events=128533`。E5 Key Scan 対応で、以前の全体 timeline 失敗は解消したと判断できる。
- これは現行診断パイプラインが12曲を MIDI イベント列へ符号化できたことを示す。参照コンバーターとのバイト単位の出力一致、MIDI ファイルの実再生、聴感上の互換性を示すものではない。
- 次はプライベートの実曲データを公開リポジトリへコピーせず、参照コンバーターとの比較方法を整え、代表曲の出力差分を調べる。比較条件・入力・イベント数・差分を記録するまでは互換性完了としない。


## 2026-10-25 — Actions #433 合格後の E7 gradual tempo 調査準備

- ユーザー報告により Actions #433 の合格を確認。これは E7 パラメータ診断を追加する前の CI 結果として記録する。
- 実サンプル12曲では E7 が複数曲に出現するため、段階的テンポ変化（p2）の意味を推測で確定せず、実データで使われている p1/p2 の組合せを先に観測する方針とした。
- `src/verify.cpp` の `--rcp2-sample` に、E7 の `p1/p2` 組合せ別件数を出力する診断を追加。既存の command histogram とは別に表示し、E7 gradual の意味論や変換結果は変更していない。
- 追加コミット: [c22f498](https://github.com/pikachu364/S-MU2000/commit/c22f49844f5d85833ef13f47205d76673648064f)、改行修正 [62676b2](https://github.com/pikachu364/S-MU2000/commit/62676b2bb8c697423c5fef8ec65a1da82d93ff1e)。
- この診断追加後の CI は未確認。次は CI 合格後に private sample-check を再実行し、p1/p2 の組合せを集計して参照実装の gradual tempo 処理と照合する。


## 2026-10-26 — Actions #443 合格後の E7 gradual tempo 調査

- ユーザー報告により Actions #443 の合格を確認。これを直近の CI 成功として記録する。
- `RCPFormat.txt`（ValleyBell/MidiConverters）の E7 記述を再確認。p1 はテンポ倍率（0x40=100%）、p2 は補間に使う tick 数で、p2=0 は即時変更、p2=1..FF はその tick 数にわたる補間と説明されている。
- 同資料は補間の曲線・tick ごとの更新規則までは説明していない。`rcp2mid.c` も p2 が非ゼロの場合に警告しつつ目標テンポを即時設定するため、元プレイヤーの補間アルゴリズムの根拠にはならない。
- 追加の公開ソース検索でも、元プレイヤーの補間曲線や更新規則を示す一次資料は見つからなかった。複数の変換ツールが補間を省略していることを、元仕様の証明として扱わない。
- したがって E7 の補間実装は保留し、未確認の曲線を推測で追加しない。既存の `gradual` 値保持と「interpolation not implemented」の診断を維持する。
- 次の実作業は、プライベートの実 RCP2 サンプルを公開リポジトリへコピーせず、代表曲の参照コンバーター出力と S-MU2000 の MIDI イベント列を同じ条件で比較する手順を整えること。CI 合格と実曲互換性は引き続き区別する。


## 2026-10-26 — 決定的な MIDI イベント診断ダンプを追加

- `src/verify.cpp` に `--rcp2-event-dump` を追加。既存の `--rcp2-sample` は要約出力を維持し、必要な時だけイベント単位の行を出す。
- ダンプ行は track index、絶対 tick、秒、port、MIDI bytes を固定書式で記録する。MIDI 符号化成功後にのみ出力し、tempo/key-signature のように MIDI channel bytes を持たないイベントは除外する。E6 route change は状態を更新し、それ自体は MIDI message 行として出さない。
- これは参照コンバーターとの差分調査を可能にする診断基盤で、出力互換性を証明するものではない。
- このコード変更後の CI と実サンプル実行は未確認。次は Actions の結果を確認し、代表曲のイベントダンプで比較条件とイベント順を検証する。


## 2026-10-26 — Actions #458 合格とイベントダンプの失敗判定

- ユーザー報告により Actions #458 の合格を確認。
- #456 の `build` / `linux` ジョブは、同じブランチで優先度の高い待機リクエストがあるため自動キャンセルされた。コンパイル・テスト失敗を示すログではなく、#458 が合格したため #456 の再実行は不要と判断した。
- `--rcp2-event-dump` はイベント件数の不一致または MIDI エンコード失敗を検出すると終了コード 1 を返す。通常の `--rcp2-sample` の終了条件は変えていない。実装コミット: [ace293e](https://github.com/pikachu364/S-MU2000/commit/ace293edb02f8cc6fe151217143274684e593e05)。
- `rcpdoc/NEXT_STEPS.md` の T-008 を更新し、CI 合格を記録。次は private の代表曲1曲でイベントダンプを実行し、終了コード、件数、先頭/末尾、トラック順を確認する。その後、参照コンバーターと同じ条件でイベント列を比較する。
- 実サンプルのイベントダンプ、参照出力との一致、実再生互換性はまだ未確認。曲データやイベントダンプを公開リポジトリにコミットしない。


## 2026-10-28 — sample-26 同一tick順序の再調査

- private sample-26 の reference comparison 4件を確認。イベント総数は4サンプルすべて一致しているが、順序・内容を含む比較で sample-01: 627、sample-02: 16、sample-03: 598、sample-04: 3件の不一致。
- L_WING88 の3件は track 12 / tick 3648 に集中し、CC controller 7、CC controller 32、Note On velocity 0 の順序差。FC の代表例は track 6 / tick 5135・5263 で CC と Note Off の順序差。
- 現行の同tick優先処理は CC・Note Off・新規 Note の3種類が共存する場合だけなので、これらの差分には適用されない。サンプル間で必要な順序が異なるため、イベント種別だけによる固定優先順位を追加する根拠はない。
- 調査結果と次の追跡手順を private repository の報告書にも記録した。今後は参照コンバーターの running note 終了処理と元RCPレコード順を追跡し、順序差とイベント集合差を分けて原因を特定する。
- 今回は調査記録のみ。ソースコード修正、CI再実行、互換性合格の確認は行っていない。


## 2026-10-28 — Actions #510 合格後、参照側 Note Off 順序のソース確認

- ユーザー報告により Actions #510 合格を記録。
- 参照コンバーター `ValleyBell/MidiConverters` の固定 commit `a73c748960d57bd194fb64931ce07d2c1fa1bd01` で `rcp2mid.c` と `midi_utils.h` を読解。
- `RcpTrk2MidTrk()` は各コマンド処理前に `CheckRunningNotes()` を実行し、期限切れ Note Off を現在のコマンドより前に出力する。複数の期限切れノートは running-note 配列順に処理される。
- したがって、同一 tick の並べ替えだけで全差分を直すのではなく、Note Off の生成・期限判定・コマンド出力タイミングを再現する必要がある。
- sample-26 の CC → Note Off 差分の原因はまだ断定していない。入力コマンド由来か自動生成 Note Off かを切り分けてから実装を変更する。今回、コード変更・CI再実行・private sample再比較は行っていない。


## 2026-10-28 — Gearmulator RCP2 参照実装との照合

- [gearmulator commit 619b1d9](https://github.com/masanaohayashi/gearmulator/commit/619b1d9b7f7755ea7eee3bd93d8dd2a705e79465) の `RcpFilePlayer.cpp` を確認。
- `decodeTrackLength()` は格納値の bit 0-1 を bit 16-17 に移して拡張トラック長を復元する。S-MU2000 の現行コードは2 byte値を直接トラック長として使っていた。
- Gearmulator には旧形式 ST+ の符号解釈、最大36トラック、R36 のポート/チャンネル割当、SysEx F0 の正規化、期限到来ノートの処理、FC 連鎖の実装もある。これらは今後の照合対象として記録し、今回の変更と混同しない。
- この時点では参照実装の調査のみ。公式仕様との確定、CI 合格、実曲出力一致を意味しない。


## 2026-10-28 — sample-27 参照比較レポート確認

- ユーザー報告：Actions #516 は macOS job が混雑で未完了、その他ジョブは合格。
- private `sample-27/reference-comparisons` の sample-01〜04 を確認。参照コンバーター commit は `a73c748960d57bd194fb64931ce07d2c1fa1bd01`。各曲で parser / reference の総イベント数とトラック別イベント数は一致。
- それでも完全一致ではなく、順序比較の mismatch は sample-01=627、sample-02=16、sample-03=598、sample-04=3。全差分の tick 値は各ペアで一致しており、まず同一 tick 内のイベント順を調べる。
- sample-04 は track 12 / tick 3648 に `BC 07 61`、`BC 20 00`、`9C 36 00` の順序違いが3件。sample-02 でも CC / Note On / Pitch Bend の同一tick順序違いが見られる。現時点では原因を単一の固定優先順位に一般化しない。
- 比較レポート確認と NEXT_STEPS への記録を実施。ソース変更・CI再実行・実再生は未実施。

## 2026-10-29 — sample-27 同一tick差分の参照処理を追加調査

- 公開リポジトリの現行 `src/rcp.cpp` を確認。`expand_note_gates()` は Note On の走査中に Note Off 相当イベントを `expanded` 配列へ事前挿入し、後段の `stable_sort` は tick のみを比較するため、同一 tick は原則として元配列順を保つ。
- 現行の CC → Note Off → 新規 Note の優先順位処理は、3種類すべてが同一 tick に存在するグループに限られる。今回の sample-04 の CC2件と Note On velocity 0 の差分や、sample-02 の CC / Note Off 差分を一般的に説明できない。
- ValleyBell/MidiConverters の固定 commit `a73c748960d57bd194fb64931ce07d2c1fa1bd01` にある `midi_utils.h` の `CheckRunningNotes()` を追加確認。参照実装は各 RCP レコード処理前にノート残り長と delay を更新し、期限到来の Note Off 相当を現在のレコードの出力より前に発行し得る。
- 期限処理と現行実装の事前挿入方式はタイミングが異なるため、差分原因の候補ではある。ただし各問題 tick の実際の `remLen`、`*delay`、元 RCP レコードとの対応付けは未取得であり、根本原因とは断定しない。
- 参照コンバーターの `MidiDelayHandler` 定義と delay 更新の全経路、および private sample-02 / sample-04 の問題イベントと元 RCP レコードの対応付けは未完了。
- ソースコード修正、CI 再実行、サンプル再比較は今回実施していない。推測による固定優先順位は追加しない。RCP データとイベントダンプは private repository 内に保持する。

## 2026-10-29 — Actions #533 合格、参照側 delay callback の呼び出し経路を追跡

- ユーザー報告により Actions #533 の合格を記録。これは CI 成功の報告であり、sample-27 のイベント順序差分が解消したことを意味しない。
- ValleyBell/MidiConverters の固定 commit `a73c748960d57bd194fb64931ce07d2c1fa1bd01` で `rcp2mid.c`、`midi_utils.h`、`midi_funcs.h` を確認。
- `RcpTrk2MidTrk()` は各 RCP レコードから `cmdP0Delay` と `cmdDurat` を読み、イベント出力時の `WriteMidiDelay()` 経由で `MidiDelayHandler()` が呼ばれる。ハンドラーは `midiTickCount += *delay` の後に `CheckRunningNotes()` を呼び、残った delay 分を running note の `remLen` から減算する。
- ノートレコードでは通常の MIDI Note On 出力より前に `CheckRunningNotes(fInf, &MTS->curDly, ...)` が直接呼ばれる。既存ノートと同じ音高なら、Note On を追加せず `remLen = MTS->curDly + cmdDurat` として期限を延長する。
- 各コマンド処理後には `MTS->curDly += cmdP0Delay` が実行される。したがって、遅延の処理は「すべての RCP レコードごとに同じタイミングで一律実行」ではなく、Note On 経路、MIDI イベント出力経路、delay callback の組合せに依存する。
- トラック終端では `FlushRunningNotes(..., cutNotes=0)` を呼び、残存ノートを完全に終了させるために delay を延長する場合がある。
- `MidiDelayHandler` と delay 更新の主要経路は確認できたが、sample-02 / sample-04 の問題 tick に対応する元レコード、実際の `MTS->curDly` / `remLen` 値はまだ取得していない。根本原因は未確定。
- コード修正・private sample 再比較は未実施。次は代表差分の元レコード位置と累積 delay / ノート期限を照合する。

## 2026-10-29 追記 — sample-27 の代表差分を元サンプル情報へ再対応付け

- private sample-27 の `info.txt` と `event-compare.txt` を再取得し、sample-02 は `RCP_DATA/FC/FC.RCP`、sample-04 は `RCP_DATA/L_WING88/L_WING88.RCP` であることを確認。参照コンバーターは両方とも固定 commit `a73c748960d57bd194fb64931ce07d2c1fa1bd01`。
- sample-02 の16件は全て tick が一致し、イベント順序差。track 2: tick 13092（CC / Note On velocity 0 / Pitch Bend）、13128（CC / Note On velocity 0）、13524（CC / Note On velocity 0）。track 5: tick 8892 に CC と複数の Note On velocity 0 が同居。track 6: tick 5135 と 5263 に Note On velocity 0 と CC の順序差。
- sample-04 は track 12 / tick 3648 の3件で、parser `BC 07 61 → BC 20 00 → 9C 36 00`、reference `9C 36 00 → BC 07 61 → BC 20 00`。両者の総イベント数・トラック別イベント数は一致。
- private の parser dump は各約1.8〜2.0 MBで存在と blob SHA を確認したが、GitHub connector から全内容を読み出せなかった。よって各差分を元 RCP の byte offset / record index に対応付ける作業はまだ完了していない。
- 次はローカル/private runner で比較ツールに元 RCP レコード位置（track、record index、byte offset、4-byte record、累積 tick）を出力させ、問題 tick の直前・同tickのレコード列を採取する。公開リポジトリに曲データやダンプをコピーしない。


## 2026-10-29 — Action #42 のZIP確認と同一tick差分の追跡

- private Actions #42 の artifact `rcp2-sample-validation-reports` を GitHub から直接取得し、ZIP整合性検査が成功し31ファイルを収録していることを確認した。ユーザーが最初にアップロードしたZIPの不完全さとは区別して記録する。
- 比較レポートで sample-01〜04 の parser/reference イベント総数が一致し、不一致数が 627 / 16 / 598 / 3 であることを再確認した。
- sample-04 Track 12 / tick 3648 は parser `BC 07 61 → BC 20 00 → 9C 36 00`、reference `9C 36 00 → BC 07 61 → BC 20 00`。現行 `src/rcp.cpp` の同tick優先順位条件は CC・Note Off・新規 Note の3種類すべてが存在する場合に限定されることをソースで確認した。
- この順序差が Note Gate 展開と参照側 running-note 期限処理の違いに由来する可能性はあるが、元RCPのレコード位置・実際の期限値は未取得であり、原因は未確定。
- `NEXT_STEPS.md`、`CHECKLIST.md`、`FACTS.md`、`HYPOTHESES.md` に優先タスク、確認済み事実、未確認仮説、次の検証方法を追記した。
- 今回は記録更新のみ。ソースコード・テスト変更、CI再実行、実再生は行っていない。次は private runner で元RCPレコードとイベント差分を対応付ける。


## 2026-10-29 — follow-up on sample-04 SysEx 0xFF

- Reviewed current S-MU2000 src/rcp.cpp around the 0x98 SysEx expansion loop. It explicitly skips unknown high-bit template bytes such as 0xFF and documents that this matches the reference RCP2 converter behavior.
- Reviewed the reference converter ProcessRcpSysEx() at commit a73c748960d57bd194fb64931ce07d2c1fa1bd01; it warns for unknown high-bit bytes and does not append them to output.
- **Conclusion:** the warning is not currently evidence of a S-MU2000/reference behavior mismatch. The meaning/origin of the source bytes and relation to event-order mismatches remain unknown.
- No source code change, test run, or playback check was performed.


## 2026-10-29 — Actions #567 合格・private sample 4件完全一致

- ユーザー報告で公開リポジトリ Actions #567 の合格を確認。
- private sample-check run [40885695400 / artifact 12902283152](https://github.com/pikachu364/S-MU2000-private-data/actions/runs/40885695400/artifacts/12902283152) のZIPを取得し、各 `event-compare.txt` を確認。
- sample-01 BUTTFLY: 27,974 events / mismatch 0 / exact_match=true。
- sample-02 FC: 20,299 events / mismatch 0 / exact_match=true。前回残っていた Track 6 / tick 5135 と 5263 の2箇所を含め、差分ゼロ。
- sample-03 WINGS_55: 23,745 events / mismatch 0 / exact_match=true。
- sample-04 L_WING88: 22,536 events / mismatch 0 / exact_match=true。
- 合計 94,554 events。全サンプルで各トラックのイベント件数も一致。参照は ValleyBell rcp2mid commit `a73c748960d57bd194fb64931ce07d2c1fa1bd01`。
- 結論はこの4ファイルの参照出力との完全一致。実機再生、すべてのRCP2機能、他のRCPファイルまで保証するものではない。

## 2026-10-30 — sample-04 SysEx 0xFF の追加調査

- ValleyBell `ProcessRcpSysEx()` と `rcm2smf.js` の `convertSysEx()` をソースで比較。前者は未知の上位ビット付きバイトを警告してスキップ後に続行、後者は生成データの 7-bit 範囲検証で定義全体を無効にするため、警告後の挙動は異なる。
- 前回の暫定メモにあった「`0xFF` を `0x7E` 相当として扱うと後続チェックサムが整合する」という主張は独立再計算で裏付けられず撤回。現時点では置換値を示すチェックサム証拠はない。private の生バイト列は公開文書に転記していない。
- ValleyBell の固定版 `RCPFormat.txt` では、この `0xFF` 置換規則は見つからなかった。
- S-MU2000 の現行処理は ValleyBell と同様に未知バイトをスキップしている。コード変更は行わず、仕様上の意味を追加調査する。イベント列の完全一致は SysEx の意味的正しさや実機再生を証明しない。


## 2026-10-30 — SysEx 0xFF 調査の再検算・訂正

- ValleyBell `ProcessRcpSysEx()` の実装を再確認。未知の高ビットバイト `0xFF` は警告後に出力・チェックサム加算されず、その後の入力処理は継続する。
- `rcm2smf.js` の `convertSysEx()` は `0xFF` を通常データとして評価し、7-bit 範囲外のため `null` を返す。呼び出し側はその SysEx 定義を無視する。
- 先の記録にあった `0xFF → 0x7E` とチェックサムが整合するという記述は、独立再計算で確認できず撤回した。置換仮説の根拠として使わない。
- RCPFormat.txt にも今回の `0xFF` の意味や置換規則は確認できていない。コード変更なし。次は RCP レコードの原位置と、MIDI 出力に実際に生成される SysEx を private 環境で抽出して仕様に照らす。公開リポジトリへサンプル本体・ダンプは転記しない。
