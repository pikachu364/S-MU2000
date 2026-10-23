// license:BSD-3-Clause
//
// RCP2 の固定ヘッダーと未解釈シーケンスレコードを安全に読み取る。
// コマンド意味論やイベント時間の展開は別段階で実装する。

#include "rcp.h"

#include <algorithm>
#include <cstring>
#include <vector>

namespace rcp {
namespace {

// RCP2 (RCP-PC98 V2.0) の識別文字列。ヘッダー先頭の 32 バイト中、
// 先頭 30 バイトが文字列＋CRLF、残り 2 バイトは NUL。
// レイアウトは ValleyBell/MidiConverters の RCPFormat.txt を参考にした。
constexpr char RCP2_SIGNATURE[] = "RCM-PC98V2.0(C)COME ON MUSIC\r\n";
constexpr size_t RCP2_SIGNATURE_SIZE = sizeof(RCP2_SIGNATURE) - 1;
constexpr size_t RCP2_PREFIX_SIZE = 0x20;
constexpr size_t RCP2_HEADER_SIZE = 0x586;
constexpr size_t RCP2_TRACK_HEADER_SIZE = 0x2c;

std::string fixed_string(const u8 *data, size_t size)
{
	while (size > 0 && (data[size - 1] == 0 || data[size - 1] == ' '))
		--size;
	return std::string(reinterpret_cast<const char *>(data), size);
}

bool signature_matches(const u8 *data, size_t size)
{
	return data != nullptr && size >= RCP2_PREFIX_SIZE &&
	       std::memcmp(data, RCP2_SIGNATURE, RCP2_SIGNATURE_SIZE) == 0 &&
	       data[30] == 0 && data[31] == 0;
}

} // namespace

bool inspect_header(const u8 *data, size_t size, header_info &out, std::string &err)
{
	out = {};
	err.clear();

	if (data == nullptr) {
		err = "RCP データが null です";
		return false;
	}
	if (size < RCP2_PREFIX_SIZE) {
		err = "RCP ヘッダーが短すぎます";
		return false;
	}
	if (!signature_matches(data, size)) {
		err = "RCP2 の識別文字列が一致しません";
		return false;
	}

	out.type = format::rcp2;
	out.minimum_header_size = RCP2_PREFIX_SIZE;
	return true;
}

bool parse_header(const u8 *data, size_t size, document_header &out, std::string &err)
{
	out = {};
	err.clear();

	header_info info;
	if (!inspect_header(data, size, info, err))
		return false;
	if (size < RCP2_HEADER_SIZE) {
		err = "RCP2 固定ヘッダー全体がありません";
		return false;
	}

	// RCPFormat.txt の Header v2 [RCP] オフセット表に基づく。
	out.title = fixed_string(data + 0x020, 0x040);
	out.comment = fixed_string(data + 0x060, 0x150);
	out.ticks_per_quarter = static_cast<u16>(data[0x1c0]) |
	                        (static_cast<u16>(data[0x1e7]) << 8);
	out.tempo_bpm = data[0x1c1];
	out.beat_numerator = data[0x1c2];
	out.beat_denominator = data[0x1c3];
	out.key_signature = data[0x1c4];
	out.play_bias = data[0x1c5];
	out.track_count = data[0x1e6];
	return true;
}

bool parse_track_header(const u8 *data, size_t size, track_header &out, std::string &err)
{
	out = {};
	err.clear();

	if (data == nullptr) {
		err = "RCP トラックヘッダーが null です";
		return false;
	}
	if (size < RCP2_TRACK_HEADER_SIZE) {
		err = "RCP トラックヘッダーが短すぎます";
		return false;
	}

	out.encoded_length = static_cast<u16>(data[0]) |
	                     (static_cast<u16>(data[1]) << 8);
	out.track_id = data[2];
	out.rhythm_mode = data[3];
	out.midi_channel = data[4];
	out.key = data[5];
	out.tick_offset = data[6];
	out.mute = data[7];
	out.name = fixed_string(data + 8, 0x24);
	return true;
}

bool parse_sequence(const u8 *data, size_t size,
                    std::vector<sequence_event> &out,
                    size_t &bytes_consumed, std::string &err)
{
	out.clear();
	bytes_consumed = 0;
	err.clear();

	if (data == nullptr) {
		err = "RCP シーケンスデータが null です";
		return false;
	}

	// Only inspect complete records. Avoid arithmetic that can overflow.
	const size_t record_count = size / 4;
	for (size_t i = 0; i < record_count; ++i) {
		const size_t offset = i * 4;
		sequence_event event;
		event.code = data[offset];
		event.p0 = data[offset + 1];
		event.p1 = data[offset + 2];
		event.p2 = data[offset + 3];
		out.push_back(event);
		bytes_consumed = offset + 4;

		if (event.code == 0xfe)
			return true;
	}

	out.clear();
	bytes_consumed = 0;
	if (size % 4 != 0)
		err = "RCP シーケンスの末尾が 4 byte レコード境界に一致しません";
	else
		err = "RCP シーケンスに Track End (FE) がありません";
	return false;
}


bool extract_comment_data(const std::vector<sequence_event> &sequence,
                          size_t start_index, std::vector<u8> &out,
                          size_t &records_consumed, std::string &err)
{
    out.clear();
    records_consumed = 0;
    err.clear();

    if (start_index >= sequence.size() || sequence[start_index].code != 0xf6) {
        err = "RCP コメント抽出の開始位置が F6 ではありません";
        return false;
    }

    // Require a track terminator in the supplied sequence before interpreting
    // the record chain. parse_sequence normally guarantees this, but callers
    // can construct the intermediate vector directly.
    bool found_end = false;
    for (size_t i = start_index; i < sequence.size(); ++i) {
        if (sequence[i].code == 0xfe) {
            found_end = true;
            break;
        }
    }
    if (!found_end) {
        err = "RCP コメント抽出の入力に Track End (FE) がありません";
        return false;
    }

    for (size_t i = start_index; i < sequence.size(); ++i) {
        const sequence_event &event = sequence[i];
        if (i != start_index && event.code != 0xf7)
            break;
        if (event.code == 0xfe)
            break;

        out.push_back(event.p1);
        out.push_back(event.p2);
        ++records_consumed;
    }
    return true;
}


size_t trim_comment_trailing_spaces(std::vector<u8> &data)
{
    while (!data.empty() && data.back() == 0x20)
        data.pop_back();
    return data.size();
}


bool decode_key_signature_event(const sequence_event &raw,
                                key_signature_event &out, std::string &err)
{
    out = {};
    err.clear();

    if (raw.code != 0xf5) {
        err = "RCP レコードは F5 キー署名コマンドではありません";
        return false;
    }

    // Mirrors the referenced converter's mapping: bit 3 selects flats,
    // low three bits give the count, and bit 4 selects minor.
    const int count = raw.p0 & 0x07;
    out.accidentals = (raw.p0 & 0x08) ? -count : count;
    out.minor = (raw.p0 & 0x10) != 0;
    // In the reference handler p0 is consumed as key data and then cleared,
    // so it does not advance the event timeline as a delay.
    out.delay_advance = 0;
    return true;
}


bool decode_note_event(const sequence_event &raw, note_event &out, std::string &err)
{
    out = {};
    err.clear();

    if (raw.code >= 0x80) {
        err = "RCP レコードはノートではなくコマンドです";
        return false;
    }

    out.note = raw.code;
    out.delay = raw.p0;
    out.duration = raw.p1;
    out.velocity = raw.p2;
    return true;
}


bool expand_note_only_timeline(const std::vector<sequence_event> &sequence,
                               std::vector<timed_note_event> &out,
                               std::string &err)
{
    out.clear();
    err.clear();

    u32 current_tick = 0;
    bool found_end = false;
    for (const sequence_event &raw : sequence) {
        if (raw.code == 0xfe) {
            found_end = true;
            break;
        }
        if (raw.code >= 0x80) {
            out.clear();
            err = "ノート専用時間展開では未対応コマンドを扱えません";
            return false;
        }

        note_event note;
        if (!decode_note_event(raw, note, err)) {
            out.clear();
            return false;
        }
        timed_note_event timed;
        timed.start_tick = current_tick;
        timed.note = note;
        out.push_back(timed);

        if (note.delay > 0xffffffffu - current_tick) {
            out.clear();
            err = "RCP ノートの tick が 32 bit 範囲を超えます";
            return false;
        }
        current_tick += note.delay;
    }

    if (!found_end) {
        out.clear();
        err = "ノート専用時間展開に Track End (FE) がありません";
        return false;
    }
    return true;
}



bool decode_tempo_event(const sequence_event &raw, u16 bpm,
                        tempo_event &out, std::string &err)
{
    out = tempo_event{};
    err.clear();

    if (raw.code != 0xe7) {
        err = "テンポイベントのコマンドが E7 ではありません";
        return false;
    }

    u32 tempo = 0;
    if (!rcp2_tempo_us_per_quarter(bpm, raw.p1, tempo, err))
        return false;

    out.delay = raw.p0;
    out.scale = raw.p1;
    out.gradual = raw.p2;
    out.microseconds_per_quarter = tempo;
    return true;
}


bool rcp2_tempo_us_per_quarter(u16 bpm, u8 scale, u32 &out, std::string &err)
{
    out = 0;
    err.clear();

    if (bpm == 0 || scale == 0) {
        err = "RCP2 テンポ計算では BPM と scale に 0 を指定できません";
        return false;
    }

    // Match ValleyBell/MidiConverters' Tempo2Mid integer formula without rounding:
    // 60,000,000 * 64 / (BPM * scale). Use u64 for the numerator.
    const u64 denominator = static_cast<u64>(bpm) * static_cast<u64>(scale);
    out = static_cast<u32>((60000000ull * 64ull) / denominator);
    return true;
}


bool expand_note_key_signature_timeline(const std::vector<sequence_event> &sequence,
                                        std::vector<timed_event> &out,
                                        std::string &err)
{
    out.clear();
    err.clear();

    u32 current_tick = 0;
    bool found_end = false;
    for (const sequence_event &raw : sequence) {
        if (raw.code == 0xfe) {
            found_end = true;
            break;
        }

        timed_event timed;
        timed.tick = current_tick;
        if (raw.code == 0xf5) {
            if (!decode_key_signature_event(raw, timed.key, err)) {
                out.clear();
                return false;
            }
            timed.kind = timeline_event_kind::key_signature;
            out.push_back(timed);
            // F5 p0 is key-signature data in the reference implementation.
            continue;
        }

        if (raw.code >= 0x80) {
            out.clear();
            err = "時間展開では F5 以外の未対応コマンドを扱えません";
            return false;
        }

        if (!decode_note_event(raw, timed.note, err)) {
            out.clear();
            return false;
        }
        timed.kind = timeline_event_kind::note;
        out.push_back(timed);

        if (timed.note.delay > 0xffffffffu - current_tick) {
            out.clear();
            err = "RCP イベントの tick が 32 bit 範囲を超えます";
            return false;
        }
        current_tick += timed.note.delay;
    }

    if (!found_end) {
        out.clear();
        err = "時間展開に Track End (FE) がありません";
        return false;
    }
    return true;
}



bool decode_midi_channel_event(const sequence_event &raw,
                              midi_channel_event &out,
                              timeline_event_kind &kind,
                              std::string &err)
{
    out = {};
    kind = timeline_event_kind::note;
    err.clear();

    if (raw.code == 0xea) {
        if (raw.p1 > 0x7f) {
            err = "RCP EA Channel Aftertouch の値が 7 bit 範囲外です";
            return false;
        }
        out.delay = raw.p0;
        out.data1 = raw.p1;
        out.data2 = 0;
        kind = timeline_event_kind::channel_aftertouch;
        return true;
    }

    if (raw.code == 0xeb) {
        if (raw.p1 > 0x7f || raw.p2 > 0x7f) {
            err = "RCP EB Control Change のデータ値が 7 bit 範囲外です";
            return false;
        }
        out.delay = raw.p0;
        out.data1 = raw.p1;
        out.data2 = raw.p2;
        kind = timeline_event_kind::control_change;
        return true;
    }

    if (raw.code == 0xec) {
        if (raw.p1 >= 0x80) {
            err = "RCP EC の特殊インストゥルメント値は未対応です";
            return false;
        }
        out.delay = raw.p0;
        out.data1 = raw.p1;
        out.data2 = 0;
        kind = timeline_event_kind::program_change;
        return true;
    }

    if (raw.code == 0xed) {
        if (raw.p1 > 0x7f || raw.p2 > 0x7f) {
            err = "RCP ED Polyphonic Aftertouch の値が 7 bit 範囲外です";
            return false;
        }
        out.delay = raw.p0;
        out.data1 = raw.p1;
        out.data2 = raw.p2;
        kind = timeline_event_kind::poly_aftertouch;
        return true;
    }

    if (raw.code == 0xee) {
        if (raw.p1 > 0x7f || raw.p2 > 0x7f) {
            err = "RCP EE Pitch Bend のデータ値が 7 bit 範囲外です";
            return false;
        }
        out.delay = raw.p0;
        out.data1 = raw.p1;
        out.data2 = raw.p2;
        kind = timeline_event_kind::pitch_bend;
        return true;
    }

    err = "RCP レコードは EA/EB/EC/ED/EE MIDI チャンネルコマンドではありません";
    return false;
}


bool encode_midi_message(const timed_event &event, u8 channel,
                         std::vector<u8> &out, std::string &err)
{
    out.clear();
    err.clear();
    if (channel > 0x0f) {
        err = "MIDI チャンネルが 0..15 の範囲外です";
        return false;
    }

    switch (event.kind) {
    case timeline_event_kind::note:
        if (event.note.note > 0x7f || event.note.velocity > 0x7f) {
            err = "MIDI ノート番号またはベロシティが 7 bit 範囲外です";
            return false;
        }
        out = {u8(0x90 | channel), event.note.note, event.note.velocity};
        return true;
    case timeline_event_kind::note_off:
        if (event.note.note > 0x7f) {
            err = "MIDI ノートオフのノート番号が 7 bit 範囲外です";
            return false;
        }
        out = {u8(0x80 | channel), event.note.note, 0};
        return true;
    case timeline_event_kind::control_change:
        if (event.channel.data1 > 0x7f || event.channel.data2 > 0x7f) {
            err = "MIDI Control Change のデータ値が 7 bit 範囲外です";
            return false;
        }
        out = {u8(0xb0 | channel), event.channel.data1, event.channel.data2};
        return true;
    case timeline_event_kind::program_change:
        if (event.channel.data1 > 0x7f) {
            err = "MIDI Program Change のデータ値が 7 bit 範囲外です";
            return false;
        }
        out = {u8(0xc0 | channel), event.channel.data1};
        return true;
    case timeline_event_kind::channel_aftertouch:
        if (event.channel.data1 > 0x7f) {
            err = "MIDI Channel Aftertouch の値が 7 bit 範囲外です";
            return false;
        }
        out = {u8(0xd0 | channel), event.channel.data1};
        return true;
    case timeline_event_kind::poly_aftertouch:
        if (event.channel.data1 > 0x7f || event.channel.data2 > 0x7f) {
            err = "MIDI Polyphonic Aftertouch の値が 7 bit 範囲外です";
            return false;
        }
        out = {u8(0xa0 | channel), event.channel.data1, event.channel.data2};
        return true;
    case timeline_event_kind::pitch_bend:
        if (event.channel.data1 > 0x7f || event.channel.data2 > 0x7f) {
            err = "MIDI Pitch Bend のデータ値が 7 bit 範囲外です";
            return false;
        }
        out = {u8(0xe0 | channel), event.channel.data1, event.channel.data2};
        return true;
    case timeline_event_kind::key_signature:
    case timeline_event_kind::tempo:
        err = "MIDI メタデータイベントはチャンネルメッセージではありません";
        return false;
    }
    err = "未対応の MIDI イベント種別です";
    return false;
}


bool expand_note_key_signature_tempo_timeline(
    const std::vector<sequence_event> &sequence, u16 bpm,
    std::vector<timed_event> &out, std::string &err)
{
    out.clear();
    err.clear();

    u32 current_tick = 0;
    bool found_end = false;
    for (const sequence_event &raw : sequence) {
        if (raw.code == 0xfe) {
            found_end = true;
            break;
        }

        timed_event timed;
        timed.tick = current_tick;
        u32 delay_advance = 0;

        if (raw.code == 0xf5) {
            if (!decode_key_signature_event(raw, timed.key, err)) {
                out.clear();
                return false;
            }
            timed.kind = timeline_event_kind::key_signature;
            delay_advance = timed.key.delay_advance;
        } else if (raw.code == 0xe7) {
            if (!decode_tempo_event(raw, bpm, timed.tempo, err)) {
                out.clear();
                return false;
            }
            timed.kind = timeline_event_kind::tempo;
            delay_advance = timed.tempo.delay;
        } else if (raw.code == 0xea || raw.code == 0xeb || raw.code == 0xec || raw.code == 0xed || raw.code == 0xee) {
            if (!decode_midi_channel_event(raw, timed.channel, timed.kind, err)) {
                out.clear();
                return false;
            }
            delay_advance = timed.channel.delay;
        } else if (raw.code < 0x80) {
            if (!decode_note_event(raw, timed.note, err)) {
                out.clear();
                return false;
            }
            timed.kind = timeline_event_kind::note;
            delay_advance = timed.note.delay;
        } else {
            out.clear();
            err = "時間展開ではノート・EA・EB・EC・ED・EE・F5・E7 以外のコマンドを扱えません";
            return false;
        }

        out.push_back(timed);
        if (delay_advance > 0xffffffffu - current_tick) {
            out.clear();
            err = "RCP イベントの tick が 32 bit 範囲を超えます";
            return false;
        }
        current_tick += delay_advance;
    }

    if (!found_end) {
        out.clear();
        err = "時間展開に Track End (FE) がありません";
        return false;
    }
    return true;
}


bool merge_timed_tracks(const std::vector<std::vector<timed_event>> &tracks,
                        std::vector<timed_track_event> &out, std::string &err)
{
    out.clear();
    err.clear();
    for (const auto &track_events : tracks) {
        for (size_t i = 1; i < track_events.size(); ++i) {
            if (track_events[i].tick < track_events[i - 1].tick) {
                err = "RCP トラック内のイベントが tick 順ではありません";
                return false;
            }
        }
    }
    std::vector<size_t> positions(tracks.size(), 0);
    size_t remaining = 0;
    for (const auto &track_events : tracks)
        remaining += track_events.size();
    out.reserve(remaining);
    while (remaining != 0) {
        size_t selected = tracks.size();
        u32 selected_tick = 0;
        for (size_t i = 0; i < tracks.size(); ++i) {
            if (positions[i] >= tracks[i].size())
                continue;
            const u32 tick = tracks[i][positions[i]].tick;
            if (selected == tracks.size() || tick < selected_tick) {
                selected = i;
                selected_tick = tick;
            }
        }
        if (selected == tracks.size()) {
            out.clear();
            err = "RCP トラック統合で内部状態が不正です";
            return false;
        }
        out.push_back({selected, tracks[selected][positions[selected]++]});
        --remaining;
    }
    return true;
}


bool parse_tracks(const u8 *data, size_t size,
                  std::vector<track> &out, std::string &err)
{
    out.clear();
    err.clear();

    document_header header;
    if (!parse_header(data, size, header, err))
        return false;

    size_t offset = RCP2_HEADER_SIZE;
    for (size_t i = 0; i < header.track_count; ++i) {
        if (offset > size || size - offset < RCP2_TRACK_HEADER_SIZE) {
            out.clear();
            err = "RCP2 トラックヘッダーがファイル末尾を越えています";
            return false;
        }

        track parsed;
        if (!parse_track_header(data + offset, size - offset, parsed.header, err)) {
            out.clear();
            return false;
        }

        const size_t track_length = parsed.header.encoded_length;
        if (track_length < RCP2_TRACK_HEADER_SIZE) {
            out.clear();
            err = "RCP2 トラック長がトラックヘッダーより短いです";
            return false;
        }
        if (track_length > size - offset) {
            out.clear();
            err = "RCP2 トラック長がファイル末尾を越えています";
            return false;
        }

        const size_t sequence_size = track_length - RCP2_TRACK_HEADER_SIZE;
        size_t consumed = 0;
        if (!parse_sequence(data + offset + RCP2_TRACK_HEADER_SIZE,
                            sequence_size, parsed.sequence, consumed, err)) {
            out.clear();
            err = "RCP2 トラック " + std::to_string(i + 1) +
                  " のシーケンスが不正です: " + err;
            return false;
        }

        // parse_sequence stops at FE. The encoded track boundary is authoritative;
        // bytes after FE remain unparsed padding/unknown data.
        out.push_back(parsed);
        offset += track_length;
    }
    return true;
}


bool build_intermediate(const u8 *data, size_t size,
                        intermediate_document &out, std::string &err)
{
    out = {};
    err.clear();

    if (!parse_header(data, size, out.header, err))
        return false;

    std::vector<track> parsed_tracks;
    if (!parse_tracks(data, size, parsed_tracks, err)) {
        out = {};
        return false;
    }

    out.tracks.reserve(parsed_tracks.size());
    for (const track &source : parsed_tracks) {
        intermediate_track target;
        target.header = source.header;
        target.events.reserve(source.sequence.size());
        for (size_t i = 0; i < source.sequence.size(); ++i) {
            intermediate_event event;
            event.record_index = i;
            event.raw = source.sequence[i];
            target.events.push_back(event);
        }
        out.tracks.push_back(target);
    }
    return true;
}

bool apply_note_transposition(u8 note, u8 raw_track_transposition,
                              u8 raw_document_transposition, u8 &out,
                              std::string &err)
{
    out = 0;
    err.clear();
    if (note > 0x7f) {
        err = "RCP ノート番号が 7 bit 範囲外です";
        return false;
    }

    int transpose = 0;
    if ((raw_track_transposition & 0x80) == 0) {
        const int track_value = raw_track_transposition & 0x7f;
        const int track_offset = (track_value & 0x40) ? track_value - 0x80 : track_value;
        const int document_offset = raw_document_transposition < 0x80
            ? raw_document_transposition : int(raw_document_transposition) - 0x100;
        transpose = track_offset + document_offset;
    }
    int adjusted_note = (int(note) + transpose) % 128;
    if (adjusted_note < 0)
        adjusted_note += 128;
    out = static_cast<u8>(adjusted_note);
    return true;
}


bool apply_signed_track_offset(u32 tick, u8 raw_offset, u32 &out, std::string &err)
{
    out = 0;
    err.clear();

    const int tick_offset = raw_offset < 0x80 ? raw_offset : int(raw_offset) - 0x100;
    const long long adjusted = static_cast<long long>(tick) + tick_offset;
    if (adjusted > 0xffffffffll) {
        err = "RCP トラック開始オフセット適用後の tick が 32 bit 範囲を超えます";
        return false;
    }

    out = adjusted < 0 ? 0u : static_cast<u32>(adjusted);
    return true;
}


namespace {

bool expand_note_gates(const std::vector<timed_event> &timeline,
                       std::vector<timed_event> &out, std::string &err)
{
    struct running_note { u8 pitch; size_t note_off_index; u32 end_tick; };
    out.clear();
    err.clear();
    std::vector<timed_event> expanded;
    std::vector<running_note> running;
    expanded.reserve(timeline.size());

    for (const timed_event &event : timeline) {
        if (event.kind != timeline_event_kind::note) {
            expanded.push_back(event);
            continue;
        }
        for (size_t i = 0; i < running.size();) {
            if (running[i].end_tick <= event.tick)
                running.erase(running.begin() + i);
            else
                ++i;
        }
        if (event.note.duration > 0xffffffffu - event.tick) {
            out.clear();
            err = "RCP ノートオフの tick が 32 bit 範囲を超えます";
            return false;
        }
        const u32 new_end_tick = event.tick + event.note.duration;
        size_t active_index = running.size();
        for (size_t i = 0; i < running.size(); ++i) {
            if (running[i].pitch == event.note.note) { active_index = i; break; }
        }
        if (active_index != running.size()) {
            running_note &active = running[active_index];
            expanded[active.note_off_index].tick = new_end_tick;
            active.end_tick = new_end_tick;
            if (event.note.duration == 0)
                running.erase(running.begin() + active_index);
            continue;
        }
        if (event.note.duration == 0)
            continue;

        expanded.push_back(event);
        timed_event note_off;
        note_off.tick = new_end_tick;
        note_off.kind = timeline_event_kind::note_off;
        note_off.note = event.note;
        note_off.note.delay = 0;
        note_off.note.duration = 0;
        note_off.note.velocity = 0;
        const size_t note_off_index = expanded.size();
        expanded.push_back(note_off);
        running.push_back({event.note.note, note_off_index, new_end_tick});
    }
    std::stable_sort(expanded.begin(), expanded.end(),
        [](const timed_event &a, const timed_event &b) {
            if (a.tick != b.tick) return a.tick < b.tick;
            const bool a_off = a.kind == timeline_event_kind::note_off;
            const bool b_off = b.kind == timeline_event_kind::note_off;
            return a_off && !b_off;
        });
    out.swap(expanded);
    return true;
}

} // namespace

bool expand_intermediate_timeline(const intermediate_document &document,
                                  std::vector<timed_track_event> &out,
                                  std::string &err)
{
    out.clear();
    err.clear();
    std::vector<std::vector<timed_event>> timelines;
    timelines.reserve(document.tracks.size());
    for (size_t i = 0; i < document.tracks.size(); ++i) {
        // The referenced RCP2 layout assigns bit 0 of the mute byte to track mute.
        // Keep the track slot so merged events retain their original track index.
        if (document.tracks[i].header.mute & 0x01) {
            timelines.emplace_back();
            continue;
        }
        std::vector<sequence_event> sequence;
        sequence.reserve(document.tracks[i].events.size());
        for (const intermediate_event &event : document.tracks[i].events)
            sequence.push_back(event.raw);
        std::vector<timed_event> timeline;
        if (!expand_note_key_signature_tempo_timeline(
                sequence, document.header.tempo_bpm, timeline, err)) {
            out.clear();
            err = "RCP2 トラック " + std::to_string(i + 1) +
                  " の時間展開に失敗しました: " + err;
            return false;
        }
        for (timed_event &event : timeline) {
            if (event.kind != timeline_event_kind::note) continue;
            u8 adjusted_note = 0;
            if (!apply_note_transposition(event.note.note,
                                          document.tracks[i].header.key,
                                          document.header.play_bias,
                                          adjusted_note, err)) {
                out.clear();
                return false;
            }
            event.note.note = adjusted_note;
        }
        std::vector<timed_event> gated_timeline;
        if (!expand_note_gates(timeline, gated_timeline, err)) {
            out.clear();
            err = "RCP2 トラック " + std::to_string(i + 1) +
                  " のノート長展開に失敗しました: " + err;
            return false;
        }
        for (timed_event &event : gated_timeline) {
            u32 adjusted_tick = 0;
            if (!apply_signed_track_offset(event.tick,
                                           document.tracks[i].header.tick_offset,
                                           adjusted_tick, err)) {
                out.clear();
                return false;
            }
            event.tick = adjusted_tick;
        }
        timelines.push_back(std::move(gated_timeline));
    }
    if (!merge_timed_tracks(timelines, out, err)) {
        out.clear();
        return false;
    }
    return true;
}

bool convert_timeline_to_seconds(const document_header &header,
                                 const std::vector<timed_track_event> &timeline,
                                 std::vector<timed_seconds_event> &out,
                                 std::string &err)
{
    out.clear();
    err.clear();
    if (header.ticks_per_quarter == 0) {
        err = "RCP2 tick/秒変換では ticks_per_quarter が 0 です";
        return false;
    }
    if (header.tempo_bpm == 0) {
        err = "RCP2 tick/秒変換では初期 BPM が 0 です";
        return false;
    }

    u64 tempo_us = 60000000ull / header.tempo_bpm;
    u32 previous_tick = 0;
    double seconds = 0.0;
    out.reserve(timeline.size());
    for (const timed_track_event &source : timeline) {
        if (source.event.tick < previous_tick) {
            out.clear();
            err = "RCP2 秒変換の入力イベントが tick 順ではありません";
            return false;
        }
        const u32 delta = source.event.tick - previous_tick;
        seconds += (double(delta) * double(tempo_us)) /
                   (double(header.ticks_per_quarter) * 1000000.0);
        previous_tick = source.event.tick;

        timed_seconds_event converted;
        converted.time = seconds;
        converted.source = source;
        out.push_back(converted);

        if (source.event.kind == timeline_event_kind::tempo) {
            if (source.event.tempo.microseconds_per_quarter == 0) {
                out.clear();
                err = "RCP2 秒変換で 0 microseconds_per_quarter のテンポを検出しました";
                return false;
            }
            tempo_us = source.event.tempo.microseconds_per_quarter;
        }
    }
    return true;
}


bool map_track_midi_outputs(const intermediate_document &document,
                            std::vector<track_midi_mapping> &out, std::string &err)
{
    out.clear();
    err.clear();
    out.reserve(document.tracks.size());
    for (const intermediate_track &track : document.tracks) {
        const u8 raw = track.header.midi_channel;
        track_midi_mapping mapping;
        if (raw & 0x80) {
            // The reference converter treats this as a dummy channel and omits
            // its MIDI events instead of routing them to channel zero.
            mapping.enabled = false;
        } else {
            mapping.channel = raw & 0x0f;
            mapping.port = raw >> 4;
        }
        out.push_back(mapping);
    }
    return true;
}

bool encode_timeline_to_midi_events(
    const std::vector<timed_seconds_event> &timeline,
    const std::vector<track_midi_mapping> &mapping,
    std::vector<midi_output_event> &out, std::string &err)
{
    out.clear();
    err.clear();
    double previous_time = 0.0;
    out.reserve(timeline.size());
    for (const timed_seconds_event &source : timeline) {
        if (!(source.time >= 0.0) || source.time < previous_time) {
            out.clear(); err = "RCP2 MIDI 出力イベントの時刻が負値または逆順です"; return false;
        }
        previous_time = source.time;
        const auto kind = source.source.event.kind;
        if (kind == timeline_event_kind::tempo || kind == timeline_event_kind::key_signature)
            continue;
        const size_t track = source.source.track_index;
        if (track >= mapping.size()) {
            out.clear(); err = "RCP2 MIDI 出力のトラック割り当てが不足しています"; return false;
        }
        if (!mapping[track].enabled)
            continue;
        std::vector<u8> bytes;
        if (!encode_midi_message(source.source.event, mapping[track].channel, bytes, err)) {
            out.clear();
            err = "RCP2 トラック " + std::to_string(track + 1) + " の MIDI 符号化失敗: " + err;
            return false;
        }
        out.push_back({source.time, std::move(bytes), mapping[track].port});
    }
    return true;
}

bool encode_timeline_to_midi_events(
    const std::vector<timed_seconds_event> &timeline,
    const std::vector<u8> &channel_by_track, const std::vector<u8> &port_by_track,
    std::vector<midi_output_event> &out, std::string &err)
{
    out.clear(); err.clear();
    double previous_time = 0.0;
    out.reserve(timeline.size());
    for (const timed_seconds_event &source : timeline) {
        if (!(source.time >= 0.0) || source.time < previous_time) {
            out.clear(); err = "RCP2 MIDI 出力イベントの時刻が負値または逆順です"; return false;
        }
        previous_time = source.time;
        const auto kind = source.source.event.kind;
        if (kind == timeline_event_kind::tempo || kind == timeline_event_kind::key_signature)
            continue;
        const size_t track = source.source.track_index;
        if (track >= channel_by_track.size() || track >= port_by_track.size()) {
            out.clear(); err = "RCP2 MIDI 出力のトラック割り当てが不足しています"; return false;
        }
        std::vector<u8> bytes;
        if (!encode_midi_message(source.source.event, channel_by_track[track], bytes, err)) {
            out.clear();
            err = "RCP2 トラック " + std::to_string(track + 1) + " の MIDI 符号化失敗: " + err;
            return false;
        }
        out.push_back({source.time, std::move(bytes), port_by_track[track]});
    }
    return true;
}


} // namespace rcp
