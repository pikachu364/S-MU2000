# 次の作業

更新日：2026-10-29  
対象ブランチ：`feature/rcp2-parser-skeleton`  
直近 CI：Actions #567 合格（ユーザー報告）。private sample-check run [40885695400](https://github.com/pikachu364/S-MU2000-private-data/actions/runs/40885695400/artifacts/12902283152) の成果物を確認。ValleyBell 参照コンバーター（commit `a73c748960d57bd194fb64931ce07d2c1fa1bd01`）との比較は、sample-01 BUTTFLY=27,974イベント・不一致0、sample-02 FC=20,299・不一致0、sample-03 WINGS_55=23,745・不一致0、sample-04 L_WING88=22,536・不一致0。全4サンプル合計94,554イベントで、各トラックの件数を含め完全一致。以前 sample-02 Track 6 / tick 5135 と 5263 に残っていた同一tick順序差も解消。参照コンバーターとの比較結果であり、実機再生やRCP2全体の互換性を保証するものではない。

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

**状態:** 選定した4サンプルは Actions #567 の成果物で参照出力と完全一致。全RCP2データへの一般化・実機再生は未確認。次は未完了の仕様境界テスト（FC展開予算、FC端点、F8 step値）と実ファイルでのプレイヤーAPI経路を優先する。

- [x] `verify --rcp2-event-dump` でイベント単位ダンプ（index / track / absolute tick / seconds / port / MIDI bytes）を出力し、private sample-9 の BUTTFLY で確認。27,974イベントすべての index 連番、tick/秒の非減少、空 bytes なし、`count=27974 encoded_count=27974 consistency=OK` を確認。
- [ ] 参照コンバーターと現行実装の比較をプライベート環境で実行する手順を整える。現在は参照コンバーターの MIDI を同じサンプルから生成し、イベントを正規化して比較する処理が未整備。
- [ ] BUTTFLY を代表曲として、現行実装と参照コンバーターの MIDI を同じ条件で生成する。参照コンバーターは段階的テンポ変化など未実装の意味論があり得るため、比較結果をそのまま正解とみなさず、該当コマンドの対応範囲を併記する。
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


## T-008 — 決定的な MIDI イベント診断ダンプ

**状態:** イベントダンプ実装済み。Actions #458 合格（ユーザー報告）。実サンプルのダンプと参照コンバーター比較は未実施。

- [x] `verify --rcp2-event-dump <file.rcp>` を追加。従来の `--rcp2-sample` は通常どおり要約診断だけを出し、既存の期待出力を変えない。
- [x] ダンプは変換成功時に限り、イベントごとに連番 index、track index、絶対 tick、秒、MIDI port、MIDI bytes を16進数で出す。出力は private サンプルのローカル比較用であり、曲データをリポジトリへ保存しない。
- [x] ダンプ行数と MIDI encoder の出力件数を比較し、`event_dump: count=... encoded_count=... consistency=OK/FAIL` を表示する。エンコード失敗または件数不一致は stderr に診断し、`--rcp2-event-dump` の終了コードも失敗にする（commit [ace293e](https://github.com/pikachu364/S-MU2000/commit/ace293edb02f8cc6fe151217143274684e593e05)）。
- [x] GitHub Actions CI の合格を確認（Actions #458、ユーザー報告）。#456 は同じブランチの優先度の高い待機実行があるためキャンセルされたもので、失敗とは扱わない。
- [x] private sample-check #10 の sample-9 artifact に代表曲のイベントダンプが出力されたことを確認。`rcp2-event-dump-info.txt` は `private-data/RCP_DATA/BUTTFLY/BUTTFLY.RCP` を選択したと記録している。レポートの先頭ブロックでは BUTTFLY の timeline / seconds / MIDI encoding が OK（28008 / 28008 / 27974 events）。
- [ ] イベントダンプの `event_dump: count=... encoded_count=... consistency=OK` 行と先頭/末尾イベント、複数トラックの並びを精査する。ダンプファイル自体はプライベート側に保持し、公開リポジトリへコピーしない。
- [ ] 参照コンバーター側のイベント列と同条件で比較する。現時点では一致や実再生互換性を主張しない。

実行例（サンプルファイルのパスはローカル環境に合わせる）:

```sh
./verify --rcp2-event-dump /path/to/sample.rcp > sample-events.txt
echo $?
```

終了コード `0` はダンプの内部件数整合性チェック成功、`1` は処理または整合性チェック失敗を示す。これは参照コンバーターとの一致を示すものではない。


## 2026-10-26 追記 — Actions #460 / private sample-check #10 合格

- ユーザー報告により公開リポジトリ Actions #460 と、`S-MU2000-private-data` の sample-check #10 の合格を確認。
- private `sample-9` に `rcp2-sample-report.txt`、`rcp2-event-dump.txt`、`rcp2-event-dump-info.txt` が存在することを GitHub 上で確認。ダンプ対象は `private-data/RCP_DATA/BUTTFLY/BUTTFLY.RCP`。
- レポートの BUTTFLY ブロックでは全体 timeline=OK (28008 events)、seconds=OK (28008)、midi_encode=OK (27974)。イベントダンプの内部 consistency 行・先頭/末尾の全件精査、および参照コンバーターとの比較はまだ未完了。
- 次はプライベート環境でダンプの consistency / 先頭・末尾・複数トラックの順序を確認し、参照コンバーター出力との差分を比較する。曲データとダンプは公開リポジトリへコピーしない。


## 2026-10-26 追記 — Actions #462 と sample-9 イベントダンプ監査

- ユーザー報告により Actions #462 合格を確認。
- private sample-9 の `rcp2-event-dump.txt`（2,449,128 bytes）を GitHub blob から取得して全行を解析。BUTTFLY.RCP、27,974イベント、`count=27974 encoded_count=27974 consistency=OK`。
- 全イベントの index は 0〜27,973 の連番、tick と time_seconds は非減少、MIDI bytes は全行に存在。出力トラックは 0〜11。先頭は tick 4 / 0.071428500 秒、末尾は tick 21,033 / 381.203079354 秒。
- 要約レポートは timeline=28,008、seconds=28,008、MIDI encoding=27,974。timeline と符号化後の件数が異なるため、比較時には MIDI 出力フィルター／非出力イベントの扱いを確認する。
- これはダンプ内部の整合性確認であり、参照コンバーターとの一致・全18トラックの出力内容・実再生互換性を意味しない。
- 次はプライベート側で参照コンバーターを用いた MIDI を生成し、SMF イベントを同一表現に正規化して比較する。曲データとダンプは公開リポジトリへコピーしない。


## 2026-10-26 追記 — 参照 MIDI 比較の準備

- [x] ValleyBell/MidiConverters の `rcp2mid.c` を調査。E7 は MIDI Set Tempo (meta 0x51) を出力する。p2 != 0 の場合は gradual tempo の警告を出すが、補間自体は実装せず p1 のテンポ値を即時適用する。
- [x] 参照実装の変換ソースが `rcp2mid.c` 単体ではなく、`stdtype.h`、`midi_funcs.h`、`midi_utils.h` 等の補助ファイルを include することを確認。比較ワークフローに組み込む前に、依存関係とコンパイル手順を確定する必要がある。
- [x] BUTTFLY の現行イベントダンプは内部整合性 OK（27,974件）。timeline 28,008件との差34件は E7 tempo イベントの非出力と一致するが、MIDI ファイル全体のメタイベント比較は未実施。
- [ ] プライベート Actions で参照コンバーターを固定 commit SHA から取得し、必要な補助ソースとともにビルドする最小手順を検証する。外部ソースを公開リポジトリへ複製・改変して保存しない。
- [ ] BUTTFLY.RCP だけを対象に参照 MIDI を生成し、SMF parser で各 track の delta time を absolute tick に変換してイベント種別・データ・tick を比較する。
- [ ] 比較レポートには、SMF format / PPQN、track 数、tempo/time-signature meta、note/channel event、SysEx、end-of-track、未対応または警告のあるコマンドを別々に集計する。トラック番号の違いだけで不一致と判定しない。
- [ ] 比較前に参照コンバーターの出力設定・ループ展開・テンポ処理を記録し、仕様差による差分と実装バグ候補を分ける。
- [ ] プライベートサンプルのファイル、MIDI、イベントダンプは private artifact 内だけに保持し、公開リポジトリへアップロードしない。

現段階では参照コンバーターのコンパイル・MIDI生成・SMF比較は未実行。上記の調査は比較の前提を確認したものであり、互換性合格を意味しない。

## 2026-10-26 追記 — sample-13 参照 MIDI 比較結果

- ユーザー報告により、公開リポジトリ Actions #468 と private sample-check #15 の合格を確認。
- private の sample-13 で BUTTFLY.RCP の C++ イベント列と参照 MIDI を意味的に比較し、双方 27,974 イベント、mismatch_count=0、exact_match=true となった。
- 比較器は Note Off の表現差（MIDI 0x8n Note Off と 0x9n velocity 0）を比較時のみ正規化する。S-MU2000 の再生出力は従来どおり 0x8n Note Off を維持し、比較器の正規化を再生出力へ持ち込まない。
- この結果は sample-13 の比較条件におけるイベント一致を示す。全サンプル・全 RCP2 コマンドの互換性や、実機での再生成功を保証するものではない。
- 次は、比較レポートの条件と対象ファイルを private 側で保持したまま、別サンプルにも同じ比較を適用する。差分が出た場合は、イベント種別・tick・チャンネルデータ・SysEx を分け、仕様差と実装不具合候補を区別する。


## 2026-10-28 — sample-26 の同一tickイベント順序差分

- private 側の `S-MU2000-private-data/Sample report files/rcp2-sample-report/sample-26/reference-comparisons/` にある4サンプルの比較結果を確認。
- sample-01 BUTTFLY: parser/reference とも 27,974 events、不一致627。
- sample-02 FC: 両方20,299 events、不一致16。
- sample-03 WINGS_55: 両方23,745 events、不一致598。
- sample-04 L_WING88: 両方22,536 events、不一致3。
- L_WING88 の track 12 / tick 3648 は parser が `bc0761 → bc2000 → 9c3600`、reference が `9c3600 → bc0761 → bc2000`。イベント数・tick・イベント集合は一致し、順序が異なる。
- FC の track 6 / tick 5135 および 5263 では CC と Note Off の順序が逆。L_WING88 と FC の例で優先順が異なるため、「同じtickならCC優先」などの固定優先順位を推測で導入しない。
- 現行 `expand_note_gates()` の優先順位調整は CC・Note Off・新規 Note の3種類が同一tickに共存する場合に限定され、今回の少数差分を説明しない。
- 次の作業: 参照コンバーターの `rcp2mid.c` で running note の終了・イベント出力処理を追い、各差分tickに対応する元RCPレコード順と比較する。イベント集合の差と順序だけの差を分離し、多数差分も同じ原因か検証する。
- 実装変更・CI再実行はまだ行っていない。比較資料と曲データは private repository 内に保持し、公開リポジトリへコピーしない。

比較資料:
- [sample-01 event-compare](https://github.com/pikachu364/S-MU2000-private-data/blob/main/Sample%20report%20files/rcp2-sample-report/sample-26/reference-comparisons/sample-01/event-compare.txt)
- [sample-02 event-compare](https://github.com/pikachu364/S-MU2000-private-data/blob/main/Sample%20report%20files/rcp2-sample-report/sample-26/reference-comparisons/sample-02/event-compare.txt)
- [sample-03 event-compare](https://github.com/pikachu364/S-MU2000-private-data/blob/main/Sample%20report%20files/rcp2-sample-report/sample-26/reference-comparisons/sample-03/event-compare.txt)
- [sample-04 event-compare](https://github.com/pikachu364/S-MU2000-private-data/blob/main/Sample%20report%20files/rcp2-sample-report/sample-26/reference-comparisons/sample-04/event-compare.txt)


## 2026-10-28 — Actions #510 合格後：参照コンバーターの Note Off 発行順をソース確認

- ユーザー報告により Actions #510 合格を記録。今回の調査は参照コンバーターのソース読解であり、新しいコード変更や sample-26 再比較を行ったという意味ではない。
- 参照先：`ValleyBell/MidiConverters` の commit `a73c748960d57bd194fb64931ce07d2c1fa1bd01`、`rcp2mid.c` と `midi_utils.h`。
- `RcpTrk2MidTrk()` は各 RCP コマンドを処理する前に `CheckRunningNotes()` を呼ぶ。ノート残り長がそのコマンドまでの遅延内で期限切れになる場合、参照コンバーターは現在のコマンドを書き出す前に Note Off を出力する。複数の期限切れノートは `RunNotes` 配列の順で処理され、削除時に配列を詰める。
- これは「同じ tick の全イベントに固定優先順位を適用する」方式とは異なる。Note Off の位置は、RCP コマンド処理中の期限判定・発行タイミングに依存する。
- ただし sample-26 の FC で観測した CC → Note Off の例が、すべてこの running-note 経路で説明できるとは未確認。明示的な Note Off 相当イベント、ノート再利用/延長、同一 tick の複数コマンドの遅延状態を切り分ける必要がある。
- 次工程：既知差分の tick について、RCP ソースレコード順と各コマンドの delay/duration を対応付け、Note Off が参照コンバーターの running-note 自動生成か、入力由来イベントかを判定する。その後に限り実装規則を変更する。現時点では推測による優先順位変更を行わない。


## 2026-10-28 — Gearmulator RCP2 参照実装との照合

- 参照コミット [gearmulator 619b1d9](https://github.com/masanaohayashi/gearmulator/commit/619b1d9b7f7755ea7eee3bd93d8dd2a705e79465) の `RcpFilePlayer.cpp` を確認。
- `decodeTrackLength()` は格納値の bit 0-1 を bit 16-17 に移して拡張トラック長を復元する。S-MU2000 は2 byte値をそのまま使っていたため、ここを先に修正する。
- 併せて旧形式 ST+ の判定、最大36トラック、R36 のポート/チャンネル割当、SysEx F0、ノート期限処理、FC 連鎖を差分候補として記録。これらは未修正・未検証。
- 参照実装の挙動を採用したことは、RCP2 公式仕様として確定したことを意味しない。修正後の CI と実サンプル比較は別途確認する。


## 2026-10-28 — sample-27 参照比較結果

- ユーザー報告：Actions #516 は macOS job が混雑で未完了、その他ジョブは合格。
- private sample-27 の `reference-comparisons` を確認。sample-01〜04 の parser dump と参照 MIDI は各曲で総イベント数が一致するが、順序比較は全曲 exact match ではない。
- mismatch_count: sample-01=627、sample-02=16、sample-03=598、sample-04=3。各 mismatch は同一 tick に集中しており、イベント数差より同一 tick 内の並び順・生成順を優先して調査する。
- sample-02 には CC / Note On / Pitch Bend 等の並び違い、sample-04 は track 12 tick 3648 で CC・Note On の並び違いが3件。これだけで参照側が正しいと断定せず、元 RCP レコード順と Note Off 自動生成時点を追跡する。
- sample-27 は比較レポートと参照 MIDI を用意した段階。今回の確認ではコード変更・CI再実行・実再生は行っていない。次は代表差分を元レコードへ対応付けて分類する。

## T-009 — 同一tickイベント順序差分の原因特定（最優先）

**状態:** 参照側の running-note 期限処理と現行実装のイベント生成タイミングに差があることを確認。sample-27 の不一致原因は未確定。コード修正は保留。

- [x] private sample-27 の sample-01〜04 の比較結果を確認し、不一致数を記録する（01=627、02=16、03=598、04=3）。総イベント数・トラック別イベント数は各曲で一致。
- [x] 現行 `src/rcp.cpp` の Note Off 相当イベント事前挿入と同一tick sort 条件を確認する。
- [x] ValleyBell/MidiConverters の固定 commit `a73c748960d57bd194fb64931ce07d2c1fa1bd01` にある `CheckRunningNotes()` を確認する。参照側は各 RCP レコード処理前に期限到来ノートを出力し得る。
- [x] `RcpTrk2MidTrk()`、`MidiDelayHandler()`、`WriteMidiDelay()` の主要呼び出し経路を追跡する。Note On 経路での直接呼び出し、イベント出力時の callback、コマンド処理後の `cmdP0Delay` 加算、トラック終端の `FlushRunningNotes()` を確認した。
- [x] `MidiDelayHandler` の定義と主要な delay 更新・消費経路を追跡する。
- [ ] 差分再現に必要な delay の詳細（イベント出力を伴わないコマンドやループ分岐も含む）を確認し、tick ごとの状態遷移表にする。
- [x] sample-02 / sample-04 の `info.txt` と比較レポートを再確認し、元サンプルが FC.RCP / L_WING88.RCP であることと、代表差分 tick を確定する（sample-02: track 2 tick 13092/13128/13524、track 5 tick 8892、track 6 tick 5135/5263。sample-04: track 12 tick 3648）。
- [ ] private sample-02 / sample-04 の差分イベントを元 RCP レコードへ対応付け、record index、byte offset、4-byte record、累積 tick を記録する。
- [ ] 各差分直前の参照側 `*delay`、running note の `remLen`、期限判定・イベント発行順を取得し、現行実装との差を説明できるようにする。
- [ ] sample-01 / sample-03 の多数差分も同じ原因か、イベント集合差と同一tick内の順序差を分離して検証する。
- [ ] 根本原因と回帰テストが確定した後に限り、必要最小限の実装修正を行う。固定のイベント種別優先順位を推測で追加しない。
- [ ] 修正後に Actions CI と private sample 比較を再実行し、差分数・イベント数・比較条件を記録する。

**合格条件:** 代表差分を元 RCP レコードと参照側の期限処理に対応付け、順序が決まる因果関係を説明できること。修正した場合は、合成回帰テストと private sample の比較結果で改善を確認すること。CI 合格だけで互換性合格とはしない。

**データ保護:** 元 RCP ファイル、private のイベントダンプ、比較レポートは private repository / artifact 内だけに保持し、公開リポジトリへコピーしない。


## 2026-10-29 — sample-04 順序差分の絞り込み（調査継続）

**状態:** 原因未確定。ソース変更なし。

- 最新 private Action #42 artifact を取得し、ZIP整合性と31ファイルを確認。比較値は sample-01=627、sample-02=16、sample-03=598、sample-04=3（各サンプルで総イベント数一致）。
- sample-04 の代表差分は Track 12 / tick 3648: parser `BC 07 61 → BC 20 00 → 9C 36 00`、reference `9C 36 00 → BC 07 61 → BC 20 00`。
- 現行 `expand_note_gates()` は tick だけで stable sort し、CC→Note Off→新規 Note の優先処理は3種類が全てある場合だけ実施する。CC2件と Note On velocity 0 の組み合わせには適用されない。
- 次の優先作業: private runner 上でイベントと元 RCP レコード（track、record index、byte offset、4 byte、累積 tick）を対応付ける。元レコード順と、期限切れ Note Off の生成時点を照合し、差分が最初に生じる段階を特定する。
- 原因確定前に固定優先順位や Note Off の生成処理を変更しない。CI 合格、比較イベント数一致、実際の再生確認を別々に記録する。


## 2026-10-29 follow-up — sample-04 SysEx 0xFF

Current source review confirms src/rcp.cpp already skips unknown high-bit SysEx template bytes such as 0xFF, with a comment explicitly matching reference-converter behavior. Do not change code for this warning unless new evidence shows a behavioral mismatch.

- [x] Compare reference ProcessRcpSysEx() with S-MU2000 src/rcp.cpp 0x98 expansion.
- [ ] Recover full three SysEx payloads and determine whether 0xFF has any documented intended meaning beyond being skipped by the reference converter.
- [ ] Check whether the warning has any relation to sample-04 same-tick ordering differences; currently no relation established.


## 2026-10-29 — ValleyBell Issue #16 を踏まえた参照コンバーターの位置づけ

**状態:** 重要な比較方針を追加。サンプル04のPC上での手動変換は未実施。

- [ValleyBell/MidiConverters Issue #16](https://github.com/ValleyBell/MidiConverters/issues/16) の本文と作者コメントを確認。ValleyBell氏は2024-03-24、自身のRCP converterについて「shingo45endo/rcm2smf より少し劣る」と自己評価し、より特殊なSysExコマンドの確認に sayleen を使う可能性に言及している。
- このコメントは当時の作者自身による評価であり、現在の全バージョンや sample-04 の出力が誤っていることを証明するものではない。一方、ValleyBell版を唯一の正解オラクルとして扱わない根拠になる。
- 次の比較は3系統を区別する: (1) ValleyBell rcp2mid、(2) shingo45endo rcm2smf、(3) sayleen。各ツールのバージョン/コミット、変換ログ、生成MIDI、未対応・警告を記録し、差分が出たイベントを仕様資料と元RCPレコードで判定する。
- 最初にWindows PC上で sample-04 を ValleyBell版で実変換し、続いて同じ入力を rcm2smf と sayleen でも比較できるか確認する。sample-04 の原本は private に保持し、公開リポジトリへ置かない。
- [ ] PC上で ValleyBell rcp2mid をビルド/実行し、sample-04 の終了状態・警告・MIDI生成を確認する。
- [ ] 同じ sample-04 を rcm2smf でも変換し、出力イベントを比較する。
- [ ] 特殊SysExについて sayleen の結果を比較できるか調査する。
- [ ] ツール間の差分を、RCP生レコード・形式資料・実再生結果と照合する。単純な多数決で正解を決めない。


## 2026-10-29 追記 — Actions #556 / private sample-check run 40868277263

- ユーザー報告により Actions #556 合格を記録。
- private sample-check の成果物 [run 40868277263](https://github.com/pikachu364/S-MU2000-private-data/actions/runs/40868277263/artifacts/12894966176) を取得して比較レポートを確認。
- sample-01 BUTTFLY: parser/reference とも 27,974 events、不一致 0。
- sample-02 FC: 両方 20,299 events、不一致 4。残る差分は Track 6 / tick 5135 と 5263 の各2イベントで、parser は Note Off相当 → CC、reference は CC → Note Off相当。
- sample-03 WINGS_55: 両方 23,745 events、不一致 0。
- sample-04 L_WING88: 両方 22,536 events、不一致 0。
- 4サンプルすべて総イベント数とトラック別件数が一致。合計 94,554 events、比較上の不一致は4件。これは参照コンバーターとの比較結果であり、実機再生の確認ではない。
- 次は sample-02 の Track 6 の元RCPレコード・累積tick・ノート期限判定を対応付け、固定イベント優先順位を追加せず原因を特定する。


## 2026-10-29 — 明示的 Note Off の順序修正を実装（CI待ち）

- `src/rcp.cpp`: active note と同音高の duration=0 レコードを処理した際、保留中 Note Off の tick だけでなく `expanded` 内の位置も現在の timeline 末尾へ移動するよう変更。削除による他の `running_note.note_off_index` の補正を追加。
- `src/verify.cpp`: Note On の delay で tick 5 に進み、CC と duration=0 同音高レコードを同tickに置く回帰テストを追加。期待順序は Note On → CC → Note Off。
- コミット: `d0761a04521bda02b8db3dfac8310edfb4c0e3f7`（実装）、`d01b1578e9ca5e897344b4a49c3277828b26337d`（回帰テスト）、`eb5b1c72ad7659134305155708dc94f9a5056f37`（テストtick設定の修正）。
- 現時点で Actions 実行結果は確認できていない。ビルド・回帰テスト・private 4サンプル比較が通るまでは修正成功と判定しない。


## 2026-10-29 — Actions #564 失敗の原因と修正

- ユーザー報告の #564 Linux はコンパイル成功し、追加した「RCP2 明示的 Note Off 同tick順序」合成テスト自体も「合」だった。
- 失敗原因は回帰テストの出力文字列で改行を `\\n` と二重エスケープしており、実際の出力に文字列 `\n` が混ざったこと、および `tests/verify.txt` の期待出力を同期していなかったこと。
- `src/verify.cpp` の改行エスケープを修正（commit `58343ca34a18c1dcac304cec69879e5bf699968d`）、`tests/verify.txt` に期待行を追加（commit `ac716012995c0404a4d51bb819a514fac5ad136a`）。
- この修正後の CI は未確認。#564 を合格扱いにせず、次の Actions で verify 指紋比較と他ジョブを確認する。


## 2026-10-29 — Actions #567 / private sample-check run 40885695400

- ユーザー報告で公開リポジトリ Actions #567 合格。
- private artifact [run 40885695400 / artifact 12902283152](https://github.com/pikachu364/S-MU2000-private-data/actions/runs/40885695400/artifacts/12902283152) を取得して比較レポートを確認。
- sample-01 BUTTFLY: parser/reference 27,974 events、mismatch 0。
- sample-02 FC: parser/reference 20,299 events、mismatch 0。前回残っていた Track 6 / tick 5135, 5263 の同一tick順序差が解消。
- sample-03 WINGS_55: parser/reference 23,745 events、mismatch 0。
- sample-04 L_WING88: parser/reference 22,536 events、mismatch 0。
- 合計 94,554 events。全4サンプルで総数・各トラック数が一致し、全イベント列が完全一致（exact_match=true）。参照コンバーターは固定 commit `a73c748960d57bd194fb64931ce07d2c1fa1bd01`。
- これにより直近の Note Off 展開変更は、合成回帰テストと今回の4サンプル参照比較で良好な結果。実機再生・全RCP2互換性までは証明しない。
- 次は比較結果を履歴文書へ反映したうえで、次の未完了タスクに進む。

## 2026-10-30 — sample-04 SysEx 0xFF の追加調査

**状態:** 意味未確定。コード変更なし。private sample の生バイト列はこの公開文書へ転記しない。

- ValleyBell の `ProcessRcpSysEx()` は `0x80..0x84` と `0xF7` 以外の上位ビット付きバイトを未知コマンドとして警告し、そのバイトを出力せず処理を継続する。
- `rcm2smf.js` の `convertSysEx()` は未知バイトを通常データとして扱った後、7-bit 範囲検証で SysEx 定義全体を無効にする。したがって両ツールは同じ警告を出しても、後続データの扱いは同じではない。
- 前回の暫定メモで「`0xFF` を `0x7E` 相当としてチェックサムが整合する」と記載したが、独立再計算で裏付けられず、根拠として使用できないため撤回。`0xFF` を何らかの値へ置換する仮説を支持するチェックサム証拠は現時点でない。
- ValleyBell の固定版 `RCPFormat.txt` では、User SysEx の保存形式と `0x98` の利用は説明されているが、今回のような `0xFF` の置換規則は確認できなかった。
- 現行 `src/rcp.cpp` は ValleyBell と同様に未知の上位ビット付きテンプレートバイトをスキップする。現状、参照コンバーターとの差分を示す証拠はないが、SysEx の意味的正しさや実機動作を証明したことにもならない。
- 次の作業は、RCP2 の別実装・仕様資料から `0xFF` の意味を探し、出力 MIDI の3件を独立した SysEx 検証で確認すること。根拠が見つかるまでは置換・警告抑制・挙動変更を行わない。


## 2026-10-30 — SysEx 0xFF の仕様照合後

**判明したこと:** ValleyBell の `RCPFormat.txt` が定義する `0x98` のテンプレート制御値は `0x80`–`0x84` で、今回の `0xFF` に対応する置換規則は記載されていない。Gearmulator の現行処理も `0x98` のデータを後続 `0xF7` 継続レコードから読み、未知の高ビット値をスキップする。S-MU2000 の動作はこの点で参照規則と一致している。

**次の作業**
1. private 環境で該当3件の `0x98` と継続 `0xF7` レコードの raw bytes、record index、file offset を対応付ける。
2. S-MU2000 と ValleyBell が出力した該当 SysEx 3件を private 環境で比較する。公開リポジトリへ生データは転記しない。
3. 元データの意図を説明する一次資料がない限り、`0xFF` の置換、チェックサム修正、警告抑制はしない。
4. イベント列の一致と SysEx の意味的妥当性・実機再生確認を別々に記録する。

**制約:** 今回の更新は資料・ソースレビューに基づく。実機再生やローカルビルド・テストは実施していない。


## 2026-10-30 — SysCheck error と再生継続の切り分け

**目的:** 88emu の SysCheck error が、RCPファイル読み込み失敗なのか、特定 SysEx の生成/検査警告なのか、音源ファームウェア側の checksum error なのかを特定する。

- [x] Gearmulator の RCP構造解析で全体失敗となる条件を確認。
- [x] Gearmulator の `0x98` SysEx展開で未知高ビット値がスキップされ、後続イベント処理が継続する経路を確認。
- [x] 正常な `0x98` / `0xF7` 継続レコードの既存 unit test を確認。
- [ ] 88emu で表示される正確な SysCheck error 文言、表示位置、発生タイミングを特定する。
- [ ] 88emu の parser / event expansion / MIDI dispatch / firmware response の各段階を分けてソース追跡する。
- [ ] 対象 SysEx 前後のノート・CC等が再生経路に残るか確認する。
- [ ] 未知高ビット値を含む最小合成 fixture を作り、RCP全体の読み込み成功・後続ノートの保持・SysEx出力の有無を個別にテストする。
- [ ] 上記が確認できてから、S-MU2000 に同じ継続仕様が必要か、既存挙動で十分かを判断する。

**注意:** 「SysCheck error が出ても聴ける」という観測は、エラーが非致命である可能性を示すが、どの段階のエラーかは特定しない。エラー表示を隠す、異常バイトを置換する、checksum を自動修正する変更は行わない。


### 追加確認 — 88emu "Check Sum Error" 表示との照合

- [x] SC-88 sub-MCU が Roland DT1 checksum 不一致時に `0xE0 / param1=3` の "Check Sum Error" 通知を出し、その DT1 の処理だけを拒否することを確認。
- [x] SysEx 終端後に assembly をクリアし、後続 MIDI byte を通常処理できる経路を確認。
- [ ] ユーザーが見たエラーの正確な画面表示が "Check Sum Error" か確認する。
- [ ] 対象3件の S-MU2000 / ValleyBell 生成 SysEx を private 環境で比較し、Roland DT1 として sub-MCU checksum 検査に入る形か確認する。
- [ ] checksum 不一致なら、そのメッセージだけが拒否される想定と、後続ノートの再生継続を分けて確認する。

**暫定結論:** エラー表示が Roland "Check Sum Error" なら、曲全体は停止せず、個別 DT1 だけ拒否されるため、88emuで再生を聴ける可能性が高い。まだ今回の3件が該当すると確定していない。



## 2026-10-30 — Check Sum Error 調査の訂正と次の確認

1. Gearmulator SC-88 sub-MCU の DT1 最低長条件と checksum error 通知条件を区別する。
2. 今回の3件（各8-byte body）は最低長不足で checksum 検査前に無通知 return するため、Check Sum Error の原因候補から外す。
3. private sample に含まれる他の DT1 SysEx を抽出し、body長・checksum・参照MIDIとの対応を調べる。元データの値は推測で変更しない。
4. 88emuで過去に表示された Check Sum Error がどの曲・どのタイミングだったかは、ユーザーが特定できる場合に限り照合する。特定できなくても調査を継続する。

**完了条件:** エラー表示を発生させるDT1を特定するか、private sampleに含まれるDT1が表示条件に該当しないことを示す。RCP parser の成功、DT1個別の拒否、後続MIDIの演奏継続を別々に記録する。



## 2026-10-30 — sample-04 DT1 checksum 棚卸し結果

保存済みsample-04 validation artifactのevent dumpからRoland DT1を120件抽出した。body長9 bytes以上の117件はすべてchecksum合計0。残り3件は8-byte bodyでchecksum byteがなく、Gearmulator SC-88 sub-MCUではchecksum検査前に無通知returnする。したがって、同artifactのDT1イベントにはGearmulatorのCheck Sum Error通知条件に該当するものはなかった。

次の候補:
1. 他のsampleでも同じ棚卸しを行う。
2. 過去のCheck Sum Errorが別曲で起きた可能性を残す。
3. 実再生確認と静的checksum照合を混同しない。
4. checksum補正やraw data変更は行わない。


## 2026-10-30 — 4 sample DT1 checksum 棚卸し結果

保存済みsample-01〜04のparser event dumpからRoland DT1を503件抽出。500件はGearmulatorの最低長条件を満たし、全件checksum合計0。残る3件はsample-04の8-byte bodyでchecksum byteなし、Gearmulatorでは検査前に無通知return。checksum不一致は0件。

次の候補:
1. 新しいsampleが追加されたら同じ棚卸しを実施する。
2. 過去に表示されたCheck Sum Errorが別曲・別SysEx由来である可能性を残す。
3. 実再生確認と静的event dump検査を区別する。
4. checksum補正やraw data変更は行わない。


## 2026-10-30 追加調査 — Check Sum Error 後の継続処理

**確認済み**
- Gearmulator SC-88系統合テストソースは、checksum 不正 DT1 が反映されないことと、その後の正常 DT1 が反映されることを順に検査する構成。
- Gearmulator changelog には、SC-55mkII で高密度 SysEx 時に同じ byte が二重に渡され、Check Sum Error / DT1 Data Error が出るタイミング競合の修正記録がある。
- 後者は SC-55mkII 固有の修正履歴として扱い、SC-88系や今回の sample へ一般化しない。

**次の作業**
1. 新規 sample が追加されたら、既存の Roland DT1 body length / checksum 検査を継続する。
2. 過去のエラー表示について機種・正確な文言・発生タイミングが分かれば、DT1 checksum不一致と高密度SysExタイミング競合を切り分ける。
3. 必要なら private event dump から SysEx の時間密度（同一tickおよび短時間窓の件数）を集計する。ただし density だけでエラー原因とは断定しない。
4. 88emu 実再生や Gearmulator optional integration test を実行した場合は、その実行条件・結果をソースレビューとは別に記録する。

**禁止事項:** 根拠なしの checksum 補正、SysEx byte 置換、警告抑制、SC-55mkII の既知不具合を SC-88系へ横展開する変更は行わない。


## 追加確認 — sample-04 の SysEx 高密度区間

保存済み4 sampleのparser event dump集計では、任意の100ms窓の最大Roland DT1件数が sample-01=5、sample-02=4、sample-03=6、sample-04=26。sample-04の同一tick最大件数は5件。最大密度区間には0x10系アドレスを含むDT1があるが、表示dot animationかどうかは未確定。

1. sample-04 の0x10系DT1の意味を表示系アドレス処理と照合する。
2. 必要に応じて private 環境で実際の送信byte数とwire durationを見積もり、parser event timestampと区別する。
3. 新しいsampleにも同じ時間密度集計を適用する。
4. 88emu実再生で再現を確認できるまでは、SC-55mkIIの既知不具合と今回のsampleを同一原因と扱わない。

100ms窓は調査用の集計幅であり、不具合閾値ではない。
