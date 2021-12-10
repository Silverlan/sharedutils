/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "sharedutils/asset_loader/file_asset_manager.hpp"
#include "sharedutils/asset_loader/file_asset_processor.hpp"
#include "sharedutils/asset_loader/asset_load_info.hpp"
#include "sharedutils/util_ifile.hpp"

#undef AddJob
#pragma optimize("",off)

#define ENABLE_VERBOSE_OUTPUT

util::FileAssetManager::FileAssetManager()
	: util::IAssetManager{},m_mainThreadId{std::this_thread::get_id()}
{}
util::FileAssetManager::~FileAssetManager()
{
	m_loader = nullptr;
}
void util::FileAssetManager::ValidateMainThread()
{
	if(std::this_thread::get_id() != m_mainThreadId)
		throw std::runtime_error{"This action is only allowed from the same thread that created the asset manager!"};
}
void util::FileAssetManager::SetRootDirectory(const std::string &dir) {m_rootDir = util::Path::CreatePath(dir);}
const util::Path &util::FileAssetManager::GetRootDirectory() const {return m_rootDir;}

void util::FileAssetManager::SetFileHandler(std::unique_ptr<AssetFileHandler> &&fileHandler) {m_fileHandler = std::move(fileHandler);}

void util::FileAssetManager::RemoveFromCache(const std::string &path)
{
	IAssetManager::RemoveFromCache(path);
	m_loader->InvalidateLoadJob(path);
}
void util::FileAssetManager::RegisterFormatHandler(
	const std::string &ext,const std::function<std::unique_ptr<IAssetFormatHandler>(util::IAssetManager&)> &factory,
	AssetFormatType formatType
)
{
	GetLoader().RegisterFormatHandler(ext,factory);
	RegisterFileExtension(ext,formatType);
}
void util::FileAssetManager::CallOnLoad(const std::string &path,const std::function<void(util::Asset*,AssetLoadResult)> &onLoad)
{
	auto hash = GetIdentifierHash(path);
	auto it = m_callOnLoad.find(hash);
	if(it == m_callOnLoad.end())
		it = m_callOnLoad.insert(std::make_pair(hash,std::vector<std::function<void(util::Asset*,AssetLoadResult)>>{})).first;
	it->second.push_back(onLoad);
}
util::FileAssetManager::PreloadResult util::FileAssetManager::PreloadAsset(const std::string &strPath,std::unique_ptr<util::AssetLoadInfo> &&loadInfo)
{
	return PreloadAsset(strPath,DEFAULT_PRIORITY,std::move(loadInfo));
}
util::FileAssetManager::PreloadResult util::FileAssetManager::PreloadAsset(
	const std::string &cacheName,std::unique_ptr<ufile::IFile> &&file,const std::string &ext,util::AssetLoadJobPriority priority,
	std::unique_ptr<util::AssetLoadInfo> &&loadInfo
)
{
	auto hash = GetIdentifierHash(cacheName);
	auto jobId = m_loader->AddJob(ToCacheIdentifier(cacheName),ext,std::move(file),priority,[this,&loadInfo,hash](util::IAssetProcessor &processor) {
		auto &faProcessor = static_cast<FileAssetProcessor&>(processor);
		faProcessor.loadInfo = std::move(loadInfo);
		InitializeProcessor(processor);
		faProcessor.onLoaded = [this,&faProcessor,hash](util::Asset *asset,AssetLoadResult result) {
			auto &loadInfo = *faProcessor.loadInfo;
			if(asset)
			{
				if(loadInfo.onLoaded)
					loadInfo.onLoaded(*asset);
			}
			else if(loadInfo.onFailure)
				loadInfo.onFailure();

			if(asset)
			{
				auto it = m_callOnLoad.find(hash);
				if(it != m_callOnLoad.end())
				{
					auto functions = std::move(it->second);
					m_callOnLoad.erase(it);
					for(auto &f : functions)
						f(asset,result);
				}
			}
		};
	});
	if(!jobId.has_value())
		return PreloadResult{{},false};
	return PreloadResult{jobId,true};
}
util::FileAssetManager::PreloadResult util::FileAssetManager::PreloadAsset(
	const std::string &strPath,util::AssetLoadJobPriority priority,std::unique_ptr<util::AssetLoadInfo> &&loadInfo
)
{
	auto *asset = FindCachedAsset(strPath);
	if(asset)
		return PreloadResult{{},true};
	auto path = util::Path::CreateFile(strPath);
	if(!loadInfo || !umath::is_flag_set(loadInfo->flags,AssetLoadFlags::AbsolutePath))
		path = m_rootDir +path;
	auto ext = path.GetFileExtension();
	AssetFormatType formatType;
	if(ext.has_value())
	{
		// Confirm that this is a valid texture format extension
		auto it = std::find_if(m_extensions.begin(),m_extensions.end(),[&ext](const FormatExtensionInfo &extInfo) {
			return *ext == extInfo.extension;
		});
		if(it == m_extensions.end())
			ext = {};
		else
			formatType = it->formatType;
	}
	if(!ext.has_value())
	{
		// No valid extension found, search for all known extensions
		for(auto &extInfo : m_extensions)
		{
			auto extPath = path +("." +extInfo.extension);
			if(m_fileHandler->exists(extPath.GetString()))
			{
				path = std::move(extPath);
				ext = extInfo.extension;
				formatType = extInfo.formatType;
				break;
			}
		}
	}
	if(!ext.has_value())
		return PreloadResult{{},false};
	auto f = m_fileHandler->open(path.GetString(),formatType);
	if(!f)
		return PreloadResult{{},false};
	return PreloadAsset(strPath,std::move(f),*ext,priority,std::move(loadInfo));
}
util::AssetObject util::FileAssetManager::LoadAsset(
	const std::string &path,const PreloadResult &r,
	std::function<void(util::Asset&)> onLoaded,
	std::function<void()> onFailure
)
{
	ValidateMainThread();
	if(r.success == false)
	{
		if(onFailure)
			onFailure();
		return nullptr;
	}
	if(!r.jobId.has_value())
	{
		// Already fully loaded
		auto *asset = FindCachedAsset(path);
		if(onLoaded)
			onLoaded(*asset);
		return asset->assetObject;
	}
	auto identifier = ToCacheIdentifier(path);
	auto jobId = *r.jobId;

#ifdef ENABLE_VERBOSE_OUTPUT
	auto t = std::chrono::high_resolution_clock::now();
#endif
	auto o = Poll(jobId,util::AssetLoaderWaitMode::None);
#ifdef ENABLE_VERBOSE_OUTPUT
	if(o)
	{
		auto dt = std::chrono::high_resolution_clock::now() -t;
		std::cout<<"Waited "<<(dt.count() /1'000'000'000.0)<<" seconds for asset '"<<identifier<<"'!"<<std::endl;
	}
#endif
	return o;
}

void util::FileAssetManager::Poll() {Poll({},util::AssetLoaderWaitMode::None);}

void util::FileAssetManager::WaitForAllPendingCompleted() {Poll({},util::AssetLoaderWaitMode::All);}

util::AssetState util::FileAssetManager::GetAssetState(const std::string &assetName) const
{
	auto hash = GetIdentifierHash(assetName);
	auto it = m_cache.find(hash);
	if(it != m_cache.end())
		return AssetState::Loaded;
	auto loadState = m_loader->GetLoadState(ToCacheIdentifier(assetName));
	switch(loadState)
	{
	case AssetLoadState::NotQueued:
		return AssetState::NotLoaded;
	case AssetLoadState::LoadedAndPendingForCompletion:
	case AssetLoadState::Loading:
		return AssetState::Loading;
	}
	// Unreachable
	return AssetState::NotLoaded;
}

std::optional<std::string> util::FileAssetManager::FindAssetFilePath(const std::string &assetName) const
{
	auto normalizedName = ToCacheIdentifier(assetName);
	for(auto &extInfo : m_extensions)
	{
		auto &ext = extInfo.extension;
		auto extName = normalizedName +'.' +ext;
		if(!m_fileHandler->exists(extName))
			continue;
		return extName;
	}
	return {};
}

util::AssetObject util::FileAssetManager::Poll(std::optional<util::AssetLoadJobId> untilJob,util::AssetLoaderWaitMode wait)
{
	if(untilJob.has_value() && wait == util::AssetLoaderWaitMode::None)
		wait = util::AssetLoaderWaitMode::Single;
	util::AssetObject targetAsset = nullptr;
	do
	{
		m_loader->Poll([this,&untilJob,&targetAsset](const util::AssetLoadJob &job,AssetLoadResult result) {
			auto &processor = *static_cast<FileAssetProcessor*>(job.processor.get());
			util::Asset *passet = nullptr;
			switch(result)
			{
			case AssetLoadResult::Succeeded:
			{
#ifdef ENABLE_VERBOSE_OUTPUT
				auto dtQueue = job.completionTime -job.queueStartTime;
				auto dtTask = job.completionTime -job.taskStartTime;
				std::cout<<job.identifier<<" has been loaded!"<<std::endl;
				std::cout<<"Time since job has been queued to completion: "<<(dtQueue.count() /1'000'000'000.0)<<std::endl;
				std::cout<<"Time since task has been started to completion: "<<(dtTask.count() /1'000'000'000.0)<<std::endl;
#endif
				auto asset = std::make_shared<util::Asset>();
			
				auto &loadInfo = *processor.loadInfo;
				if(!umath::is_flag_set(loadInfo.flags,AssetLoadFlags::DontCache))
					asset->index = AddToCache(job.identifier,asset);
				auto assetObject = InitializeAsset(*asset,job);
				asset->assetObject = assetObject;
				if(untilJob.has_value() && job.jobId == *untilJob)
				{
					targetAsset = asset->assetObject;
					untilJob = {};
				}
				passet = asset.get();
				break;
			}
			default:
			{
#ifdef ENABLE_VERBOSE_OUTPUT
				std::cout<<job.identifier<<" has failed!"<<std::endl;
#endif

				if(untilJob.has_value() && job.jobId == *untilJob)
				{
					targetAsset = nullptr;
					untilJob = {};
				}
				break;
			}
			}
			if(processor.onLoaded)
				processor.onLoaded(passet,result);
		},wait);
	}
	while(untilJob.has_value());
	return targetAsset;
}

util::AssetObject util::FileAssetManager::LoadAsset(
	const std::string &cacheName,std::unique_ptr<ufile::IFile> &&file,const std::string &ext,std::unique_ptr<util::AssetLoadInfo> &&loadInfo
)
{
	ValidateMainThread();
	auto onLoaded = loadInfo ? loadInfo->onLoaded : nullptr;
	auto onFailure = loadInfo ? loadInfo->onFailure : nullptr;
	auto r = PreloadAsset(cacheName,std::move(file),ext,IMMEDIATE_PRIORITY,std::move(loadInfo));
	if(r.success == false)
		return nullptr;
	return LoadAsset(cacheName,r,onLoaded,onFailure);
}
util::AssetObject util::FileAssetManager::LoadAsset(const std::string &path,std::unique_ptr<util::AssetLoadInfo> &&loadInfo)
{
	ValidateMainThread();
	auto onLoaded = loadInfo ? loadInfo->onLoaded : nullptr;
	auto onFailure = loadInfo ? loadInfo->onFailure : nullptr;
	auto r = PreloadAsset(path,IMMEDIATE_PRIORITY,std::move(loadInfo));
	return LoadAsset(path,r,onLoaded,onFailure);
}
#pragma optimize("",on)
