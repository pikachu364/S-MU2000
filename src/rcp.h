// license:BSD-3-Clause
//
// Roland RCP シーケンサーファイルの解析入口。
// 現段階では RCP2 の固定ヘッダーまでを扱い、トラック／イベント解析はまだ行わない。

#ifndef S_MU2000_RCP_H
#define S_MU2000_RCP_H

#pragma once

#include "compat/mamecompat.h"

#include <cstddef>
#include <string>
#include <vector>

namespace rcp {

enum class format {
	unknown,
	rcp2,
};

struct header_info {
	format type = format::unknown;
	size_t minimum_header_size = 0;
};

// RCP ファイルの形式を識別する。成功してもイベント解析・再生を保証しない。
// data はファイル先頭、size はバイト数。
bool inspect_header(const u8 *data, size_t size, header_info &out, std::string &err);

// RCP2 固定ヘッダーを読む。文字列は文字コード変換せず、末尾の空白/NUL のみ除去する。
// 現段階ではトラックデータの存在やイベント列の正当性は検証しない。
struct document_header {
	std::string title;
	std::string comment;
	u16 ticks_per_quarter = 0;
	u8 tempo_bpm = 0;
	u8 beat_numerator = 0;
	u8 beat_denominator = 0;
	u8 key_signature = 0;
	u8 play_bias = 0; // 仕様の符号表現は未確定のため生バイトを保持
	u8 track_count = 0;
};

bool parse_header(const u8 *data, size_t size, document_header &out, std::string &err);

// RCP2 トラックヘッダー（0x2c bytes）。意味が確定していない値は生バイトで保持する。
// encoded_length はヘッダーを含む長さフィールドの格納値。拡張長の解釈は別途検証する。
struct track_header {
	u16 encoded_length = 0;
	u8 track_id = 0;
	u8 rhythm_mode = 0;
	u8 midi_channel = 0;
	u8 key = 0;
	u8 tick_offset = 0;
	u8 mute = 0;
	std::string name;
};

bool parse_track_header(const u8 *data, size_t size, track_header &out, std::string &err);

// RCP2 のシーケンスレコードを生データのまま読む。
// 各レコードは 4 bytes。FE (Track End) を含めて返し、FE までの消費 byte 数を返す。
// コマンドの意味・p0 の時間解釈・ループ展開はこの段階では行わない。
// FE がない、または途中でレコードが切れている入力は失敗とする。
struct sequence_event {
	u8 code = 0; // 00..7F: note height, 80..FF: command ID
	u8 p0 = 0;   // delay field; interpretation depends on command
	u8 p1 = 0;
	u8 p2 = 0;
};

bool parse_sequence(const u8 *data, size_t size,
                    std::vector<sequence_event> &out,
                    size_t &bytes_consumed, std::string &err);

} // namespace rcp

#endif // S_MU2000_RCP_H
