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
