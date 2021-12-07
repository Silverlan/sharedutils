/* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __ASSET_MANAGER_HPP__
#define __ASSET_MANAGER_HPP__

#include "sharedutils/utildefinitions.h"
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <memory>

namespace util
{
	using AssetObject = std::shared_ptr<void>;
	struct DLLSHUTIL Asset
	{
	public:
		bool IsInUse() const;
		AssetObject assetObject;
	};
	class DLLSHUTIL IAssetManager
	{
	public:
		struct DLLSHUTIL AssetInfo
		{
			std::shared_ptr<Asset> asset;
			std::string identifier;
		};
		std::string ToCacheIdentifier(const std::string &assetName);

		IAssetManager();
		virtual ~IAssetManager()=default;

		uint32_t ClearUnused();
		uint32_t ClearFlagged();
		uint32_t Clear();
		void FlagForRemoval(const std::string &assetName,bool flag=true);
		void FlagAllForRemoval();

		const std::unordered_map<size_t,AssetInfo> &GetCache() const;
		const Asset *FindCachedAsset(const std::string &assetName) const;
		Asset *FindCachedAsset(const std::string &assetName);
	protected:
		bool RemoveFromCache(const std::string &assetName);
		void FlagForRemoval(size_t hashId,bool flag=true);
		void AddToCache(const std::string &assetName,const std::shared_ptr<Asset> &asset);
		size_t GetIdentifierHash(const std::string &assetName) const;
		void RegisterFileExtension(const std::string &ext);
		void StripFileExtension(std::string_view &key) const;

		std::unordered_map<size_t,AssetInfo> m_cache;
		std::unordered_set<size_t> m_flaggedForDeletion;
		std::vector<std::pair<std::string,size_t /* hash */>> m_extensions;
	};
};

#endif
