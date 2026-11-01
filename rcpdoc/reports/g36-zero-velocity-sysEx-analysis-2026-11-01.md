# 2026-11-01 — G36 zero-velocity note と SC-8850 SysEx 調査

## 調査範囲

対象は private data repository の G36 4 sample（85SO018A、EVA3_22D、SAKURA、TAKARAJI）と、公開参照実装 ValleyBell MidiConverters の `rcp2mid.c`。このレポートにはprivate sampleのraw bytes、private path、曲データ本体を転載しない。

今回の作業は既存データ・参照実装の静的解析であり、S-MU2000のソース変更、ローカルビルド、CI実行、実機再生は行っていない。

## 1. 85SO018A — SC-8850関連情報

### 確認できた内容

- ユーザー報告では85SO018AはSC-8850で制作された曲。G36タイトル欄にも `SC-8850` の記載がある。
- G36ヘッダー内のUser SysEx定義領域には8スロットがあり、7スロットに定義名・テンプレートデータがある。確認できた名称は以下。
  - GS MASTER VOLUME
  - GS MASTER KEY-SHIFT
  - GS MASTER PAN
  - GS SYSTEM RESET
  - GS REVERB MACRO
  - GS CHORUS MACRO
  - GS MASTER TUNE = 442.0
- 曲中の `0x98` SysExコマンドは16件確認。ValleyBell実装では後続の `0xF7` 継続レコードからSysExデータを組み立て、テンプレート制御値を置換してMIDI SysExを生成する。
- この曲では `0x90–0x97` のUser SysExスロット呼び出しは確認されず、ヘッダー定義が存在することと、曲中でそのスロットが呼び出されることは別である。
- 直接SysExにはRoland GS系の識別情報を含むものがあり、SC-8850制作というユーザー情報と整合する。
- `0xEB` はValleyBell実装上Control Change、`0xEC` はInstrument/Program Change相当として扱われる。85SO018Aでは両コマンドが存在する。

### 解釈上の注意

- GS共通の識別情報だけでSC-8850専用メッセージと断定しない。
- ヘッダー内User SysEx定義はテンプレート情報であり、実際に送信されたかどうかは曲中の呼び出しコマンドで別途判断する。
- CC 98/99/100/101やCC 6の出現はNRPN/Data Entry系制御と整合するが、個々のパラメーター番号・値の意味は全イベントの順序と値を照合して確定する。
- SysExの機種固有機能や各payloadの用途は、Roland公式SC-8850資料のアドレス表・実データを一件ずつ照合して分類する。GS互換であることとSC-8850固有であることを混同しない。

## 2. SAKURA — Drums channel 9 / pitch 36

### 既知の差分

- G36 Track 9はDrums、MIDI channel 9。
- pitch 36のsource note recordsは206件、そのうちvelocity=0 / duration>0は206件。
- 以前のMIDI集計ではValleyBellとrcm2smfのchannel 9 / pitch 36に410イベントの差があり、205 records × 2イベントで説明され、1 record分が例外として残っていた。
- ValleyBellの `rcp2mid.c` には、同じpitchのnoteがまだ有効な間に同音程のイベントが来た場合、既存running noteを更新・統合する処理がある。

### 例外候補と訂正

- G36 Track 9のrecord index 991はpitch 36、velocity 92、delay 120、duration 450。
- record index 992、993、994は同じpitch 36、velocity 0、delay 120、duration 1で連続する。
- これらはrecord 991のdurationが満了する前に現れるため、ValleyBellのrunning-note重複処理に関係する候補である。
- **重要:** 992だけが抑止され、993/994は独立出力されると断定できる証拠はまだない。3レコードとも元noteの有効期間内に現れるため、実際にどのレコードが統合されるか、ValleyBellのrunning-note状態遷移とMIDI出力を再照合する必要がある。
- したがって、以前の「例外1件の原因を確定」という説明は、現時点では「record 992付近の同音程・duration重複が有力候補」に修正する。例外の厳密な1件対応は未確定。

## 3. 4 sampleのzero-velocity note差分

既存の集計結果では、ValleyBellとrcm2smfの差は、zero-velocity / nonzero-duration source note recordsの扱い、およびループ展開・running-note統合の違いと関係する。

85SO018Aでは、ループ内259 records、ループ外20 records、計279 records。ValleyBellの2-pass loop展開を前提にすると、
`259 × 2 passes × 2 MIDI events + 20 × 2 MIDI events = 1076 events`
となり、両コンバーター間のzero-velocity Note Onイベント数差と一致する。これは参照コンバーター間の差分説明であり、どちらかを仕様上の唯一の正解とするものではない。

SAKURAではDrums channel 9に差分が集中し、loop pairがない。zero-velocity noteの扱いに加えて、同音程running-noteのduration重複処理が差分に影響する可能性がある。record単位の完全対応は継続調査する。

## 4. S-MU2000実装への影響

- 現行G36 parserの6-byte sequence record解析と、`0x98`後続の`0xF7`継続データブロックの意味解析は区別する。
- `0x98`を通常の6-byteイベント1件として終端扱いせず、後続継続レコードとの関係を保持できる設計が必要。
- zero-velocity / nonzero-duration noteを一律に削除・Note Off化する変更は行わない。loop展開、running-note重複、同音程の重なり、duration=0を分けて検証する。
- ValleyBellは有力な比較対象だが、参照実装の動作をG36正式仕様と同一視しない。

## 5. 次の調査

1. ValleyBellのrunning-note管理部分を再読し、record 991–994の各イベントが既存noteの期限・同音程状態をどう更新するか、MIDI出力と対応付ける。
2. SAKURA全4 pitch（36/37/40/45）のsource recordsを、同一pitchの有効期間と出力イベントに対応付ける。
3. 85SO018Aの16件の`0x98` SysExについて、全payload・置換後byte列・Roland checksumを再検証し、SC-8850公式資料のアドレス表に沿って用途を分類する。
4. `0xEB` CCおよび`0xEC` Program Change相当イベントを時系列・チャンネル別に集計し、NRPN/Data Entryの選択と値設定のペアを検証する。
5. 上記の仕様・差分整理が終わるまでS-MU2000ソースは変更しない。

## 参照

- [ValleyBell MidiConverters — rcp2mid.c](https://github.com/ValleyBell/MidiConverters/blob/master/rcp2mid.c)
- private sampleは `pikachu364/S-MU2000-private-data` 内で調査。raw dataは公開リポジトリへ複製しない。


## 6. 2026-11-01追記 — ValleyBell実MIDI照合と前回説明の訂正

### 実ファイルで確認した事実

- private source の SAKURA.G36 Track 9（Drums / MIDI channel 9）には、pitch 36 / velocity 0 / duration > 0のsource recordが206件ある。
- ValleyBell版 SAKURA.mid は完全な32,395 bytesのファイルとして取得・解析でき、SMF track数は37。
- MIDIの11番目のtrackでchannel 9 / pitch 36を集計すると706イベント。その内訳はNote On velocity > 0 が148件、Note On velocity = 0 が558件、Note Off statusのイベントは0件。ValleyBell出力ではNote Off相当がNote On velocity 0として記録されている。
- MIDI上の例としてtick 8880にpitch 36 velocity 92、tick 8881に同pitch velocity 0がある。後続にも同様のイベント列が存在する。

### 重要な訂正

- 直前の会話で提示した「全1,058レコードのシミュレーションで抑制はrecord 992の1件だけ」「残り459件が出力」という結果は、G36の小節リピート（0xFC）・ループ（0xF8/0xF9）および実際のMIDI出力時刻を正しく再現した検証ではない。したがって、この数値とrecord 992だけが抑制されるという説明は撤回する。
- その後の実MIDI解析で確認した706件は、206件のsource velocity=0 recordと単純に一対一対応しない。558件のvelocity=0 MIDIイベントには、通常ノートの自動Note Off相当も含まれる。
- 以前の「ValleyBellとrcm2smfのpitch 36イベント差410件 = 205 source records × 2」という算術的一致だけでは、どのsource recordが出力・統合されたかは証明できない。
- rcm2smf版MIDIは以前の取得が完全ファイルであると確認できていないため、296件という過去集計も今回の再解析では独立に再確認していない。

### 参照実装で確認した処理

- ValleyBell rcp2mid.cは各ノートレコードの前にCheckRunningNotes()を呼び、同じpitchのrunning noteが残っていればremLen = MTS->curDly + cmdDuratとして既存ノートを更新し、新しいNote Onを抑止する。
- CheckRunningNotes()はdelay + 1と残り時間を比較し、期限到達時にNote Off相当を出力してdelayを調整する。
- G36 track処理は0xFCの小節リピート、0xF8/0xF9のループ、0xFDの小節終端を処理する。これらを無視した単純なrecord順シミュレーションは実出力の再現にならない。

### 結論と制約

- ValleyBellのrunning-note統合がSAKURAの差分に関係する可能性はあるが、record 991–994のどれがどのMIDIイベントに対応するかは未確定。
- 706件の実MIDI集計は確認済みだが、source record単位の対応付け、rcm2smfとの差分原因、S-MU2000側の修正要否は未確定。
- 今回はprivateデータの静的解析と公開参照ソースの確認のみ。S-MU2000ソース変更、ビルド、CI、実機再生は行っていない。

### 次の検証

1. ValleyBellのG36 track interpreterを、0xFC / 0xF8 / 0xF9 / 0xFDとrunning-note状態を含めて正確に再現するか、実行可能な参照コンバーターを用いてsource recordとMIDI出力をトレースする。
2. 完全なrcm2smf版SAKURA.midを取得し、同じtrack/channel/pitch条件でNote On正速度・Note On velocity 0・Note Offを別々に集計する。
3. その後、pitch 36/37/40/45をrecord単位で照合し、差分がloop traversal、running-note統合、または別の変換規則のどれに起因するかを判定する。
4. 根拠が揃うまでS-MU2000のノート変換ロジックは変更しない。


## 追加検証 — pitch 36 running-noteモデル（2026-11-01）

**結果:** ValleyBellソースのrunning-note処理とdelay消費に沿ってrecord-level状態遷移を追ったモデルでは、pitch 36の新規Note On抑止対象はsource record #993となった。

- record #992: pitch 36 / velocity 92 / duration 450 / P0 delay 120。
- record #993: pitch 36 / velocity 0 / duration 1 / P0 delay 120。
- record #993直前にpitch 44の満了処理が1 tickを消費し、残delayは119。既存pitch 36 running noteが残るため、remLen = 119 + 1 = 120としてrecord #993の新規Note Onを抑止するモデルとなる。
- pitch 36 source records 356件のうちduration 0は2件。モデル上Note Onは353件（正velocity 148、zero velocity 205）、自動Note Off相当は353件、合計706件。
- ValleyBell版MIDI Track 11 / channel 9 / pitch 36の実測706件（正velocity Note On 148、zero velocity Note On 558、explicit Note Off 0）と一致。
- 全channel 9についてもモデルのNote On 920件（正velocity 461、zero velocity 459）+自動終了920件=1,840件となり、既存実測と一致。

**この結果の位置づけ:** これはValleyBellのソースコードに基づく状態遷移モデルであり、ValleyBell実行ファイルを実行して得たrecord-by-record traceではない。MIDI件数との一致はモデルの整合性を支持するが、全イベントのtick対応やrcm2smfとの差分原因を確定しない。

同じモデルでpitch 37/40/45の集計も得たが、ValleyBell実MIDIとの独立照合は未完了。完全なrcm2smf版SAKURA.midも未取得。したがってS-MU2000側の修正要否は未確定であり、ソース変更は行わない。


## 追記 — rcm2smf版SAKURA.midの実体照合（2026-11-01）

ユーザー添付のSAKURA.midを独立解析した。サイズ22,878 bytes、SHA-256 4d64af89a1723beb3856c832a2764ebc762c484975681d86716763d07ec99f70。SMF Format 1、12 tracks、TPB 480で、全MTrkチャンク長がファイル末尾と一致し、全イベントを解析できた。

### 実測

- Track 11 (Drums, MIDI channel 9) のNote On status 0x99: 922件（positive velocity 461、velocity 0が461）。explicit Note Off status 0x89は0件。
- pitch 36: positive velocity Note On 148件、velocity-zero Note On 148件、合計296件。
- 既存ValleyBell実測pitch 36: 706件（positive 148、velocity-zero 558）。差410件。
- 既存ValleyBellモデルでは、source pitch36 velocity-zero records 206件のうちduration nonzeroは206件、同一pitch running-note抑止で出力されない1件を除く205件がNote Onとして出力される。各Note Onの自動終了を含めると205×2=410件。
- channel 9全体でも、rcm2smf 922件に対しValleyBell 1,840件。差918件はValleyBellモデルのsource velocity-zero Note On 459件とその自動終了459件の合計に一致。

### 解釈と制限

数値の一致から、rcm2smfはsource velocity-zero recordsをValleyBellのような独立Note Onとして出力していない可能性が高い。今回の差分を単なるファイル破損や転送欠落で説明する必要はない。一方、rcm2smfのソースコード／仕様に基づく処理経路は未確認であり、velocity-zero recordを無視する仕様なのか、Note Off相当として扱う仕様なのか、または不具合なのかは未確定。

今回添付物は以前の取得時に想定していた31,013 bytesではなく22,878 bytes。添付物自体はSMFとして完全に解析できたが、private repo上の保存ファイルと同一かはhash照合が必要。S-MU2000ソース変更、ビルド、CI、実機再生は未実施。


## 訂正 — private repo保存物との同一性確認（2026-11-01）

- 先行記載の「31,013 bytes」は誤り。GitHub fetch_fileが返したBase64文字列の文字数31,013を、ファイルbyte数と取り違えていた。
- private repoのG36_DATA/SAKURA/rcm2smf/SAKURA.midのGit blob SHAはe2163caadb82f96a652a804f96516a115914f532。添付ファイルから計算したGit blob SHAも同一であり、両者は完全に同一ファイル。
- 正しいファイルサイズは22,878 bytes。したがって添付物とprivate repo保存物の同一性は確認済みであり、再アップロードや再コンバートは不要。


## 追記 — rcm2smf実装コードでvelocity 0の扱いを確認（2026-11-01）

公開リポジトリshingo45endo/rcm2smfのrcm_converter.jsを確認した。

- RCP/G36のノートイベント処理は、channelが有効でgt>0かつvel>0の場合にだけNote Onを生成する。さらにvelocity値のmask後も0ならNote Onを生成しない。したがってsource velocity-zero note recordsは、独立したNote OnとしてSMFへ出力されない。
- ノート長はnoteGtsで管理し、duration満了時にmakeNoteOffで終了イベントを生成する。つまり今回のrcm2smf版にあるvelocity-zero MIDI eventsは、元sourceのvelocity-zero recordsではなく、positive-velocity notesの自動終了イベント。
- makeNoteOffはsettings.noteOff=trueならstatus 0x8n、falseならstatus 0x9n velocity 0を生成する。今回のSMFはstatus 0x99のvelocity-zero eventsで、status 0x89は0件なので、Note Off optionは無効（velocity-zero Note On形式）で出力されたと判断できる。
- この実装上の挙動は、今回の実測（channel 9: positive 461 + generated velocity-zero endings 461、pitch 36: 148 + 148）と一致する。
- ValleyBell版はsource cmdP2をvelocityとしてNote On出力へ渡し、velocity 0でもNote Onを抑止しないため、source velocity-zero recordsがSMFイベントとして増える。今回の差410 / 918はこの挙動差で説明できる。
- これで差分の直接原因は実装コードレベルで確認できた。ただし、どちらがG36/RCP仕様として正しいか、S-MU2000がどちらに合わせるべきかは、公式仕様・実機挙動・サンプルの意味を確認して決める。現時点でrcm2smfを不具合と断定しない。
- 参照: https://github.com/shingo45endo/rcm2smf/blob/master/rcm_converter.js （ノートイベント処理、noteGtsによる終了処理、makeNoteOff）
