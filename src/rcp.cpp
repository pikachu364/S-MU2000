// license:BSD-3-Clause
//
// RCP2 の固定ヘッダーと未解釈シーケンスレコードを安全に読み取る。
// コマンド意味論やイベント時間の展開は別段階で実装する。

#include "rcp.h"

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

} // namespace rcp
