/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "sharedutils/asset_loader/asset_format_handler.hpp"

util::IAssetFormatHandler::IAssetFormatHandler(util::IAssetManager &assetManager) : m_assetManager {assetManager} {}
