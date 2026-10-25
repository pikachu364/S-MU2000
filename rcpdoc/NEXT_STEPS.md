# 次の作業

更新日：2026-10-25  
対象ブランチ：`feature/rcp2-parser-skeleton`  
直近の CI：Actions #431 合格（ユーザー報告）。private sample-6 レポートで12曲すべてが構造解析、全体 timeline、seconds 変換、MIDI event encoding に成功。これは診断パイプラインの成功であり、参照コンバーターとの出力一致や実曲再生の成功を意味しない。

優先順位順。資料上の仕様、コードの存在、合成テスト、CI、実ファイルでの再生を区別し、実際に確認したときだけ完了チェックを付ける。

## T-001 — MinGW/MSYS2 CI の安定化

**状態:** 当面の回避策を確認済み。根本原因の調査は保留。

- [x] `.github/workflows/build.yml` の Windows MSYS2 セットアップを `update: false` に変更。
- [x] [GitHub Actions #225](https://github.com/pikachu364/S-MU2000/actions/runs/40017822466) の全7ジョブが成功したことを確認。
- [ ] MSYS2 の推奨環境を維持したまま、更新時に発生した `__mingw_fe_dfl_env` のリンクエラーを将来調査する。

注意: 現設定は MSYS2 を外すものではなく、CI の全面更新を避ける回避策。根本原因が解決したことを意味しない。

## T-002 — RCP2 から既存プレイヤーへのイベント接続

**状態:** 実装あり。CI #225 成功。実ファイル再生は未確認。

- [x] `src/ui/player.cpp` のメモリ読み込み経路で RCP2 を識別し、SMF 読み込みと分岐する。
- [x] RCP2 を中間表現へ解析し、対応イベントを展開、tick→秒変換、track port/channel 割当、MIDI byte 列へ変換する。
- [x] `smf::event`（秒・MIDI byte 列・port）へ詰め替え、既存プレイヤーのイベント経路へ渡す。
- [x] 合成データで解析→展開→秒変換→割当→MIDI 変換の経路を検査する。
- [x] `player::add_from_memory` からの正常な RCP2 読み込みと、未対応コマンド時に部分追加しないことを API レベルの合成テストで検証。CI #274 成功。
- [ ] 実際の RCP2 ファイルでの API 読み込みとエラー伝播を確認する。
- [ ] 実際の RCP2 ファイルで比較・再生検証する。

制約: CI と合成パイプラインの成功は、すべての RCP2 コマンドへの対応や実曲の互換性を保証しない。

## T-003 — 対応コマンドと未対応コマンドの境界を明確化

**優先度:** 高

現在の文書レベル展開は、通常ノート、F5 キー署名、E7 テンポ、EA/EB/EC/ED/EE の対応サブセット、D0/D2/D3、E1/E2、DD/DE/DF、E6 などを扱う。ただし段階的テンポ変化、特殊 EC、ユーザー SysEx 群（0x90–0x98）などは未対応で、RCP2 全体への対応を意味しない。

- [x] `rcpdoc/CHECKLIST.md` を現コード・テストと照合し、対応済みと未対応をコード／合成テスト別に記録した（2026-10-24）。
- [x] 未対応コマンドが混在する入力を、部分的なイベント出力のまま成功扱いにしないことをテストした。CI #232 成功。
- [x] F8/F9/FC ループの参照資料・挙動を再確認し、安全条件と合成テスト計画を `rcpdoc/LOOP_EXPANSION_DESIGN.md` に記録した（2026-10-24）。
- [x] F8/F9 の有限反復（1〜126）とネストループをレコード列として展開し、単純ループ・ネスト・不対ペア・未確定カウント・1回反復境界の合成テストを追加した。CI #262 成功。
- [ ] F8 step=0 と 0x7f 以上の仕様/互換性を引き続き調査する。現在は安全のため拒否。
- [x] FC の参照範囲・端点・連鎖・循環について正常/不正の合成テストを追加。CI #256 成功。
- [x] FC展開後にF8/F9が続く連携テストを追加。CI #260 成功。
- [ ] FC の参照展開に対し、展開予算超過・深い連鎖などの境界テストを追加し、出力が安全に破棄されることを確認する。F8/F9側の100万レコード予算テスト（#275）は成功済みだが、FC固有のテストとは区別する。
- [ ] FCの端点規則を実RCP2データや独立した参照出力と照合し、互換性を確認する。
- [x] FC/F8/F9を含む文書レベル展開パイプラインの合成回帰テストを追加。CI #262 成功。
- [x] 既存プレイヤーAPI経由の正常読み込み・未対応コマンド拒否・失敗時の部分追加防止を独立した合成テストで確認（#274成功）。
- [ ] 実 RCP2 ファイルで API 読み込み・エラー伝播を確認する。
- [ ] 0x98、F8 の step=0、特殊 EC、E7 gradual tempo の未解決点を `HYPOTHESES.md` に維持する。
- [x] 実データ12曲のイベントコード頻度を集計。`FD` が18,324件、`DD/DE/DF` が計1,028件、`E1/E2/E6` は資料上の対応候補として優先調査が必要。多くの曲で現在未対応のコマンドが出現するため、実曲再生にはコマンド対応範囲の拡張が必要。
- [x] 参照形式資料の説明に基づき、`FD`（小節終端）を時間を進めないマーカーとして扱う変更と合成テストを追加。CI #299 は全7ジョブ成功。
- [x] `E1/E2` の参照資料上の意味を照合し、CC32 / CC0 / Program Change への展開と 7-bit 境界の合成テストを追加（2026-10-24）。新しいコードの CI は未確認。
- [x] E6 のトラック途中ルート変更を timeline event として表現し、出力時にトラック別の channel/port/enabled 状態へ反映。dummy 切替と tick 進行を合成テスト。Actions #338 合格（ユーザー報告）。
- [x] DD/DE/DF の GS DT1 SysEx 経路を実装し、合成テスト追加。Actions #320 合格（ユーザー報告）。
- [x] 0x90–0x97 のヘッダー定義ユーザー SysEx を実装。ヘッダーの 0x406 から 8 スロット（各 0x30 byte）を読み、名前と 0x18 byte テンプレートを抽出。80/81/82/83/84 の置換、チェックサム、F0/F7、E6後のチャンネル・dummy抑止を合成テストへ追加。今回の CI は未確認。
- [x] 0x98 の F7 継続レコード結合、置換、空 payload、ループ境界を合成テストで扱う。これは固定4-byteレコードを意味的に結合する方式であり、任意の可変長バイト列 parser を実装したという意味ではない。
- [x] Glosa の `RcpConverter.cs` を確認。`TrackExclusive (0x98)` / `ExternalCommand (0x99)` / `Comment (0xF6)` は後続 `F7` レコードを集約してループ展開し、末尾の `F7` を除去する。`ExternalCommand` は MIDI 出力せず、時間進行のみ残す。ValleyBell と QB64 Recomposer は 0x99 の変換処理をコメントアウトしており、Glosa が直接の根拠。
- [x] プライベートの実 RCP2 サンプル12曲を独立スキャン。トラック内レコードのコマンド位置に `0x99` は検出されなかった（ヘッダーなどにある生バイト出現はコマンドとは扱わない）。payload の実例・用途は実曲から確認できていない。
- [x] CI の `verify --rcp2-sample` により現行 C++ `parse_tracks` / `build_intermediate` で12曲の構造解析結果を記録。全曲 `OK; parsed_tracks=18; intermediate_tracks=18`。ただし timeline 展開・MIDI 出力は未検証。
- [x] `verify --rcp2-sample` に timeline 展開・秒変換・MIDI イベント符号化の診断出力を追加し、sample-6 レポートで12曲すべての成功とイベント件数を確認した（Actions #431 合格報告後）。

## T-004 — 実 RCP2 サンプルとの比較・再生検証

**状態:** 12曲の C++ 診断パイプラインは成功。参照出力比較・実再生は未実施。

- [x] 使用する実 RCP2 サンプルをプライベート資料リポジトリ `pikachu364/S-MU2000-private-data/RCP_DATA` に用意。12個の `.RCP` と曲ごとの `.DOC` 説明書を確認した（公開コードリポジトリには曲データをコピーしない）。
- [x] 12ファイルのバイナリ構造を調査。すべて RCP2 識別文字列を持ち、18個のトラック領域が並ぶ。多くの旧形式ファイルではヘッダーのトラック数 byte `0x1E6` が 0 のため、参照実装の挙動に合わせて 18 トラックと解釈する必要がある。
- [x] 現行 C++ コードで12曲すべての `parse_tracks` / `build_intermediate` を実行し、全曲 `parsed_tracks=18` / `intermediate_tracks=18` を確認。sample-6 では全体 timeline / seconds / MIDI encoding も12曲すべて成功した。
- [x] サンプルのトラック列を独立スクリプトで追跡し、12ファイルすべて18トラック境界とFE終端を確認。FC展開は全トラックで参照先検査を通過。F8/F9展開は MIND track 11 と WINGS_55 track 5 で F9/F8 対応不一致を検出。参照仕様と実データを照合して原因を調べる。
- [ ] 説明書と照合し、参照コンバーターとの比較条件、入力、出力、差分を記録する。プライベート曲データは公開リポジトリへコピーしない。
- [ ] 実際に再生した場合は、使用機器・出力経路・観測結果を記録する。
- [ ] CI 合格や合成データテストを実曲の再生互換性の証明として扱わない。

`/r/roms` はアップロードしない。

## T-005 — E5 Key Scan の時間展開対応

**状態:** CI と private sample-check で確認済み。参照出力との比較は未実施。

- [x] `src/rcp.cpp` に E5 の処理を追加。MIDI イベントを生成せず p0 tick を進める。
- [x] `src/verify.cpp` に合成回帰テストを追加し、`tests/verify.txt` の期待出力を更新。
- [x] Actions #431 合格（ユーザー報告）。
- [x] sample-6 で CHISA140.RCP の timeline / seconds / MIDI encoding がすべて OK になったことを確認。以前の sample-5 の timeline 失敗は解消。
- [ ] 参照コンバーターとのイベント出力差分を比較する。

## T-006 — 実サンプルの参照出力比較

**状態:** 次の優先作業。

- [ ] プライベートの実 RCP2 サンプルを公開リポジトリへコピーせず、ローカルまたはプライベート環境で参照コンバーターと現行実装を実行する比較手順を整える。
- [ ] 代表曲を1曲選び、両方の MIDI 出力を同じ条件で生成する。
- [ ] MIDI ファイルのトラック数、イベント種別、tick/時間、SysEx、ルーティング差分を集計する。
- [ ] 差分を仕様・参照実装に照らして分類し、根拠が確認できた差だけを修正する。
- [ ] 実際に再生していない限り、聴感上の互換性は未確認として記録する。

## 継続作業の制約

- `main` は直接変更しない。
- `feature/rcp2-playback` の未コミット作業を上書き・reset しない。
- JMPlayer / ValleyBell は仕様比較の参考とし、コードをコピーしない。
- CM6/GSD と RCP3 は初期スコープ外。
- 実行していないテストは成功と記録しない。


## 2026-10-24 追記 — 最新確認

- 直近の既確認コード HEAD: [077da3b](https://github.com/pikachu364/S-MU2000/commit/077da3bab92e817ea7d1adba07319258aeb2f9cf)。旧RCP2のトラック数0フォールバック修正と合成テストを追加中。Actions #287 はキュー待ちで、まだ成功判定しない。
- [Actions #262](https://github.com/pikachu364/S-MU2000/actions/runs/40064492056) は Linux、Windows、Windows 32-bit、MSVC Win32/x64、README、macOS の全7ジョブ成功。
- F8/F9 の1回反復境界と、FC展開後のF8/F9連携テストを追加済み。合成テストの成功であり、実曲再生の検証ではない。

## 2026-10-24 追記 — ループ展開予算と player API

- [Actions #274](https://github.com/pikachu364/S-MU2000/actions/runs/40069003704) は全7ジョブ成功。Windows verify の `timeBeginPeriod` / `timeEndPeriod` 未解決シンボルは `-lwinmm` 追加で修正し、`L'\\0'` の誤った文字リテラルも `L'\0'` に修正した。
- `player::add_from_memory` の合成 API テストを追加し、正常データの追加、未対応コマンドの拒否、失敗時に部分的な曲を追加しないことを確認した。
- [Actions #275](https://github.com/pikachu364/S-MU2000/actions/runs/40072000688) はユーザー報告により合格確認。3重の126回ループで100万レコードの展開予算を超過させ、失敗・出力クリア・エラー設定を確認するテストを追加した。
- 次は FC 展開予算の境界テストと、実ファイルの比較・再生検証へ向けた fixture/参照出力の整備を優先する。実 RCP2 楽曲の再生互換性は未確認。


## 2026-10-24 追記 — CI #299 と MIDI コマンド別名

- ユーザー報告により Actions #299 の合格を確認。実曲の C++ パーサー実行・再生確認とは区別する。
- 参照資料のコマンド表にある `D0` / `D2` / `D3` を、既存の MIDI チャンネルイベント経路へ追加。D0=Pitch Bend、D2=Control Change、D3=Program Change として扱い、時間展開・範囲検査の合成テストを追加した。
- この変更の CI は未確認。次に Actions の全ジョブを確認する。


## 2026-10-24 追記 — Actions #307 と E1/E2 実装

- ユーザー報告により Actions #307 の合格を確認。これは E1/E2 実装前の確認結果であり、直近の E1/E2 コミットを含む CI は未確認。
- 参照資料に基づき `E1` を CC32=p2 → Program Change=p1、`E2` を CC0=p2 → Program Change=p1 の順で同一 tick に展開する処理を追加。step による時間進行は各 RCP レコードにつき一度だけ。
- `src/verify.cpp` にイベント順序・tick・7-bit 範囲外拒否の合成テストを追加し、`tests/verify.txt` の期待出力を同期した。
- 実曲12ファイルでの C++ パーサー実行、比較、再生互換性は未確認。関連コミット: [E1/E2 実装](https://github.com/pikachu364/S-MU2000/commit/7788e6fa63e7623843595cdc078d528ce54e2e56)、[合成テスト](https://github.com/pikachu364/S-MU2000/commit/aa74cf217de259f50dc339bd40bc37f2d90be43c)、[期待出力](https://github.com/pikachu364/S-MU2000/commit/e5478490ddf1d286e2016e2d447c078642ee3508)。


## 2026-10-24 追記 — #320 合格と GS パラメータ SysEx

- ユーザー報告により Actions #320 の合格を確認。E1/E2 の改訂を含む当該時点の CI 成功として記録する。
- `DD` は Roland GS アドレス上位/下位、`DF` はデバイス ID / モデル ID を状態として保持し、`DE` は `F0 41 device model 12 address data checksum F7` の DT1 SysEx を生成するようにした。チェックサムはアドレス2 byte とデータ2 byte の 7-bit 和から算出する。
- 合成テストで状態保持、チェックサム、tick 進行、7-bit 範囲外拒否を検査。`tests/verify.txt` を同期した。
- 追加変更の CI は未確認。次は E6 の動的チャンネル/ポート切替を調査・実装する。実 RCP2 ファイルの C++ パーサー実行・再生互換性は未確認。


## 2026-10-24 追記 — Actions #328 合格と E6 動的ルーティング

- ユーザー報告により Actions #328 の合格を確認。DD/DE/DF GS SysEx 対応までの CI 成功として記録する。
- E6 を時間イベント `route_change` として展開し、指定値を 1 減らした後にポート（上位側）とチャンネル（下位 4 bit）へ分離する。ダミー指定では以後の MIDI イベントを抑止する。
- MIDI 出力ではトラックごとの現在ルートを保持し、E6 以後のイベントに動的に反映する。ルート変更、dummy 切替、tick 進行を合成テストし、期待出力を同期した。
- E6 追加変更の CI は未確認。次は参照実装と仕様を照合し、E5 / その他の未対応コマンドの扱いと実 RCP2 ファイルでの互換性を整理する。


## 2026-10-25 追記 — 0x98 Track SysEx

- Glosa の `RcpConverter.cs` では `0x98` の後に連続する `0xF7` レコードの p1/p2 をデータとして結合し、末尾の F7 をパディングとして除去してから SysEx 化している。
- S-MU2000 でも FC/F8/F9 の展開後に `0x98` の後続 `0xF7` を消費する実装を追加。テンプレート置換 0x80〜0x84、F0/F7、tick 進行、E6 dummy route 抑止を扱う。
- 合成テストは複数 F7 の結合、置換、Roland checksum、次イベント tick、孤立 F7 拒否を対象とする。テストは追加済みだが、実行結果と CI は未確認。
- 注意: 現在の parser は物理的な 4-byte レコード列を保持する。これは可変長の生バイト列を直接走査する方式ではなく、RCP2 の F7 継続レコードを意味的にまとめる方式。
- 次は Gearmulator PR #303 と Glosa の挙動を細部比較し、ループ境界内外の F7 継続、空 payload、F7 終端とパディングの違いを追加検証する。
- 参照: https://github.com/hsshss/glosa/blob/main/src/Glosa.Core/Rcp/RcpConverter.cs と https://github.com/dsp56300/gearmulator/pull/303


## 2026-10-25 — #358 合格後の Track SysEx ループ境界テスト

- ユーザー報告により Actions #358 の合格を確認。今回の追加テスト後の CI 成功とは区別する。
- `0x98` と連続 `F7` 継続レコードが F8/F9 展開でまとまった単位として反復されることを合成テストに追加。反復ごとの SysEx tick とバイト列を検査する。
- 継続レコードに置換コード以外の不正な high-bit 値がある場合、部分タイムラインを残さず拒否することも検査する。
- このテスト追加後の CI は未確認。次は参照実装の 0x98 終端/パディング規則と、ループ境界をまたぐ継続レコードの解釈を比較する。


## 2026-10-25 — #364 合格後の空 Track SysEx ケース

- ユーザー報告により Actions #364 の合格を確認。今回の追加テスト後の CI とは区別する。
- Glosa の展開処理では 0x98 / 外部コマンド / コメントの後続 F7 レコードを集約し、末尾の F7 をすべて除いてから resolved event 化する。TrackExclusive の MIDI 出力は集約データが空なら SysEx を出さない。
- これに合わせ、S-MU2000 で F7 パディングのみの 0x98 は空 SysEx を生成しない一方、0x98 の p0 による tick 加算は保持されることを合成テストに追加。
- 追加変更後の CI は未確認。引き続き、0x98 のループ境界と実 RCP2 ファイルの C++ 実行を検証する。


## 2026-10-25 — #370 失敗後のテスト期待値修正

- ユーザー報告の Actions #370 で `RCP2 98 空 payload: SysEx省略/tick維持` が NG。原因はテストが次の音符の開始 tick を `5` としていたこと。
- レコードの p0 はそのイベントの開始時刻ではなく、処理後に次イベントへ進む時間。0x98 の p0=2 の後の音符開始 tick は 2 が正しいため、`src/verify.cpp` の期待値を `5` から `2` に修正。
- 修正コミット後の CI は未確認。実 RCP2 ファイルの C++ 実行・再生も未確認。


## 2026-10-25 — #374 合格後の 0x98 ループ境界修正

- ユーザー報告により Actions #374 の合格を確認。これは空 payload の期待 tick 修正までの成功として記録する。
- Glosa の `Unroll` は TrackExclusive / ExternalCommand / Comment の直後に続く F7 レコードを、その場で同じイベントのデータとして集約してからループを展開する。ループ終端 F8 の外にある F7 はループ内の最後の 0x98 に接続されず、孤立継続としてスキップされる。
- S-MU2000 の F8/F9 展開でも 0x98 と F6 の後続 F7 を一体としてコピーするようにし、孤立 F7 をスキップする挙動に合わせた。ループ外 F7 が最後の反復に誤接続しない回帰テストを追加。
- 今回の修正後 CI は未確認。0x99 外部コマンドの継続データは未対応のまま。実 RCP2 データでの C++ 実行・再生も未確認。
- 参照: https://github.com/hsshss/glosa/blob/main/src/Glosa.Core/Rcp/RcpConverter.cs


## 2026-10-25 追記 — Actions #384 合格と FC 展開予算テスト

- ユーザー報告により Actions #384 の合格を確認。これは孤立 F7 テストの変数名参照修正までの成功であり、今回追加した FC 展開予算テストの CI 結果ではない。
- `src/verify.cpp` に FC の走査予算超過テストを追加し、100万件超の入力で文書レベル展開が失敗し、部分タイムラインを残さず、エラーを返すことを検査する。`tests/verify.txt` の期待出力も同期した。
- 追加コミット: [366a0aec](https://github.com/pikachu364/S-MU2000/commit/366a0aec278258d54b10afa6c5c4e067f1411b76)、[baseline 更新](https://github.com/pikachu364/S-MU2000/commit/b79d7b8ec4fe302dc0333e8af90eeaf0969267aa)。この変更後の CI は未確認。
- 次は `0x99` 外部コマンドの継続レコード規則を参照実装と照合する。実 RCP2 12曲の C++ 実行・実曲再生は未確認。


## 2026-10-25 追記 — Actions #406 と実サンプル診断レポート

- ユーザー報告により Actions #406 合格を確認。添付の `rcp2-sample-report.zip` 内のレポートでは、12曲すべてが `OK; parsed_tracks=18; intermediate_tracks=18`。
- F6/F7 診断は計3,245件の連鎖記録を出力。連鎖の長さが10レコードのケースが多数あるが、これは診断結果であり、コメント本文の復元や意味的な正しさまで検証したものではない。
- 孤立 F7 は1件: `CHISA140.RCP` の track[0]、record 994、p0=00 / p1=41 / p2=10。周辺イベントと参照実装の挙動を確認するまで、異常データと断定しない。
- F8/F9 数の不一致を示す実例が少なくとも2件ある: DINA の track[10] は F9=1 / F8=0（record 35 の F9 p0=00）、WINGS_55 の track[4] は F8=44 / F9=45。これらを直ちに不正と扱わず、ループ境界と参照実装に照らして調査する。
- このレポートは parser の構造的な読込成功を示す。実曲の MIDI 出力比較・再生互換性は未確認。
- 次の優先作業: 更新した診断レポートを再取得し、孤立 F7 の前後レコードと unmatched F8/F9 を確認する。仕様根拠が得られるまでパーサー意味論を変更しない。


## 2026-10-25 追記 — 診断の精度向上（CI 未確認）

- `src/verify.cpp` の実データ診断だけを拡張。F8/F9 の数が不一致なトラックでは、単純なスタック照合により unmatched F8 / unmatched F9 のレコード位置を追加出力する。
- F7 継続レコードの所有候補を F6 に限定せず、Glosa の挙動調査に基づく 0x98 / 0x99 も診断対象に含めた。直前が F6/F7/0x98/0x99 の場合は、F7 を孤立扱いしない。
- 孤立 F7 について前後最大3レコードの raw code/p0/p1/p2 を追加出力し、周辺コンテキストを見て判定できるようにした。
- これはサンプルレポート用診断の変更であり、RCP の受理条件やイベント展開の意味論は変更していない。CI は未確認。


## 2026-10-25 — sample-4 F8/F9 不一致の再照合

- private repository の `sample-4/rcp2-sample-report.txt` を直接読み、F8/F9 数が不一致の実例をサンプル名・トラック番号まで再確認した。
- **MIND.RCP track[10]**（`TR808 Set`）は F9=1 / F8=0。record 35 に `F9 00 00 00` がある。
- **WINGS_55.RCP track[4]**（`リード・ギター＆フェード・アウト`）は F8=44 / F9=45。単純な LIFO 照合では record 2184 の F9 が未対応として残る。
- 訂正: 以前のメモで「DINA track[10]」と書いたのは誤り。DINA の track[10] は `MELO1` で F8/F9 は 0/0。サンプル名はレポートの実際のブロック境界に基づいて記録する。
- 参照コンバーター `rcp2mid.c` では F9 をループ開始としてスタックへ積み、F8 で対応フレームを処理する。また、別途トラック全体のループ拡張 (`loopOfs`) を扱うコードがある。これだけでは、実データの未対応 F9 が意図的なループ点か不正なネストかは確定できない。
- 現在の `src/rcp.cpp` は、有限 F8/F9 ペアを事前に対応付け、対応する F8 のない F9 をトラック全体ループ候補として消費せずに通過させる。ネストした有限ループが後続の F8 と誤対応しないようにする。unmatched F8 は引き続き拒否する。
- `src/verify.cpp` に unmatched F9 のトラックループマーカー受理と、未閉鎖の外側 F9 の内側に有限ネストループがあるケースを追加。Actions #416 合格（ユーザー報告）。これは合成テストと CI の結果であり、MIND/WINGS 実曲の timeline 展開・MIDI 出力が成功した証拠ではない。
- [ ] MIND record 35 と WINGS record 2122–2210 の該当区間について、参照コンバーターの事前走査・実際の再生時処理を引き続き照合し、実データでの互換性を確認する。
- レポートは `parse_tracks` / `build_intermediate` の成功を示すが、これらのトラックでの timeline 展開・MIDI 出力成功を示すものではない。


## T-004 — 実 RCP2 サンプルのトラック別パイプライン診断

**状態:** 診断コード追加済み。Actions #425 合格を確認。sample-5 の12曲レポートを確認し、CHISA140 の E5 Key Scan 未対応を特定。E5 対応コミット後の CI と再検証は未確認。

- [x] 全体の timeline → seconds → MIDI 診断を追加。
- [x] 各トラック単独で timeline 展開、秒変換、MIDI マッピング、MIDI 符号化を診断する出力を追加（今回）。全体診断で一つのトラックが失敗しても、残りのトラックの状態を観測できる。
- [x] トラック別診断追加変更の CI を確認する（Actions #425 合格）。
- [x] private sample-check の12曲レポートを回収し、全体 timeline 失敗が CHISA140 のみであることを確認する。
- [x] 失敗トラックのエラーと raw command histogram を突き合わせ、CHISA140 track[0] の E5=1 を特定する。
- [ ] E5 Key Scan 対応後の CI を確認する。
- [ ] private sample-check を再実行し、CHISA140 の timeline / MIDI 変換が成功するか確認する。
- [ ] 実際の MIDI 出力を参照コンバーターと比較し、互換性を確認する。

注意: トラック単独の成功は、複数トラックの統合、テンポ同期、ポート割当、実曲再生の正しさを保証しない。


## T-005 — E5 Key Scan の時間展開対応

- sample-5 レポートでは CHISA140.RCP の track[0] だけ timeline 展開に失敗し、histogram に E5 が1件あった。
- 参照実装は E5 を Key Scan として MIDI イベントなしで扱うため、S-MU2000 も MIDI イベントを出さず p0 tick を進めるようにした。
- [x] `src/rcp.cpp` に E5 の処理を追加。
- [x] `src/verify.cpp` に合成回帰テストを追加。
- [x] `tests/verify.txt` の期待出力を更新。
- [ ] Actions CI を確認。
- [ ] private sample-check を再実行して CHISA140 の失敗が解消したか確認。
- [ ] 実際の MIDI 出力と参照コンバーターの結果を比較する。

参照: https://github.com/ValleyBell/MidiConverters/blob/master/rcp2mid.c


## T-007 — E7 p1/p2 実サンプル分布と gradual tempo の参照比較

**状態:** sample-7 を確認。E7 の参照実装調査で gradual tempo の未対応警告を確認。実装変更は未着手。

- [x] `src/verify.cpp` の `--rcp2-sample` に E7 の p1/p2 組合せ別件数を表示する診断を追加。
- [x] ユーザー報告により Actions #437 合格を確認。これは E7 分布診断を含むコードの CI 成功。
- [x] private の [sample-7 レポート](https://github.com/pikachu364/S-MU2000-private-data/blob/main/Sample%20report%20files/rcp2-sample-report/sample-7/rcp2-sample-report.txt) を確認。12曲すべて構造解析成功、全体 timeline / seconds / MIDI event encoding が OK。各 track_pipeline に FAIL は見当たらない。
- [x] 12曲の E7 分布を確認。E7 は 8 曲に出現。p2=00 が大半で、非ゼロ p2 は CHISA140.RCP の 4件（p1/p2: 28/30、32/30、3A/30、3C/30）のみ。
- [x] ValleyBell/MidiConverters の `rcp2mid.c` の `case 0xE7` を確認。p2 != 0 の場合は “Gradual Tempo Change” の警告を出すが、警告後も p1 を使って即時の MIDI tempo event を書き出す。コメント上の speed は `0x40` 固定。段階的なテンポ変化そのものはこの参照コードでは実装されていない。
- [ ] p2=0x30 の実サンプル文脈と RCP の仕様資料を追加調査し、p2 の値・単位・更新間隔を推測なしに特定する。
- [ ] 仕様根拠が得られるまでは、現行の `gradual` フィールド保持を維持し、変換動作を変更しない。
- [ ] 必要な仕様が確定したら、p2=0 と非ゼロ双方の合成テストを追加してから実装する。

補足: sample-7 のレポートではコマンド histogram 終端がリテラル `\\n` になっていたため、`src/verify.cpp` で改行出力も修正した（commit [abb3c08](https://github.com/pikachu364/S-MU2000/commit/abb3c080644248f7d344657ddce3a9dec5f0c811)）。この修正後の CI / レポート再生成は未確認。


## T-007 follow-up — gradual E7 semantics

- Actions #439, #441, and #442 passed (user-reported). #441/#442 are recorded as validation reports after the diagnostic/documentation follow-ups.
- ValleyBell/MidiConverters `RCPFormat.txt` defines E7 p1 as a tempo multiplier (`0x40` = 100%) and p2 as interpolation duration in ticks: p2=0 sets tempo immediately; p2=1..FF interpolates over that many ticks.
- `rcp2mid.c` and QB64-Phoenix-Edition's Recomposer implementation both warn about nonzero p2 and currently ignore the gradual interpolation, setting the target tempo immediately. This corroborates the gap but does not establish the exact interpolation curve used by the original RCP player.
- `src/verify.cpp` now prints a separate `E7 gradual tempo` count and explicitly says interpolation is not implemented, so a successful seconds conversion is not mistaken for complete gradual-tempo support. Commit: 9fd7296.
- A follow-up public-source search found the same warning/ignore behavior in ValleyBell/MidiConverters `rcp2mid.c`, QB64-Phoenix-Edition's `Recomposer/RCP.cpp`, and Stuerp's `libmidi/src/RCP/RCP.cpp`. The latter two implementations may share code lineage, so they should not be counted as fully independent confirmation. No source found so far establishes the original player's interpolation curve or update rule.
- Next: seek primary evidence such as original player documentation/source or a controlled playback comparison for an explicit curve/update rule. Do not infer semantics merely from multiple converters that ignore the feature. Until evidence is found, retain the raw `gradual` value and do not invent a curve. Once evidence is sufficient, test p2=0, p2=1, p2=0x30, ramp completion, and a new E7 arriving during an active ramp. Actions #442 passed (user-reported).
