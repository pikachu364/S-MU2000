// license:BSD-3-Clause
//
// RCP ファイルの形式識別。イベントの解釈は仕様・実ファイルの照合後に追加する。

#include "rcp.h"

#include <cstring>

namespace rcp {
namespace {

// RCP2 (RCP-PC98 V2.0) の識別文字列。終端 NUL はファイルに含まれない。
// 既存コンバーターの形式判定と照合した値。これは全ヘッダー構造の確定を意味しない。
constexpr char RCP2_SIGNATURE[] = "RCM-PC98V2.0(C)COME ON MUSIC\r\n";
constexpr size_t RCP2_MINIMUM_HEADER_SIZE = 0x20;

} // namespace

bool inspect_header(const u8 *data, size_t size, header_info &out, std::string &err)
{
	out = {};
	err.clear();

	if (data == nullptr) {
		err = "RCP データが null です";
		return false;
	}
	if (size < RCP2_MINIMUM_HEADER_SIZE) {
		err = "RCP ヘッダーが短すぎます";
		return false;
	}
	if (std::memcmp(data, RCP2_SIGNATURE, sizeof(RCP2_SIGNATURE) - 1) != 0) {
		err = "RCP2 の識別文字列が一致しません";
		return false;
	}

	out.type = format::rcp2;
	out.minimum_header_size = RCP2_MINIMUM_HEADER_SIZE;
	return true;
}

} // namespace rcp
