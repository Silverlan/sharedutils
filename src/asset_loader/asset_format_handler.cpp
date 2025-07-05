// SPDX-FileCopyrightText: (c) 2021 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

#include "sharedutils/asset_loader/asset_format_handler.hpp"

util::IAssetFormatHandler::IAssetFormatHandler(util::IAssetManager &assetManager) : m_assetManager {assetManager} {}
