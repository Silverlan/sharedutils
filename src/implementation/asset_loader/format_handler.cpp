// SPDX-FileCopyrightText: (c) 2021 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

module pragma.util;

import :asset_loader.format_handler;

pragma::util::IAssetFormatHandler::IAssetFormatHandler(IAssetManager &assetManager) : m_assetManager {assetManager} {}
