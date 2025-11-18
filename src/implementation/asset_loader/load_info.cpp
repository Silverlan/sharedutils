// SPDX-FileCopyrightText: (c) 2021 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

module pragma.util;

import :asset_loader.load_info;

util::AssetLoadInfo::AssetLoadInfo(AssetLoadFlags flags) : flags {flags} {}
