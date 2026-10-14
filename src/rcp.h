// license:BSD-3-Clause
//
// Roland RCP シーケンサーファイルの解析入口。
// 現段階では形式識別だけを行い、トラック／イベント解析はまだ行わない。

#ifndef S_MU2000_RCP_H
#define S_MU2000_RCP_H

#pragma once

#include "compat/mamecompat.h"

#include <cstddef>
#include <string>

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

} // namespace rcp

#endif // S_MU2000_RCP_H
