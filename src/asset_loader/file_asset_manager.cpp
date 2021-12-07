/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "sharedutils/asset_loader/file_asset_manager.hpp"
#include "sharedutils/asset_loader/file_asset_processor.hpp"
#include "sharedutils/asset_loader/asset_load_info.hpp"
#include "sharedutils/util_ifile.hpp"

#undef AddJob

util::FileAssetManager::FileAssetManager()
	: util::IAssetManager{}
{}
void util::FileAssetManager::SetRootDirectory(const std::string &dir) {m_rootDir = util::Path::CreatePath(dir);}
const util::Path &util::FileAssetManager::GetRootDirectory() const {return m_rootDir;}

void util::FileAssetManager::SetFileHandler(std::unique_ptr<AssetFileHandler> &&fileHandler) {m_fileHandler = std::move(fileHandler);}

void util::FileAssetManager::RemoveFromCache(const std::string &path)
{
	IAssetManager::RemoveFromCache(path);
	m_loader->InvalidateLoadJob(path);
}
void util::FileAssetManager::RegisterFormatHandler(const std::string &ext,const std::function<std::unique_ptr<IAssetFormatHandler>(util::IAssetManager&)> &factory)
{
	GetLoader().RegisterFormatHandler(ext,factory);
	RegisterFileExtension(ext);
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
	auto jobId = m_loader->AddJob(ToCacheIdentifier(cacheName),ext,std::move(file),priority,[this,&loadInfo](util::IAssetProcessor &processor) {
		auto &faProcessor = static_cast<FileAssetProcessor&>(processor);
		faProcessor.loadInfo = std::move(loadInfo);
		InitializeProcessor(processor);
		faProcessor.onLoaded = [&faProcessor](util::Asset *asset) {
			auto &loadInfo = *faProcessor.loadInfo;
			if(asset)
			{
				if(loadInfo.onLoaded)
					loadInfo.onLoaded(*asset);
			}
			else if(loadInfo.onFailure)
				loadInfo.onFailure();
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
	if(ext.has_value())
	{
		// Confirm that this is a valid texture format extension
		auto it = std::find_if(m_extensions.begin(),m_extensions.end(),[&ext](const std::pair<std::string,size_t> &pair) {
			return *ext == pair.first;
		});
		if(it == m_extensions.end())
			ext = {};
	}
	if(!ext.has_value())
	{
		// No valid extension found, search for all known extensions
		for(auto &pair : m_extensions)
		{
			auto extPath = path +("." +pair.first);
			if(m_fileHandler->exists(extPath.GetString()))
			{
				path = std::move(extPath);
				ext = pair.first;
				break;
			}
		}
	}
	if(!ext.has_value())
		return PreloadResult{{},false};
	auto f = m_fileHandler->open(path.GetString());
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

	return Poll(jobId,util::AssetLoaderWaitMode::None);
}

void util::FileAssetManager::Poll() {Poll({},util::AssetLoaderWaitMode::None);}

void util::FileAssetManager::WaitForAllPendingCompleted() {Poll({},util::AssetLoaderWaitMode::All);}

util::AssetObject util::FileAssetManager::Poll(std::optional<util::AssetLoadJobId> untilJob,util::AssetLoaderWaitMode wait)
{
	util::AssetObject targetAsset = nullptr;
	do
	{
		m_loader->Poll([this,&untilJob,&targetAsset](const util::AssetLoadJob &job,IAssetLoader::AssetLoadResult result) {
			switch(result)
			{
			case IAssetLoader::AssetLoadResult::Succeeded:
			{
#ifdef ENABLE_VERBOSE_OUTPUT
				auto dtQueue = job.completionTime -job.queueStartTime;
				auto dtTask = job.completionTime -job.taskStartTime;
				std::cout<<job.identifier<<" has been loaded!"<<std::endl;
				std::cout<<"Time since job has been queued to completion: "<<(dtQueue.count() /1'000'000'000.0)<<std::endl;
				std::cout<<"Time since task has been started to completion: "<<(dtTask.count() /1'000'000'000.0)<<std::endl;
#endif
				auto assetObject = InitializeAsset(job);
				auto asset = std::make_shared<util::Asset>();
				asset->assetObject = assetObject;
			
				auto &processor = *static_cast<FileAssetProcessor*>(job.processor.get());
				auto &loadInfo = *processor.loadInfo;
				if(!umath::is_flag_set(loadInfo.flags,AssetLoadFlags::DontCache))
					AddToCache(job.identifier,asset);
				if(untilJob.has_value() && job.jobId == *untilJob)
				{
					targetAsset = asset->assetObject;
					untilJob = {};
				}
				if(processor.onLoaded)
					processor.onLoaded(asset.get());
				break;
			}
			default:
			{
#ifdef ENABLE_VERBOSE_OUTPUT
				std::cout<<job.identifier<<" has failed!"<<std::endl;
#endif

				auto &processor = *static_cast<FileAssetProcessor*>(job.processor.get());
				if(untilJob.has_value() && job.jobId == *untilJob)
				{
					targetAsset = nullptr;
					untilJob = {};
				}
				if(processor.onLoaded)
					processor.onLoaded(nullptr);
				break;
			}
			}
		},wait);
	}
	while(untilJob.has_value());
	return targetAsset;
}

util::AssetObject util::FileAssetManager::LoadAsset(
	const std::string &cacheName,std::unique_ptr<ufile::IFile> &&file,const std::string &ext,std::unique_ptr<util::AssetLoadInfo> &&loadInfo
)
{
	auto onLoaded = loadInfo ? loadInfo->onLoaded : nullptr;
	auto onFailure = loadInfo ? loadInfo->onFailure : nullptr;
	auto r = PreloadAsset(cacheName,std::move(file),ext,IMMEDIATE_PRIORITY,std::move(loadInfo));
	if(r.success == false)
		return nullptr;
	return LoadAsset(cacheName,r,onLoaded,onFailure);
}
util::AssetObject util::FileAssetManager::LoadAsset(const std::string &path,std::unique_ptr<util::AssetLoadInfo> &&loadInfo)
{
	auto onLoaded = loadInfo ? loadInfo->onLoaded : nullptr;
	auto onFailure = loadInfo ? loadInfo->onFailure : nullptr;
	auto r = PreloadAsset(path,IMMEDIATE_PRIORITY,std::move(loadInfo));
	return LoadAsset(path,r,onLoaded,onFailure);
}
