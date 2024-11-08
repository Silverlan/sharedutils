/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "sharedutils/asset_loader/file_asset_processor.hpp"

util::FileAssetProcessor::FileAssetProcessor(util::AssetFormatLoader &loader, std::unique_ptr<util::IAssetFormatHandler> &&handler) : handler {std::move(handler)}, m_loader {loader} {}

void util::FileAssetProcessor::Close()
{
	// We need to close the file handle as soon as possible to prevent congestion problems (e.g. EMFILE "Too many open files")
	if(handler)
		handler->Close();
}

std::optional<std::string> util::FileAssetProcessor::GetErrorMessage() const { return handler->GetErrorMessage(); }
