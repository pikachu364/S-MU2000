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
