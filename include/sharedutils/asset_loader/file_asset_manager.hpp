/* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __FILE_ASSET_MANAGER_HPP__
#define __FILE_ASSET_MANAGER_HPP__

#include "sharedutils/asset_loader/asset_manager.hpp"
#include "sharedutils/asset_loader/asset_format_loader.hpp"
#include "sharedutils/asset_loader/asset_format_handler.hpp"
#include "sharedutils/asset_loader/asset_load_info.hpp"
#include "sharedutils/util_path.hpp"

namespace util
{
	struct AssetLoadInfo;
	struct DLLSHUTIL AssetFileHandler
	{
		std::function<std::unique_ptr<ufile::IFile>(const std::string&,AssetFormatType)> open;
		std::function<bool(const std::string&)> exists;
	};
	enum class AssetState : uint8_t
	{
		NotLoaded = 0,
		Loaded,
		FailedToLoad,
		Loading
	};
	class IAssetFormatHandler;
	class DLLSHUTIL FileAssetManager
		: public util::IAssetManager
	{
	public:
		static constexpr util::AssetLoadJobPriority DEFAULT_PRIORITY = 0;
		static constexpr util::AssetLoadJobPriority IMMEDIATE_PRIORITY = 10;
		struct DLLSHUTIL PreloadResult
		{
			enum class Result : uint8_t
			{
				Pending = 0,
				AlreadyLoaded,
				UnsupportedFormat,
				FileNotFound,
				UnableToOpenFile,
				JobCreationFailed,
				Success
			};
			std::optional<util::AssetLoadJobId> jobId {};
			Result result = Result::Pending;
			operator bool() const
			{
				return result == Result::AlreadyLoaded || result == Result::Success;
			}
		};

		FileAssetManager();
		virtual ~FileAssetManager() override;
		AssetState GetAssetState(const std::string &assetName) const;
		std::optional<std::string> FindAssetFilePath(const std::string &assetName,bool includeImportTypes=false) const;
		
		// MT-safe
		PreloadResult PreloadAsset(const std::string &path,std::unique_ptr<AssetLoadInfo> &&loadInfo=nullptr);

		// These may only be called from the same thread that handles the polling!
		util::AssetObject LoadAsset(const std::string &path,std::unique_ptr<AssetLoadInfo> &&loadInfo=nullptr);
		util::AssetObject LoadAsset(
			const std::string &cacheName,std::unique_ptr<ufile::IFile> &&file,const std::string &ext,std::unique_ptr<AssetLoadInfo> &&loadInfo=nullptr
		);

		void SetFileHandler(std::unique_ptr<AssetFileHandler> &&fileHandler);
		void RemoveFromCache(const std::string &path);
		void SetRootDirectory(const std::string &dir);
		const util::Path &GetRootDirectory() const;
		void SetImportDirectory(const std::string &dir);
		const util::Path &GetImportDirectory() const;

		void WaitForAllPendingCompleted();

		AssetFormatLoader &GetLoader() {return *m_loader;}
		const AssetFormatLoader &GetLoader() const {return const_cast<FileAssetManager*>(this)->GetLoader();}

		void CallOnLoad(const std::string &path,const std::function<void(util::Asset*,AssetLoadResult)> &onLoad);

		bool Import(const std::string &path);
		virtual void Poll();
	protected:
		virtual void InitializeProcessor(util::IAssetProcessor &processor)=0;
		virtual util::AssetObject InitializeAsset(const Asset &asset,const util::AssetLoadJob &job)=0;
		void RegisterFormatHandler(
			const std::string &ext,const std::function<std::unique_ptr<IAssetFormatHandler>(util::IAssetManager&)> &factory,
			AssetFormatType formatType=AssetFormatType::Binary
		);

		template<class T>
			void RegisterFormatHandler(
				const std::string &ext,
				AssetFormatType formatType=AssetFormatType::Binary
			)
		{
			return RegisterFormatHandler(ext,[](util::IAssetManager &assetManager) -> std::unique_ptr<util::IAssetFormatHandler> {
				return std::make_unique<T>(assetManager);
			},formatType);
		}

		PreloadResult PreloadAsset(const std::string &path,util::AssetLoadJobPriority priority,std::unique_ptr<AssetLoadInfo> &&loadInfo);
		PreloadResult PreloadAsset(
			const std::string &cacheName,std::unique_ptr<ufile::IFile> &&file,const std::string &ext,util::AssetLoadJobPriority priority,
			std::unique_ptr<AssetLoadInfo> &&loadInfo
		);
		util::AssetObject LoadAsset(
			const std::string &path,const PreloadResult &r,
			std::function<void(util::Asset&)> onLoaded,
			std::function<void()> onFailure
		);
		util::AssetObject Poll(std::optional<util::AssetLoadJobId> untilJob,util::AssetLoaderWaitMode wait);
		
		void RegisterImportHandler(
			const std::string &ext,const std::function<std::unique_ptr<util::IImportAssetFormatHandler>(util::IAssetManager &assetManager)> &factory,
			util::AssetFormatType formatType=util::AssetFormatType::Binary
		);
		template<class T>
			void RegisterImportHandler(
				const std::string &ext,
				util::AssetFormatType formatType=util::AssetFormatType::Binary
			)
		{
			return RegisterImportHandler(ext,[](util::IAssetManager &assetManager) -> std::unique_ptr<util::IImportAssetFormatHandler> {
				return std::make_unique<T>(assetManager);
			},formatType);
		}
		bool Import(const std::string &path,const std::string &ext);

		std::unique_ptr<AssetFormatLoader> m_loader;
	private:
		void ValidateMainThread();
		std::unordered_map<size_t,std::vector<std::function<void(util::Asset*,AssetLoadResult)>>> m_callOnLoad;
		std::unique_ptr<AssetFileHandler> m_fileHandler;
		util::Path m_rootDir;
		util::Path m_importRootDir;
		std::thread::id m_mainThreadId;

		std::unordered_map<std::string,std::function<std::unique_ptr<IImportAssetFormatHandler>(util::IAssetManager&)>> m_importHandlers;
	};

	template<typename TAssetType,typename TLoadInfo>
	class TFileAssetManager
		: public util::FileAssetManager
	{
	public:
		using AssetType = TAssetType;
		static std::shared_ptr<TAssetType> GetAssetObject(util::Asset &asset)
		{
			return std::static_pointer_cast<TAssetType>(asset.assetObject);
		}
		
		TAssetType *GetAssetObject(AssetIndex index)
		{
			auto *assetInfo = (index < m_assets.size()) ? &m_assets[index] : nullptr;
			return (assetInfo && assetInfo->asset && assetInfo->asset->assetObject) ? static_cast<TAssetType*>(assetInfo->asset->assetObject.get()) : nullptr;
		}
		FileAssetManager::PreloadResult PreloadAsset(const std::string &path,std::unique_ptr<TLoadInfo> &&loadInfo=nullptr)
		{
			if(!loadInfo)
				loadInfo = std::make_unique<TLoadInfo>();
			return FileAssetManager::PreloadAsset(path,std::move(loadInfo));
		}
		std::shared_ptr<TAssetType> LoadAsset(const std::string &path,std::unique_ptr<TLoadInfo> &&loadInfo=nullptr)
		{
			if(!loadInfo)
				loadInfo = std::make_unique<TLoadInfo>();
			return std::static_pointer_cast<TAssetType>(FileAssetManager::LoadAsset(path,std::move(loadInfo)));
		}
		std::shared_ptr<TAssetType> LoadAsset(const std::string &path,util::AssetLoadFlags flags)
		{
			auto loadInfo = std::make_unique<TLoadInfo>();
			loadInfo->flags = flags;
			return LoadAsset(path,std::move(loadInfo));
		}
		std::shared_ptr<TAssetType> LoadAsset(
			const std::string &cacheName,std::unique_ptr<ufile::IFile> &&file,const std::string &ext,std::unique_ptr<TLoadInfo> &&loadInfo=nullptr
		)
		{
			if(!loadInfo)
				loadInfo = std::make_unique<TLoadInfo>();
			return std::static_pointer_cast<TAssetType>(FileAssetManager::LoadAsset(cacheName,std::move(file),ext,std::move(loadInfo)));
		}
	private:
		FileAssetManager::PreloadResult PreloadAsset(const std::string &path,util::AssetLoadJobPriority priority,std::unique_ptr<TLoadInfo> &&loadInfo)
		{
			if(!loadInfo)
				loadInfo = std::make_unique<TLoadInfo>();
			return FileAssetManager::PreloadAsset(path,priority,std::move(loadInfo));
		}
		FileAssetManager::PreloadResult PreloadAsset(
			const std::string &cacheName,std::unique_ptr<ufile::IFile> &&file,const std::string &ext,util::AssetLoadJobPriority priority,
			std::unique_ptr<TLoadInfo> &&loadInfo
		)
		{
			if(!loadInfo)
				loadInfo = std::make_unique<TLoadInfo>();
			return FileAssetManager::PreloadAsset(cacheName,std::move(file),ext,priority,std::move(loadInfo));
		}
	};
};

#endif
