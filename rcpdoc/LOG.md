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
- `src/verify.cpp` に FE 後の不完全な断片を扱うテストを追加したコミットを参照し、実行検証は未完了と明記した。
- 詳細: [2026-10-16 作業レポート](reports/rcp2-work-log-2026-10-16.md)

## 2026-10-20 — 引き継ぎ用ドキュメント構造を作成

- Glosa の `docs/reverse/` を参考に、事実・仮説・次の作業・日付順ログを分ける構成を採用した。
- `rcpdoc/` を新設し、過去レポートを保存したまま参照できる形で収録する。
- 今回はドキュメントの整理のみ。ビルド、テスト、MIDI 再生は実施していない。

## 2026-10-20 — JMPlayer RCP 資料を調査・記録

- JMPlayer の [RCP format notes](https://github.com/jihodaya/jmplayer/blob/main/docs/formats/en/rcp.md) と [rcpfilehandler.cpp](https://github.com/jihodaya/jmplayer/blob/main/rcpfilehandler.cpp) を閲覧した。
- 資料が報告する 44-byte track header、transpose / delay / mute、時間を進めないイベント、`0xFC` repeat、未解決の `0x98`、GSD→SysEx 処理を、S-MU2000 での確認事項として整理した。
- 外部資料の記述は JMPlayer 側の検証報告であり、S-MU2000 の実装・テストで確認済みとは扱わない。
- 今回はドキュメント変更のみ。S-MU2000 のコード変更、ビルド、テスト、MIDI 再生は実施していない。
- 詳細: [JMPlayer RCP 調査ノート](references/jmplayer-rcp-notes.md)

## 2026-10-20 — 実装チェックリストを作成

- JMPlayer資料のヘッダー、イベント、時間制御、ループ、未解決項目を、S-MU2000側のコード確認・テスト追跡用 `CHECKLIST.md` に整理した。
- JMPlayer資料のチェックマークと、S-MU2000での検証状態を明確に分離した。
- `NEXT_STEPS.md` と `README.md` からチェックリストへリンクした。
- 今回はドキュメント変更のみ。ソースコード変更、ビルド、テスト、MIDI再生は実施していない。
