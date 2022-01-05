/* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __ASSET_FORMAT_LOADER_HPP__
#define __ASSET_FORMAT_LOADER_HPP__

#include "sharedutils/asset_loader/asset_loader.hpp"
#include "sharedutils/asset_loader/asset_processor.hpp"
#include "sharedutils/util.h"

#undef AddJob

namespace util
{
	class IAssetFormatHandler;
	class IAssetManager;
	class DLLSHUTIL AssetFormatLoader
		: public util::IAssetLoader
	{
	public:
		AssetFormatLoader(util::IAssetManager &assetManager,std::string name="");
		// Registering a format handler is *not* MT-safe, all format handlers
		// should therefore be registered before any jobs are added.
		void RegisterFormatHandler(const std::string &ext,const std::function<std::unique_ptr<IAssetFormatHandler>(util::IAssetManager&)> &factory);
		std::optional<util::AssetLoadJobId> AddJob(
			const std::string &identifier,const std::string &ext,std::unique_ptr<ufile::IFile> &&file,util::AssetLoadJobPriority priority=0,
			const std::function<void(util::IAssetProcessor&)> &initProcessor=nullptr
		);
	protected:
		virtual std::unique_ptr<util::IAssetProcessor> CreateAssetProcessor(
			const std::string &identifier,const std::string &ext,std::unique_ptr<IAssetFormatHandler> &&formatHandler
		)=0;
		std::unordered_map<std::string,std::function<std::unique_ptr<IAssetFormatHandler>(util::IAssetManager&)>> m_formatHandlers;
		util::IAssetManager &m_assetManager;
	};

	template<typename TProcessor>
		class TAssetFormatLoader
			: public AssetFormatLoader
	{
	public:
		TAssetFormatLoader(util::IAssetManager &assetManager,std::string name="")
			: AssetFormatLoader{assetManager,std::move(name)}
		{}
	protected:
		virtual std::unique_ptr<util::IAssetProcessor> CreateAssetProcessor(
			const std::string &identifier,const std::string &ext,std::unique_ptr<IAssetFormatHandler> &&formatHandler
		) override
		{
			return util::static_unique_pointer_cast<TProcessor,util::IAssetProcessor>(std::make_unique<TProcessor>(*this,std::move(formatHandler)));
		}
	};
};

#endif
