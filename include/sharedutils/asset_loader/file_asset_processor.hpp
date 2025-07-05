// SPDX-FileCopyrightText: (c) 2021 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

#ifndef __FILE_ASSET_PROCESSOR_HPP__
#define __FILE_ASSET_PROCESSOR_HPP__

#include "sharedutils/asset_loader/asset_processor.hpp"
#include "sharedutils/asset_loader/asset_load_info.hpp"
#include "sharedutils/asset_loader/asset_format_handler.hpp"

namespace util {
	enum class AssetLoadResult : uint8_t;
	class AssetFormatLoader;
	class DLLSHUTIL FileAssetProcessor : public util::IAssetProcessor {
	  public:
		FileAssetProcessor(AssetFormatLoader &loader, std::unique_ptr<IAssetFormatHandler> &&handler);
		virtual void Close() override;
		virtual std::optional<std::string> GetErrorMessage() const override;
		std::unique_ptr<util::AssetLoadInfo> loadInfo = nullptr;
		std::function<void(util::Asset *, AssetLoadResult)> onLoaded = nullptr;

		std::unique_ptr<IAssetFormatHandler> handler;
	  protected:
		AssetFormatLoader &m_loader;
	};
};

#endif
