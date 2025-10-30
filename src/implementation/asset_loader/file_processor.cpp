// SPDX-FileCopyrightText: (c) 2021 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;



module pragma.util;

import :asset_loader.file_processor;

util::FileAssetProcessor::FileAssetProcessor(util::AssetFormatLoader &loader, std::unique_ptr<util::IAssetFormatHandler> &&handler) : handler {std::move(handler)}, m_loader {loader} {}

void util::FileAssetProcessor::Close()
{
	// We need to close the file handle as soon as possible to prevent congestion problems (e.g. EMFILE "Too many open files")
	if(handler)
		handler->Close();
}

std::optional<std::string> util::FileAssetProcessor::GetErrorMessage() const { return handler->GetErrorMessage(); }
