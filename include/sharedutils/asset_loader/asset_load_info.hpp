// SPDX-FileCopyrightText: (c) 2021 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

#ifndef __ASSET_LOAD_INFO_HPP__
#define __ASSET_LOAD_INFO_HPP__

#include "sharedutils/utildefinitions.h"
#include <mathutil/umath.h>
#include <cinttypes>
#include <functional>

namespace util {
	struct Asset;
	enum class AssetLoadFlags : uint32_t { None = 0u, AbsolutePath = 1u, DontCache = AbsolutePath << 1u, IgnoreCache = DontCache << 1u };
	struct DLLSHUTIL AssetLoadInfo {
		AssetLoadInfo(AssetLoadFlags flags = AssetLoadFlags::None);
		virtual ~AssetLoadInfo() = default;
		AssetLoadFlags flags = AssetLoadFlags::None;
		std::function<void(util::Asset &)> onLoaded = nullptr;
		std::function<void()> onFailure = nullptr;
	};
};
REGISTER_BASIC_BITWISE_OPERATORS(util::AssetLoadFlags)

#endif
