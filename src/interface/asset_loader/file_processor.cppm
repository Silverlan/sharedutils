// SPDX-FileCopyrightText: (c) 2021 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include "definitions.hpp"



export module pragma.util:asset_loader.file_processor;

export import :asset_loader.format_handler;
export import :asset_loader.load_info;
export import :asset_loader.processor;

export namespace util {
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
