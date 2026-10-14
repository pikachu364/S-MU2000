// license:BSD-3-Clause
//
// RCP2 固定ヘッダーの形式識別と読み取り。
// イベント解釈は仕様・実ファイルの照合後に追加する。

#include "rcp.h"

#include <cstring>

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

} // namespace rcp
