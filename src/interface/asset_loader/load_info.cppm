// SPDX-FileCopyrightText: (c) 2021 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include "definitions.hpp"

export module pragma.util:asset_loader.load_info;

export import pragma.math;

export {
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
		using namespace umath::scoped_enum::bitwise;
	};

	namespace umath::scoped_enum::bitwise {
		template<>
		struct enable_bitwise_operators<util::AssetLoadFlags> : std::true_type {};
	}
}
