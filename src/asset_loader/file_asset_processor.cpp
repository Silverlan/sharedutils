/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "sharedutils/asset_loader/file_asset_processor.hpp"

util::FileAssetProcessor::FileAssetProcessor(util::AssetFormatLoader &loader,std::unique_ptr<util::IAssetFormatHandler> &&handler)
	: handler{std::move(handler)},m_loader{loader}
{}
