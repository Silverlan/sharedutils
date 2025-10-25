// SPDX-FileCopyrightText: (c) 2021 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include <optional>

#include <vector>
#include <functional>

#include <cassert>
#include <utility>
#include <memory>
#include <mutex>
#include <string>

module pragma.util;

import :asset_loader.file_manager;

#undef AddJob

util::FileAssetManager::FileAssetManager() : util::IAssetManager {} {}
util::FileAssetManager::~FileAssetManager() { Reset(); }
void util::FileAssetManager::SetRootDirectory(const std::string &dir) { m_rootDir = util::Path::CreatePath(dir); }
const util::Path &util::FileAssetManager::GetRootDirectory() const { return m_rootDir; }
void util::FileAssetManager::SetImportDirectory(const std::string &dir) { m_importRootDir = util::Path::CreatePath(dir); }
const util::Path &util::FileAssetManager::GetImportDirectory() const { return m_importRootDir; }

void util::FileAssetManager::SetFileHandler(std::unique_ptr<AssetFileHandler> &&fileHandler) { m_fileHandler = std::move(fileHandler); }
void util::FileAssetManager::SetExternalSourceFileImportHandler(const std::function<std::optional<std::string>(const std::string &, const std::string &)> &handler) { m_externalSourceFileImportHandler = handler; }
const std::function<std::optional<std::string>(const std::string &, const std::string &)> &util::FileAssetManager::GetExternalSourceFileImportHandler() const { return m_externalSourceFileImportHandler; }

bool util::FileAssetManager::WaitUntilAssetLoaded(const std::string &path)
{
	auto normalizedPath = ToCacheIdentifier(path);
	if(FindCachedAsset(normalizedPath))
		return true;
	auto jobId = m_loader->FindJobId(normalizedPath);
	if(!jobId.has_value())
		return false;
	auto asset = Poll(jobId, util::AssetLoaderWaitMode::Single);
	return asset != nullptr;
}

void util::FileAssetManager::RemoveFromCache(const std::string &path)
{
	ClearCachedResult(GetIdentifierHash(path));
	auto removed = IAssetManager::RemoveFromCache(path);
	auto invalidated = m_loader->InvalidateLoadJob(path);
	if(m_callbacks.onAssetRemoved && (removed || invalidated))
		m_callbacks.onAssetRemoved(path);
}
void util::FileAssetManager::OnAssetReloaded(const std::string &assetName)
{
	if(m_callbacks.onAssetReloaded)
		m_callbacks.onAssetReloaded(assetName);
}
void util::FileAssetManager::RegisterFormatHandler(const std::string &ext, const std::function<std::unique_ptr<IAssetFormatHandler>(util::IAssetManager &)> &factory, AssetFormatType formatType)
{
	GetLoader().RegisterFormatHandler(ext, factory);
	RegisterFileExtension(ext, formatType);
}
void util::FileAssetManager::Reset()
{
	m_callbacks = {};
	m_errorCache.clear();
	m_loader = nullptr;
	m_importHandlers.clear();
	util::IAssetManager::Reset();
}
void util::FileAssetManager::CallOnLoad(const std::string &path, const std::function<void(util::Asset *, AssetLoadResult)> &onLoad)
{
	auto hash = GetIdentifierHash(path);
	auto it = m_callOnLoad.find(hash);
	if(it == m_callOnLoad.end())
		it = m_callOnLoad.insert(std::make_pair(hash, std::vector<std::function<void(util::Asset *, AssetLoadResult)>> {})).first;
	it->second.push_back(onLoad);
}
util::FileAssetManager::PreloadResult util::FileAssetManager::PreloadAsset(const std::string &strPath, std::unique_ptr<util::AssetLoadInfo> &&loadInfo)
{
	if(!loadInfo)
		loadInfo = CreateDefaultLoadInfo();
	return PreloadAsset(strPath, DEFAULT_PRIORITY, std::move(loadInfo));
}
util::FileAssetManager::PreloadResult util::FileAssetManager::PreloadAsset(const std::string &cacheName, std::unique_ptr<ufile::IFile> &&file, const std::string &ext, util::AssetLoadJobPriority priority, std::unique_ptr<util::AssetLoadInfo> &&loadInfo)
{
	auto hash = GetIdentifierHash(cacheName);
	if(!loadInfo || !umath::is_flag_set(loadInfo->flags, util::AssetLoadFlags::IgnoreCache)) {
		auto cachedResult = GetCachedResult(hash);
		if(cachedResult.has_value())
			return *cachedResult;
	}
	auto jobId = m_loader->AddJob(ToCacheIdentifier(cacheName), ext, std::move(file), priority, [this, &loadInfo, hash](util::IAssetProcessor &processor) {
		auto &faProcessor = static_cast<FileAssetProcessor &>(processor);
		faProcessor.loadInfo = std::move(loadInfo);
		InitializeProcessor(processor);
		faProcessor.onLoaded = [this, &faProcessor, hash](util::Asset *asset, AssetLoadResult result) {
			auto &loadInfo = *faProcessor.loadInfo;
			if(asset) {
				if(loadInfo.onLoaded)
					loadInfo.onLoaded(*asset);
			}
			else if(loadInfo.onFailure)
				loadInfo.onFailure();

			if(m_callbacks.onAssetLoaded)
				m_callbacks.onAssetLoaded(asset, result);

			if(asset) {
				auto it = m_callOnLoad.find(hash);
				if(it != m_callOnLoad.end()) {
					auto functions = std::move(it->second);
					m_callOnLoad.erase(it);
					for(auto &f : functions)
						f(asset, result);
				}
			}
		};
	});
	if(!jobId.has_value())
		return PreloadResult {{}, PreloadResult::Result::JobCreationFailed};
	return PreloadResult {jobId, PreloadResult::Result::Success};
}
void util::FileAssetManager::ClearCachedResult(size_t hash)
{
	std::scoped_lock lock {m_errorCacheMutex};
	auto it = m_errorCache.find(hash);
	if(it != m_errorCache.end())
		m_errorCache.erase(it);
}
std::optional<util::FileAssetManager::PreloadResult> util::FileAssetManager::GetCachedResult(size_t hash) const
{
	std::scoped_lock lock {m_errorCacheMutex};
	auto it = m_errorCache.find(hash);
	if(it == m_errorCache.end())
		return {};
	auto result = it->second;
	result.firstTimeError = false;
	return result;
}
void util::FileAssetManager::CacheResult(size_t hash, const util::FileAssetManager::PreloadResult &result)
{
	std::scoped_lock lock {m_errorCacheMutex};
	m_errorCache[hash] = result;
}
void util::FileAssetManager::CacheResult(const std::string &assetName, const util::FileAssetManager::PreloadResult &result) { CacheResult(GetIdentifierHash(assetName), result); }
util::FileAssetManager::PreloadResult util::FileAssetManager::PreloadAsset(const std::string &strPath, util::AssetLoadJobPriority priority, std::unique_ptr<util::AssetLoadInfo> &&loadInfo)
{
	if(!loadInfo || !umath::is_flag_set(loadInfo->flags, AssetLoadFlags::IgnoreCache)) {
		auto *asset = FindCachedAsset(strPath);
		if(asset)
			return PreloadResult {{}, PreloadResult::Result::AlreadyLoaded};
	}

	auto cachedResult = GetCachedResult(GetIdentifierHash(strPath));
	if(cachedResult.has_value())
		return *cachedResult;

	auto path = util::Path::CreateFile(strPath);
	if(!loadInfo || !umath::is_flag_set(loadInfo->flags, AssetLoadFlags::AbsolutePath))
		path = m_rootDir + path;
	auto ext = path.GetFileExtension();
	AssetFormatType formatType = AssetFormatType::Binary;
	auto hadExtension = ext.has_value();
	if(hadExtension) {
		// Confirm that this is a valid asset format extension
		auto it = std::find_if(m_extensions.begin(), m_extensions.end(), [&ext](const FormatExtensionInfo &extInfo) { return extInfo.type == FormatExtensionInfo::Type::Native && *ext == extInfo.extension; });
		if(it == m_extensions.end())
			ext = {};
		else
			formatType = it->formatType;
	}
	if(!ext.has_value()) {
		// No valid extension found, search for all known extensions
		for(auto &extInfo : m_extensions) {
			if(extInfo.type != FormatExtensionInfo::Type::Native)
				continue;
			auto extPath = path + ("." + extInfo.extension);
			if(m_fileHandler->exists(extPath.GetString())) {
				path = std::move(extPath);
				ext = extInfo.extension;
				formatType = extInfo.formatType;
				break;
			}
		}
	}
	if(!ext.has_value()) {
		if(hadExtension && m_fileHandler->exists(path.GetString())) {
			// File with original path was found, but extension is not a recognized format
			return PreloadResult {{}, PreloadResult::Result::UnsupportedFormat};
		}
		return PreloadResult {{}, PreloadResult::Result::FileNotFound};
	}
	auto f = m_fileHandler->open(path.GetString(), formatType);
	if(!f)
		return PreloadResult {{}, PreloadResult::Result::UnableToOpenFile};

	// m_preloadMutex mutex ensures that the code below is only called from one thread at a time.
	// The other two mutexes are required because we need to ensure that the asset is not already in the
	// cache AND it is not already being loaded
	std::scoped_lock lock {m_preloadMutex, m_loader->GetJobGuardMutex(), m_cacheMutex};
	if(!loadInfo || !umath::is_flag_set(loadInfo->flags, AssetLoadFlags::IgnoreCache)) {
		auto *asset = FindCachedAsset(strPath, false);
		if(asset)
			return PreloadResult {{}, PreloadResult::Result::AlreadyLoaded};
	}
	// This call will add the job, but only if no job for the asset is already queued (or if the new priority is higher)
	return PreloadAsset(strPath, std::move(f), *ext, priority, std::move(loadInfo));
}
util::AssetObject util::FileAssetManager::LoadAsset(const std::string &path, const PreloadResult &r, std::function<void(util::Asset &)> onLoaded, std::function<void()> onFailure)
{
	ValidateMainThread();
	if(r == false) {
		if(onFailure)
			onFailure();
		return nullptr;
	}
	if(!r.jobId.has_value()) {
		// Already fully loaded
		auto *asset = FindCachedAsset(path);
		if(onLoaded)
			onLoaded(*asset);
		return asset->assetObject;
	}

	auto identifier = ToCacheIdentifier(path);
	auto jobId = *r.jobId;
	std::chrono::steady_clock::time_point t;
	if(ShouldLog()) {
		m_logHandler("LoadAsset '" + identifier + "': Job Id: " + std::to_string(jobId), util::LogSeverity::Debug);
		t = std::chrono::steady_clock::now();
	}

	auto o = Poll(jobId, util::AssetLoaderWaitMode::None);
	if(!o) {
		auto cached = FindCachedAsset(identifier);
		if(cached)
			o = cached->assetObject;
	}

	if(ShouldLog() && o) {
		auto dt = std::chrono::steady_clock::now() - t;
		m_logHandler("Waited " + std::to_string(dt.count() / 1'000'000'000.0) + " seconds for asset '" + identifier + "'!", util::LogSeverity::Debug);
	}
	return o;
}

void util::FileAssetManager::Poll() { Poll({}, util::AssetLoaderWaitMode::None); }

void util::FileAssetManager::WaitForAllPendingCompleted() { Poll({}, util::AssetLoaderWaitMode::All); }

util::AssetState util::FileAssetManager::GetAssetState(const std::string &assetName) const
{
	auto hash = GetIdentifierHash(assetName);
	auto it = m_cache.find(hash);
	if(it != m_cache.end())
		return AssetState::Loaded;
	auto loadState = m_loader->GetLoadState(ToCacheIdentifier(assetName));
	switch(loadState) {
	case AssetLoadState::NotQueued:
		return AssetState::NotLoaded;
	case AssetLoadState::LoadedAndPendingForCompletion:
	case AssetLoadState::Loading:
		return AssetState::Loading;
	}
	// Unreachable
	return AssetState::NotLoaded;
}

std::optional<std::string> util::FileAssetManager::FindAssetFilePath(const std::string &assetName, bool includeImportTypes) const
{
	auto normalizedName = ToCacheIdentifier(assetName);
	for(auto &extInfo : m_extensions) {
		if(!includeImportTypes && extInfo.type == FormatExtensionInfo::Type::Import)
			continue;
		auto &ext = extInfo.extension;
		auto extName = normalizedName + '.' + ext;
		if(!m_fileHandler->exists(m_rootDir.GetString() + extName))
			continue;
		return extName;
	}
	return {};
}

void util::FileAssetManager::SetLogHandler(const util::LogHandler &logHandler)
{
	util::IAssetManager::SetLogHandler(logHandler);
	m_loader->SetLogHandler(logHandler);
}

util::AssetObject util::FileAssetManager::Poll(std::optional<util::AssetLoadJobId> untilJob, util::AssetLoaderWaitMode wait)
{
	// if(ShouldLog())
	// 	m_logHandler("Poll: Waiting for job " + (untilJob.has_value() ? std::to_string(*untilJob) : std::string {"n/a"}) + "!", util::LogSeverity::Trace);
	if(untilJob.has_value() && wait == util::AssetLoaderWaitMode::None)
		wait = util::AssetLoaderWaitMode::Single;
	util::AssetObject targetAsset = nullptr;
	do {
		// if(ShouldLog())
		// 	m_logHandler("Pre-poll loader state: " + std::to_string(m_loader->HasCompletedJobs()) + "," + std::to_string(m_loader->HasPendingJobs()), util::LogSeverity::Trace);
		if(untilJob.has_value() && !m_loader->IsJobPending(*untilJob)) // TODO: What conditions cause this?
			return nullptr;
		m_loader->Poll(
		  [this, &untilJob, &targetAsset](const util::AssetLoadJob &job, AssetLoadResult result, std::optional<std::string> errMsg) {
			  if(ShouldLog())
				  m_logHandler("Poll: Job " + (untilJob.has_value() ? std::to_string(*untilJob) : std::string {"n/a"}) + " state: " + std::to_string(umath::to_integral(result)) + "!", util::LogSeverity::Trace);
			  auto &processor = *static_cast<FileAssetProcessor *>(job.processor.get());
			  util::Asset *passet = nullptr;
			  switch(result) {
			  case AssetLoadResult::Succeeded:
				  {
					  auto dtQueue = job.completionTime - job.queueStartTime;
					  auto dtTask = job.completionTime - job.taskStartTime;
					  if(ShouldLog()) {
						  m_logHandler(job.identifier + " has been loaded!", util::LogSeverity::Debug);
						  m_logHandler("Time since job has been queued to completion: " + std::to_string(dtQueue.count() / 1'000'000'000.0), util::LogSeverity::Debug);
						  m_logHandler("Time since task has been started to completion: " + std::to_string(dtTask.count() / 1'000'000'000.0), util::LogSeverity::Debug);
					  }
					  auto asset = std::make_shared<util::Asset>();

					  auto &loadInfo = *processor.loadInfo;
					  if(!umath::is_flag_set(loadInfo.flags, AssetLoadFlags::DontCache) && !umath::is_flag_set(loadInfo.flags, AssetLoadFlags::IgnoreCache))
						  AddToCache(job.identifier, asset);
					  auto assetObject = InitializeAsset(*asset, job);
					  asset->assetObject = assetObject;
					  if(untilJob.has_value() && job.jobId == *untilJob) {
						  targetAsset = asset->assetObject;
						  untilJob = {};
					  }
					  passet = asset.get();
					  break;
				  }
			  default:
				  {
					  if(ShouldLog()) {
						  std::string msg = job.identifier + " has failed: ";
						  if(errMsg)
							  msg += *errMsg;
						  else
							  msg += "Unknown error.";
						  m_logHandler(msg, util::LogSeverity::Warning);
					  }

					  if(untilJob.has_value() && job.jobId == *untilJob) {
						  targetAsset = nullptr;
						  untilJob = {};
					  }
					  break;
				  }
			  }
			  if(processor.onLoaded)
				  processor.onLoaded(passet, result);
		  },
		  wait);
	} while(untilJob.has_value());
	return targetAsset;
}

std::optional<std::string> util::FileAssetManager::ImportAssetFilesFromExternalSource(const std::string &assetName, const std::string &outputName)
{
	if(!m_externalSourceFileImportHandler)
		return {};
	return m_externalSourceFileImportHandler(assetName, outputName);
}
std::optional<std::string> util::FileAssetManager::ImportAssetFilesFromExternalSource(const std::string &assetName) { return ImportAssetFilesFromExternalSource(assetName, assetName); }
util::AssetObject util::FileAssetManager::ReloadAsset(const std::string &path, std::unique_ptr<AssetLoadInfo> &&loadInfo, PreloadResult *optOutResult)
{
	if(!loadInfo)
		loadInfo = CreateDefaultLoadInfo();
	RemoveFromCache(path);
	auto obj = LoadAsset(path, std::move(loadInfo), optOutResult);
	if(!obj)
		return nullptr;
	OnAssetReloaded(path);
	return obj;
}
util::AssetObject util::FileAssetManager::LoadAsset(const std::string &cacheName, std::unique_ptr<ufile::IFile> &&file, const std::string &ext, std::unique_ptr<util::AssetLoadInfo> &&loadInfo, PreloadResult *optOutResult)
{
	ValidateMainThread();
	if(!loadInfo)
		loadInfo = CreateDefaultLoadInfo();
	auto onLoaded = loadInfo ? loadInfo->onLoaded : nullptr;
	auto onFailure = loadInfo ? loadInfo->onFailure : nullptr;
	auto r = PreloadAsset(cacheName, std::move(file), ext, IMMEDIATE_PRIORITY, std::move(loadInfo));
	if(optOutResult)
		*optOutResult = r;
	if(!r) {
		if(!loadInfo || (loadInfo->flags & util::AssetLoadFlags::DontCache) == util::AssetLoadFlags::None)
			CacheResult(cacheName, r);
		return nullptr;
	}
	return LoadAsset(cacheName, r, onLoaded, onFailure);
}
util::AssetObject util::FileAssetManager::LoadAsset(const std::string &path, std::unique_ptr<util::AssetLoadInfo> &&loadInfo, PreloadResult *optOutResult)
{
	if(!loadInfo)
		loadInfo = CreateDefaultLoadInfo();
	ValidateMainThread();
	auto onLoaded = loadInfo->onLoaded;
	auto onFailure = loadInfo->onFailure;
	auto r = PreloadAsset(path, IMMEDIATE_PRIORITY, std::move(loadInfo));
	if(!r && !r.firstTimeError) {
		// Already failed previously; Don't bother trying again
		if(optOutResult)
			*optOutResult = r;
		return nullptr;
	}
	auto assetPath = path;
	if(loadInfo) // loadInfo should always still be valid if PreloadAsset failed
	{
		if(r.result == PreloadResult::Result::UnsupportedFormat) {
			// File was found, but format is not a native format. Try to import it.
			std::string ext;
			auto hasExt = ufile::get_extension(assetPath, &ext);
			assert(hasExt);
			if(hasExt) // This should always be true
			{
				std::string errMsg;
				auto fullAssetPath = GetRootDirectory().GetString() + assetPath;
				if(Import(fullAssetPath, fullAssetPath, ext, &errMsg)) {
					// Import was successful, attempt to preload again
					if(loadInfo) {
						ufile::remove_extension_from_filename(assetPath, std::vector<std::string> {ext});
						ClearCachedResult(GetIdentifierHash(assetPath));
						r = PreloadAsset(assetPath, IMMEDIATE_PRIORITY, std::move(loadInfo));
					}
				}
				else {
					r.result = PreloadResult::Result::ImportFailed;
					if(!errMsg.empty())
						r.errorMessage = errMsg;
				}
			}
		}
		else if(r.result == PreloadResult::Result::FileNotFound) {
			// Could not find asset at all, try all our known import formats.
			auto normPath = ToCacheIdentifier(assetPath);
			std::string errMsg;
			auto importFormat = [this, &normPath, &r, &assetPath, &loadInfo, &errMsg](const FormatExtensionInfo &extInfo) {
				if(extInfo.type != FormatExtensionInfo::Type::Import || !loadInfo)
					return false;
				auto extPath = normPath + '.' + extInfo.extension;
				if(Import(GetRootDirectory().GetString() + extPath, extPath, extInfo.extension, &errMsg)) {
					// Import was successful, attempt to preload again
					ClearCachedResult(GetIdentifierHash(assetPath));
					r = PreloadAsset(assetPath, IMMEDIATE_PRIORITY, std::move(loadInfo));
					return true;
				}
				return false;
			};
			auto importSuccess = false;
			for(auto &extInfo : m_extensions) {
				if(importFormat(extInfo)) {
					importSuccess = true;
					break;
				}
			}

			if(!importSuccess && loadInfo) {
				// Import failed, but we may still be able to find the asset in an external source
				auto formatExt = ImportAssetFilesFromExternalSource(normPath);
				if(formatExt.has_value()) {
					// Asset files have been found and copied to a location
					// where we should be able to find them; Re-try importing the asset
					auto it = FindExtension(m_extensions, *formatExt);
					if(it != m_extensions.end()) {
						auto &extInfo = *it;
						if(extInfo.type == FormatExtensionInfo::Type::Import)
							importFormat(*it); // Not a native format, still need to import
						ClearCachedResult(GetIdentifierHash(assetPath));
						r = PreloadAsset(assetPath, IMMEDIATE_PRIORITY, std::move(loadInfo)); // Native format, load directly
					}
				}
			}

			if(!r && !r.errorMessage)
				r.errorMessage = std::move(errMsg);
		}
	}
	if(optOutResult)
		*optOutResult = r;
	if(!r) {
		if(!loadInfo || (loadInfo->flags & util::AssetLoadFlags::DontCache) == util::AssetLoadFlags::None)
			CacheResult(assetPath, r);
		return nullptr;
	}
	return LoadAsset(assetPath, r, onLoaded, onFailure);
}
