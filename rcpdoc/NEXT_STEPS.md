# 次の作業

更新日：2026-10-23  
対象ブランチ：`feature/rcp2-parser-skeleton`  
最新 HEAD：[27b583a](https://github.com/pikachu364/S-MU2000/commit/27b583a981b1f2f474c5b5cebc05d197c495748e)

優先順位順。資料上の仕様、コードの存在、合成テスト、CI、実ファイルでの再生を区別し、実際に確認したときだけ完了チェックを付ける。

## T-001 — CI #189 の記録を同期する

**状態:** 作業者から合格報告あり。

- [x] #189 が合格したとの報告を受領。
- [x] 最新 HEAD と CI のリンクを記録する。
- [ ] 必要に応じて Actions の全ジョブ詳細を取得し、各 job の最終結論を追記する。

[GitHub Actions #189](https://github.com/pikachu364/S-MU2000/actions/runs/39886755808)

## T-002 — RCP2 の MIDI 出力接続境界を設計する

**優先度:** 最優先  
**状態:** 調査開始。直接接続は未実装。

確認済みの構造:
- `rcp::timed_event` / `timed_seconds_event` は RCP2 のイベント・時刻変換に使われる。
- MIDI message encoder と track port/channel mapping は存在する。
- `smf::event` は秒・MIDI byte 列・port を持つ。
- `src/ui/player.cpp` は現在 `smf::load` / `smf::load_from_memory` を使う。RCP2 event の直接再生経路は未確認。

次に行うこと:
- [ ] player のイベント順序、port mapping、tempo/seek/chase の扱いを読む。
- [ ] RCP2 の tempo metadata と秒単位 event の変換契約を照合する。
- [ ] 直接 `smf::event` 相当へ変換する方式と、SMF serialization を挟む方式を比較する。
- [ ] 未対応 event がある場合に部分出力で成功扱いにならないエラー設計にする。

**完了条件:** 既存の再生経路を壊さずに接続する方式が決まり、API とテスト方針が明文化される。

## T-003 — 最小の内部 event adapter と回帰テスト

**優先度:** 高  
**状態:** T-002 の設計後に実装。

- [ ] Note On/Off、CC、Program Change、Pitch Bend の既存 encoder を使って MIDI byte 列を生成する。
- [ ] RCP2 track header の channel/port mapping を adapter に適用する。
- [ ] tick / 秒の単調性、同 tick の安定順序、dummy track 除外をテストする。
- [ ] tempo metadata を player が必要とする形式へ変換する。段階的 E7 tempo curve は未対応として明示する。
- [ ] 不正チャンネル・ポート・イベント値、未対応 command の拒否を検証する。
- [ ] `src/verify.cpp` と `tests/verify.txt` を更新し、CI の全ジョブが完了するまで合格扱いにしない。

## T-004 — RCP2 仕様チェックリストの同期

- [ ] `CHECKLIST.md` でヘッダー、transpose、delay、mute、note gate、EB/EC/EE、tempo、port/channel mapping をコード・テスト別に追跡する。
- [ ] `FACTS.md` に根拠となるコードとテストの参照を記録する。
- [ ] `HYPOTHESES.md` に未解決の 0x98、0xFC loop、E7 gradual tempo、特殊 EC、実ファイル再生を残す。
- [ ] 古い「CI 未確認」記述は、歴史的記録として残す箇所を除き、現在状態と矛盾しないよう更新する。

## T-005 — 実 RCP2 サンプルとの比較・再生検証

**状態:** 未実施。

- [ ] 使用許可のあるサンプル、または自作の小さな RCP2 fixture を用意する。
- [ ] 参照コンバーターとの比較条件、入力、出力、差分を記録する。
- [ ] 実際に再生した場合は、使用機器・出力経路・観測結果を記録する。
- [ ] CI 合格や合成データテストを実曲の再生互換性の証明として扱わない。

`/r/roms` はアップロードしない。

## T-006 — ローカル作業環境との差分確認

**状態:** 未確認。

- [ ] PC が使えるとき、ローカルのブランチ名・作業ツリー・未追跡ファイルを確認する。
- [ ] 未コミット変更を保護し、GitHub の最新 HEAD とファイルごとの差分を確認する。
- [ ] 必要な変更だけを選択して統合する。上書き・reset・force push はしない。

## 継続作業の制約

- `main` は直接変更しない。
- `feature/rcp2-playback` の未コミット作業を上書き・reset しない。
- JMPlayer / ValleyBell は仕様比較の参考とし、コードをコピーしない。
- CM6/GSD と RCP3 は初期スコープ外。
- 実行していないテストは成功と記録しない。
