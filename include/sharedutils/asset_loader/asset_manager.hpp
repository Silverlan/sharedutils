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
#include <mutex>
#include <queue>
#include <memory>

namespace util
{
	using AssetObject = std::shared_ptr<void>;
	using AssetIndex = uint32_t;
	struct DLLSHUTIL Asset
	{
	public:
		bool IsInUse() const;
		AssetObject assetObject;
		AssetIndex index = std::numeric_limits<AssetIndex>::max();
		bool alwaysInUse = false;
	};
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
			bool anonymous = false; // If true, asset is not named (only indexed)
		};
		struct DLLSHUTIL FormatExtensionInfo
		{
			enum class Type : uint8_t
			{
				Native = 0,
				Import
			};
			std::string extension;
			size_t hash;
			AssetFormatType formatType;
			Type type;
		};
		std::string ToCacheIdentifier(const std::string &assetName) const;

		IAssetManager();
		virtual ~IAssetManager();

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
		void FlagAssetAsAlwaysInUse(const std::string &assetName,bool alwaysInUse);

		const std::vector<FormatExtensionInfo> &GetSupportedFormatExtensions() const {return m_extensions;}
	protected:
		virtual void Reset();
		Asset *FindCachedAsset(const std::string &assetName,bool lockMutex);
		void ValidateMainThread();
		bool ClearAsset(AssetIndex idx);
		bool RemoveFromCache(const std::string &assetName);
		void FlagForRemoval(AssetIndex idx,bool flag=true);
		AssetIndex AddToCache(const std::string &assetName,const std::shared_ptr<Asset> &asset);
		AssetIndex AddToIndex(const std::shared_ptr<Asset> &asset);
		AssetIdentifierHash GetIdentifierHash(const std::string &assetName) const;
		void RegisterFileExtension(const std::string &ext,AssetFormatType formatType=AssetFormatType::Binary,FormatExtensionInfo::Type type=FormatExtensionInfo::Type::Native);
		void StripFileExtension(std::string_view &key) const;
		static std::vector<FormatExtensionInfo>::const_iterator FindExtension(
			const std::vector<FormatExtensionInfo> &exts,const std::string_view &ext,
			const std::optional<FormatExtensionInfo::Type> type={}
		);

		std::vector<AssetInfo> m_assets;
		std::unordered_map<AssetIdentifierHash,AssetIndex> m_cache;
		std::queue<AssetIndex> m_freeIndices;
		mutable std::mutex m_cacheMutex;
		std::unordered_set<AssetIndex> m_flaggedForDeletion;
		std::vector<FormatExtensionInfo> m_extensions;
		std::thread::id m_mainThreadId;
		bool m_reset = false;
	};
};

#endif
