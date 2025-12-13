// SPDX-FileCopyrightText: (c) 2021 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

module pragma.util;

import :asset_loader.file_processor;

pragma::util::FileAssetProcessor::FileAssetProcessor(AssetFormatLoader &loader, std::unique_ptr<IAssetFormatHandler> &&handler) : handler {std::move(handler)}, m_loader {loader} {}

void pragma::util::FileAssetProcessor::Close()
{
	// We need to close the file handle as soon as possible to prevent congestion problems (e.g. EMFILE "Too many open files")
	if(handler)
		handler->Close();
}

std::optional<std::string> pragma::util::FileAssetProcessor::GetErrorMessage() const { return handler->GetErrorMessage(); }
