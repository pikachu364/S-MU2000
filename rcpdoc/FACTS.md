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
