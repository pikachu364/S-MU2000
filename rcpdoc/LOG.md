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
