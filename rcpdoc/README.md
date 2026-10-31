# RCP 解析・実装ノート

このディレクトリは、S-MU2000 の RCP2 対応について、ChatGPT の会話履歴に頼らず、別の会話・別の担当者でも作業を再開できるようにするための引き継ぎ用ドキュメントです。

## 最初に読む順番

1. [NEXT_STEPS.md](NEXT_STEPS.md) — 現在の優先タスクと完了条件
2. [CHECKLIST.md](CHECKLIST.md) — JMPlayer資料に基づく項目別の実装・テスト追跡
3. [FACTS.md](FACTS.md) — 根拠が確認できている事実と、その根拠
4. [HYPOTHESES.md](HYPOTHESES.md) — 未確認事項・仮説・検証方法
5. [LOG.md](LOG.md) — 日付順の作業履歴
6. [参考資料：JMPlayer RCP ノート](references/jmplayer-rcp-notes.md) — 外部資料の要点と出典
7. [過去レポート](reports/README.md) — 既存レポートと当時の状況

## 現在の対象

- リポジトリ: `pikachu364/S-MU2000`
- 作業ブランチ: `feature/rcp2-parser-skeleton`
- 最初の対象: RCP2（`.rcp`）
- 将来の方針: RCP3 に拡張できるよう、ファイル解析と MIDI イベントへの展開を分離する
- 初期対象外: CM6/GSD
- ローカルでのビルド・実行テストはパソコン環境が使えるときに行う

## 記録ルール

- **確認済み事実**と**仮説**を同じものとして扱わない。
- 各項目に根拠、確認方法、制約を記録する。
- ソースコード確認、合成データによるテスト、実際の再生確認を区別する。
- 実行していないテストを成功扱いにしない。
- 参考資料のチェックマークは、S-MU2000で検証済みという意味ではない。
- 失敗した試みや訂正も履歴に残し、古い記録を黙って書き換えない。
- ローカル環境に未コミット変更がある可能性を前提とし、上書き・リセット・削除をしない。
- `main` は直接変更せず、この作業ブランチで作業する。
- `/r/roms` はアップロードしない。テストは自作の合成データを使う。
- コードや資料の更新後は、関連するこのディレクトリの記録も必要に応じて更新する。

## 現在の状況（2026-10-29 更新）

- 公開リポジトリ Actions #556 はユーザー報告で合格。最新のコード変更・CI状態は [NEXT_STEPS.md](NEXT_STEPS.md) と [LOG.md](LOG.md) を参照。
- private sample-check run [40868277263](https://github.com/pikachu364/S-MU2000-private-data/actions/runs/40868277263) の成果物を確認。ValleyBell参照との比較で sample-01 BUTTFLY、sample-03 WINGS_55、sample-04 L_WING88 は完全一致。sample-02 FC は総イベント数・トラック別件数が一致し、同一tick内のイベント順序差が4件残る。
- 4サンプルのイベント総数はすべて一致。合計94,554イベント中、比較上の不一致は4件。これは参照コンバーターとの比較であり、実機再生を確認したという意味ではない。
- 固定 Note Off 優先順位を外してソース timeline の安定順を維持する変更を反映。sample-01〜04の差分は前回の627 / 16 / 598 / 3件から、今回0 / 4 / 0 / 0件に減少した。
- 次の重点は sample-02 Track 6 / tick 5135, 5263 の Note Off相当イベントと CC の順序差を、元RCPレコードと ValleyBell の running-note 期限処理に対応付けること。RCP2全体の互換性や実曲再生は未確定。

## 次の作業

1. [NEXT_STEPS.md](NEXT_STEPS.md) に従い、FC展開予算の境界テストと、FC参照の端点規則を継続検証する。
2. 合成データによるテストを追加し、イベント時刻・チャンネル・ポート・同 tick の順序・ループ展開予算・エラー処理を確認する。
3. `CHECKLIST.md` と `FACTS.md` をコード確認・テスト・CIの根拠に合わせて更新する。
4. プライベート資料リポジトリの12曲を使い、修正後の parser の実ファイル通過、FC/F8/F9 展開、説明書・参照コンバーターとの比較へ進む。CI合格だけで実再生を検証済みと表現しない。

この資料は途中経過の記録です。RCP2 全体の互換性や実装の完成を保証するものではありません。


## 追加状況（2026-10-30）— SysEx 0xFF

- ValleyBell の RCP2 形式資料では、`0x98` SysEx テンプレートの既知制御値は `0x80`–`0x84`。今回の `0xFF` を別値へ置換する規則は確認できていない。
- Gearmulator の現行処理は `0x98` のデータを後続 `0xF7` 継続レコードから組み立て、未知の高ビット値をスキップする。S-MU2000 と ValleyBell も同様のスキップ動作を行う。
- 元データでの意図、スキップ後のチェックサムの妥当性、実機での動作は未確定。根拠が見つかるまでは置換やチェックサム自動修正をしない。
- 詳細は [FACTS.md](FACTS.md)、[HYPOTHESES.md](HYPOTHESES.md)、[NEXT_STEPS.md](NEXT_STEPS.md)、[LOG.md](LOG.md) を参照。


## DT1 Check Sum Error 調査（2026-10-30）

保存済みsample-01〜04のevent dumpからRoland DT1を503件抽出。500件はchecksum合格、3件はchecksum byteを持たない短いメッセージ。checksum不一致は0件。今回確認した4 sampleのevent dumpにはGearmulatorのCheck Sum Error通知条件に該当するDT1は見つかっていない。これは静的検査であり、88emu実再生試験や過去のエラー表示の発生源特定ではない。


## Check Sum Error の継続処理に関する追加確認（2026-10-30）

- Gearmulator の SC-88 / SC-88VL / SC-88Pro 向け統合テストソースには、checksum不正DT1が反映されず、その後の正常DT1は反映されることを確認するテスト構成がある。ソース確認であり、今回テストを実行したわけではない。
- Gearmulator changelog には SC-55mkII の高密度SysEx時に、タイミング競合で同じbyteが二重に渡り Check Sum Error / DT1 Data Error が出る既知不具合の修正記録がある。SC-88系や今回のsampleにも同じ問題があるとは断定しない。
- 過去のエラー表示の発生源は未特定。静的checksum不一致、個別DT1拒否、後続イベント継続、機種固有のタイミング不具合を区別する。
- 詳細は [FACTS.md](FACTS.md)、[HYPOTHESES.md](HYPOTHESES.md)、[NEXT_STEPS.md](NEXT_STEPS.md)、[CHECKLIST.md](CHECKLIST.md)、[LOG.md](LOG.md) を参照。


## 追加状況（2026-10-30）— DT1時間密度

保存済み4 sampleのparser event dumpを集計したところ、100ms窓の最大DT1件数は sample-01=5、sample-02=4、sample-03=6、sample-04=26。sample-04には短時間にSysExが集中する区間がある。表示系アドレス0x10を含むが、display dot animationやエラー原因と確定したわけではない。Gearmulator changelogのSC-55mkII高密度SysEx不具合と照合するための調査手掛かりとして扱う。詳細は [FACTS.md](FACTS.md)、[HYPOTHESES.md](HYPOTHESES.md)、[NEXT_STEPS.md](NEXT_STEPS.md)、[LOG.md](LOG.md) を参照。


## 追加状況（2026-10-30）— sample-04高密度SysExの用途

sample-04の最大100ms窓にはDT1が26件あり、そのうちmodel ID 0x56の8件はRoland公式資料のパート受信OFFメッセージと一致した。GearmulatorのSC-88表示メッセージはmodel ID 0x45を使用するため、address 0x10だけで表示ドットデータと分類しない。Check Sum Errorとの因果は未確認。詳細は [FACTS.md](FACTS.md)、[HYPOTHESES.md](HYPOTHESES.md)、[NEXT_STEPS.md](NEXT_STEPS.md)、[LOG.md](LOG.md) を参照。


## GUIプレイヤーのRCP/R36ファイル選択（2026-10-31 更新）

- Windowsの「ファイルを足す」複数選択フィルターに .rcp / .r36 を含め、既存の .mid / .midi / .smf とプレイリスト .m3u / .m3u8 も維持した。
- Linuxのこの開発ブランチは、直近の別ブランチとファイル選択実装が異なる。src/ui/pc_window_linux.cpp ではなく、実際の src/ui/app_linux.h のMIDI選択フィルターを更新し、mid;midi;smf;rcp;r36;m3u;m3u8 とした。
- ユーザー報告: Windows実機でRCP/R36が選択できることを確認。Actions #664 合格。
- Linuxはユーザーが実機環境を持たないため実機未確認。今回の開発ブランチ向け変更後のCI結果も未報告。コード反映と動作確認を混同しない。
- 元の別ブランチでの変更: [Linux 57ed97d](https://github.com/pikachu364/S-MU2000/commit/57ed97d57d08c9a469824380c6f2f1e983e818dc)、[Windowsの既存プレイリスト拡張子復元 6a91df6](https://github.com/pikachu364/S-MU2000/commit/6a91df6059cb11ad2d8eac4b31f8c5e73203b611)。この開発ブランチにはコミット自体をマージせず、現在の実装構造に合わせて必要なフィルター変更だけを反映した。
