/* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __ASSET_MANAGER_HPP__
#define __ASSET_MANAGER_HPP__

#include "sharedutils/utildefinitions.h"
#include <unordered_map>
#include <unordered_set>
#include <optional>
#include <string>
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
	using AssetIndex = uint32_t;
	using AssetIdentifierHash = size_t;
	enum class AssetFormatType : uint8_t
	{
		Binary = 0,
		Text
	};
	class DLLSHUTIL IAssetManager
	{
	public:
		struct DLLSHUTIL AssetInfo
		{
			AssetIndex index;
			AssetIdentifierHash hash;
			std::shared_ptr<Asset> asset;
			std::string identifier;
		};
		std::string ToCacheIdentifier(const std::string &assetName) const;

		IAssetManager();
		virtual ~IAssetManager()=default;

		uint32_t ClearUnused();
		uint32_t ClearFlagged();
		uint32_t Clear();
		void FlagForRemoval(const std::string &assetName,bool flag=true);
		void FlagAllForRemoval();

		Asset *GetAsset(AssetIndex index);
		const Asset *GetAsset(AssetIndex index) const {return const_cast<IAssetManager*>(this)->GetAsset(index);}
		std::optional<AssetIndex> GetAssetIndex(const std::string &assetName) const;

		const std::vector<AssetInfo> &GetAssets() const;
		const std::unordered_map<AssetIdentifierHash,AssetIndex> &GetCache() const;
		const Asset *FindCachedAsset(const std::string &assetName) const;
		Asset *FindCachedAsset(const std::string &assetName);
	protected:
		struct FormatExtensionInfo
		{
			std::string extension;
			size_t hash;
			AssetFormatType formatType;
		};
		bool ClearAsset(AssetIndex idx);
		bool RemoveFromCache(const std::string &assetName);
		void FlagForRemoval(AssetIndex idx,bool flag=true);
		void AddToCache(const std::string &assetName,const std::shared_ptr<Asset> &asset);
		AssetIdentifierHash GetIdentifierHash(const std::string &assetName) const;
		void RegisterFileExtension(const std::string &ext,AssetFormatType formatType=AssetFormatType::Binary);
		void StripFileExtension(std::string_view &key) const;

		std::vector<AssetInfo> m_assets;
		std::unordered_map<AssetIdentifierHash,AssetIndex> m_cache;
		std::unordered_set<AssetIndex> m_flaggedForDeletion;
		std::vector<FormatExtensionInfo> m_extensions;
	};
};

#endif
