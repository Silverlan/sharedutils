/* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

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
