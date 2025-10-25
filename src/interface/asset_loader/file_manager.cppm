// SPDX-FileCopyrightText: (c) 2021 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include "sharedutils/utildefinitions.h"
#include <memory>
#include <string>

#include <optional>
#include <vector>
#include <functional>

export module pragma.util:asset_loader.file_manager;

export import :asset_loader.format_handler;
export import :asset_loader.format_loader;
export import :asset_loader.manager;
export import :asset_loader.load_info;
export import :asset_loader.load_job;
export import :file_interface;
export import :path;

export namespace util {
	struct AssetLoadInfo;
	struct DLLSHUTIL AssetFileHandler {
		std::function<std::unique_ptr<ufile::IFile>(const std::string &, AssetFormatType)> open;
		std::function<bool(const std::string &)> exists;
	};
	enum class AssetState : uint8_t { NotLoaded = 0, Loaded, FailedToLoad, Loading };
	class IAssetFormatHandler;
	class DLLSHUTIL FileAssetManager : public util::IAssetManager {
	  public:
		static constexpr util::AssetLoadJobPriority DEFAULT_PRIORITY = 0;
		static constexpr util::AssetLoadJobPriority IMMEDIATE_PRIORITY = 10;
		struct DLLSHUTIL PreloadResult {
			enum class Result : uint8_t {
				Pending = 0,
				AlreadyLoaded,
				UnsupportedFormat,
				FileNotFound,
				UnableToOpenFile,
				JobCreationFailed,
				ImportFailed,
				Success,
			};
			std::optional<util::AssetLoadJobId> jobId {};
			Result result = Result::Pending;
			std::optional<std::string> errorMessage {};
			bool firstTimeError = true;
			operator bool() const { return result == Result::AlreadyLoaded || result == Result::Success; }
		};
		struct DLLSHUTIL Callbacks {
			std::function<void(util::Asset *, AssetLoadResult)> onAssetLoaded;
			std::function<void(const std::string &)> onAssetReloaded;
			std::function<void(const std::string &)> onAssetRemoved;
		};

		FileAssetManager();
		virtual ~FileAssetManager() override;
		AssetState GetAssetState(const std::string &assetName) const;
		std::optional<std::string> FindAssetFilePath(const std::string &assetName, bool includeImportTypes = false) const;
		virtual void SetLogHandler(const util::LogHandler &logHandler) override;

		// MT-safe
		PreloadResult PreloadAsset(const std::string &path, std::unique_ptr<AssetLoadInfo> &&loadInfo = nullptr);

		// These may only be called from the same thread that handles the polling!
		util::AssetObject LoadAsset(const std::string &path, std::unique_ptr<AssetLoadInfo> &&loadInfo = nullptr, PreloadResult *optOutResult = nullptr);
		util::AssetObject LoadAsset(const std::string &cacheName, std::unique_ptr<ufile::IFile> &&file, const std::string &ext, std::unique_ptr<AssetLoadInfo> &&loadInfo = nullptr, PreloadResult *optOutResult = nullptr);
		bool WaitUntilAssetLoaded(const std::string &path);

		Callbacks &GetCallbacks() { return m_callbacks; }

		virtual std::unique_ptr<AssetLoadInfo> CreateDefaultLoadInfo() const = 0;
		virtual util::AssetObject ReloadAsset(const std::string &path, std::unique_ptr<AssetLoadInfo> &&loadInfo = nullptr, PreloadResult *optOutResult = nullptr);
		void SetFileHandler(std::unique_ptr<AssetFileHandler> &&fileHandler);
		void SetExternalSourceFileImportHandler(const std::function<std::optional<std::string>(const std::string &, const std::string &)> &handler);
		const std::function<std::optional<std::string>(const std::string &, const std::string &)> &GetExternalSourceFileImportHandler() const;
		void RemoveFromCache(const std::string &path);
		void SetRootDirectory(const std::string &dir);
		const util::Path &GetRootDirectory() const;
		void SetImportDirectory(const std::string &dir);
		const util::Path &GetImportDirectory() const;

		void WaitForAllPendingCompleted();

		AssetFormatLoader &GetLoader() { return *m_loader; }
		const AssetFormatLoader &GetLoader() const { return const_cast<FileAssetManager *>(this)->GetLoader(); }

		void CallOnLoad(const std::string &path, const std::function<void(util::Asset *, AssetLoadResult)> &onLoad);

		bool Import(const std::string &path);
		bool Import(const std::string &path, const std::string &outputPath);
		virtual void Poll();
	  protected:
		virtual void Reset() override;
		void ClearCachedResult(size_t hash);
		virtual void InitializeProcessor(util::IAssetProcessor &processor) = 0;
		virtual util::AssetObject InitializeAsset(const Asset &asset, const util::AssetLoadJob &job) = 0;
		void OnAssetReloaded(const std::string &assetName);
		std::optional<std::string> ImportAssetFilesFromExternalSource(const std::string &assetName, const std::string &outputName);
		std::optional<std::string> ImportAssetFilesFromExternalSource(const std::string &assetName);
		void RegisterFormatHandler(const std::string &ext, const std::function<std::unique_ptr<IAssetFormatHandler>(util::IAssetManager &)> &factory, AssetFormatType formatType = AssetFormatType::Binary);
		template<class T>
		void RegisterFormatHandler(const std::string &ext, AssetFormatType formatType = AssetFormatType::Binary)
		{
			return RegisterFormatHandler(
			  ext, [](util::IAssetManager &assetManager) -> std::unique_ptr<util::IAssetFormatHandler> { return std::make_unique<T>(assetManager); }, formatType);
		}

		PreloadResult PreloadAsset(const std::string &path, util::AssetLoadJobPriority priority, std::unique_ptr<AssetLoadInfo> &&loadInfo);
		PreloadResult PreloadAsset(const std::string &cacheName, std::unique_ptr<ufile::IFile> &&file, const std::string &ext, util::AssetLoadJobPriority priority, std::unique_ptr<AssetLoadInfo> &&loadInfo);
		util::AssetObject LoadAsset(const std::string &path, const PreloadResult &r, std::function<void(util::Asset &)> onLoaded, std::function<void()> onFailure);
		util::AssetObject Poll(std::optional<util::AssetLoadJobId> untilJob, util::AssetLoaderWaitMode wait);

		void RegisterImportHandler(const std::string &ext, const std::function<std::unique_ptr<util::IImportAssetFormatHandler>(util::IAssetManager &assetManager)> &factory, util::AssetFormatType formatType = util::AssetFormatType::Binary);
		template<class T>
		void RegisterImportHandler(const std::string &ext, util::AssetFormatType formatType = util::AssetFormatType::Binary)
		{
			return RegisterImportHandler(
			  ext, [](util::IAssetManager &assetManager) -> std::unique_ptr<util::IImportAssetFormatHandler> { return std::make_unique<T>(assetManager); }, formatType);
		}
		bool Import(const std::string &path, const std::string &outputPath, const std::string &ext, std::string *optOutErrMsg = nullptr);

		std::unique_ptr<AssetFormatLoader> m_loader;
		Callbacks m_callbacks;
	  private:
		void CacheResult(size_t hash, const util::FileAssetManager::PreloadResult &result);
		void CacheResult(const std::string &assetName, const util::FileAssetManager::PreloadResult &result);
		std::optional<util::FileAssetManager::PreloadResult> GetCachedResult(size_t hash) const;
		std::function<std::optional<std::string>(const std::string &, const std::string &)> m_externalSourceFileImportHandler = nullptr;
		std::unordered_map<size_t, std::vector<std::function<void(util::Asset *, AssetLoadResult)>>> m_callOnLoad;
		std::unique_ptr<AssetFileHandler> m_fileHandler;
		util::Path m_rootDir;
		util::Path m_importRootDir;
		std::mutex m_preloadMutex;

		std::unordered_map<size_t, util::FileAssetManager::PreloadResult> m_errorCache;
		mutable std::mutex m_errorCacheMutex;

		std::unordered_map<std::string, std::function<std::unique_ptr<IImportAssetFormatHandler>(util::IAssetManager &)>> m_importHandlers;
	};

	template<typename TAssetType, typename TLoadInfo>
	class TFileAssetManager : public util::FileAssetManager {
	  public:
		using AssetType = TAssetType;
		static std::shared_ptr<TAssetType> GetAssetObject(util::Asset &asset) { return std::static_pointer_cast<TAssetType>(asset.assetObject); }

		TAssetType *GetAssetObject(AssetIndex index)
		{
			auto *assetInfo = (index < m_assets.size()) ? &m_assets[index] : nullptr;
			return (assetInfo && assetInfo->asset && assetInfo->asset->assetObject) ? static_cast<TAssetType *>(assetInfo->asset->assetObject.get()) : nullptr;
		}
		FileAssetManager::PreloadResult PreloadAsset(const std::string &path, std::unique_ptr<TLoadInfo> &&loadInfo = nullptr) { return FileAssetManager::PreloadAsset(path, std::move(loadInfo)); }
		std::shared_ptr<TAssetType> LoadAsset(const std::string &path, std::unique_ptr<TLoadInfo> &&loadInfo = nullptr, PreloadResult *optOutResult = nullptr) { return std::static_pointer_cast<TAssetType>(FileAssetManager::LoadAsset(path, std::move(loadInfo), optOutResult)); }
		std::shared_ptr<TAssetType> LoadAsset(const std::string &path, util::AssetLoadFlags flags, PreloadResult *optOutResult = nullptr)
		{
			auto loadInfo = std::make_unique<TLoadInfo>();
			loadInfo->flags = flags;
			return LoadAsset(path, std::move(loadInfo), optOutResult);
		}
		std::shared_ptr<TAssetType> ReloadAsset(const std::string &path, std::unique_ptr<TLoadInfo> &&loadInfo = nullptr, PreloadResult *optOutResult = nullptr) { return std::static_pointer_cast<TAssetType>(FileAssetManager::ReloadAsset(path, std::move(loadInfo), optOutResult)); }
		std::shared_ptr<TAssetType> LoadAsset(const std::string &cacheName, std::unique_ptr<ufile::IFile> &&file, const std::string &ext, std::unique_ptr<TLoadInfo> &&loadInfo = nullptr, PreloadResult *optOutResult = nullptr)
		{
			return std::static_pointer_cast<TAssetType>(FileAssetManager::LoadAsset(cacheName, std::move(file), ext, std::move(loadInfo), optOutResult));
		}
		virtual std::unique_ptr<AssetLoadInfo> CreateDefaultLoadInfo() const override { return std::make_unique<TLoadInfo>(); }
	  private:
		FileAssetManager::PreloadResult PreloadAsset(const std::string &path, util::AssetLoadJobPriority priority, std::unique_ptr<TLoadInfo> &&loadInfo) { return FileAssetManager::PreloadAsset(path, priority, std::move(loadInfo)); }
		FileAssetManager::PreloadResult PreloadAsset(const std::string &cacheName, std::unique_ptr<ufile::IFile> &&file, const std::string &ext, util::AssetLoadJobPriority priority, std::unique_ptr<TLoadInfo> &&loadInfo)
		{
			return FileAssetManager::PreloadAsset(cacheName, std::move(file), ext, priority, std::move(loadInfo));
		}
	};
};
