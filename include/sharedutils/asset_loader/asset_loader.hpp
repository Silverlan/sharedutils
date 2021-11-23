/* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __ASSET_LOADER_HPP__
#define __ASSET_LOADER_HPP__

#include "sharedutils/utildefinitions.h"
#include "sharedutils/asset_loader/asset_load_job.hpp"
#include "sharedutils/ctpl_stl.h"
#include <string>
#include <memory>
#include <functional>
#include <mutex>

#undef AddJob

namespace prosper {class IPrContext;};
namespace ufile {struct IFile;};
namespace util
{
	using AssetLoadJobId = uint64_t;
	using AssetLoadJobPriority = int32_t;
	class IAssetProcessor;
	class DLLSHUTIL IAssetLoader
	{
	public:
		IAssetLoader();
		virtual ~IAssetLoader();

		void Poll(
			const std::function<void(const AssetLoadJob&)> &onComplete,
			const std::function<void(const AssetLoadJob&)> &onFailed
		);
		bool AddJob(
			const std::string &identifier,std::unique_ptr<IAssetProcessor> &&processor,AssetLoadJobPriority priority=0
		);
	private:
		ctpl::thread_pool m_pool;
		std::mutex m_queueMutex;
		std::priority_queue<AssetLoadJob,std::vector<AssetLoadJob>,CompareTextureLoadJob> m_jobs;

		std::mutex m_completeQueueMutex;
		std::queue<AssetLoadJob> m_completeQueue;
		std::atomic<bool> m_hasCompletedJobs = false;

		struct QueuedJobInfo
		{
			AssetLoadJobId jobId;
			AssetLoadJobPriority priority;
		};
		std::unordered_map<std::string,QueuedJobInfo> m_texIdToJobId;
		AssetLoadJobId m_nextJobId = 0;
	};
};

#endif
