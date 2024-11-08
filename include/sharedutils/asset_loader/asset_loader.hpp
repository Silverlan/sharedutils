/* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __ASSET_LOADER_HPP__
#define __ASSET_LOADER_HPP__

#include "sharedutils/utildefinitions.h"
#include "sharedutils/asset_loader/asset_load_job.hpp"
#include "sharedutils/ctpl_stl.h"
#include "sharedutils/util_log.hpp"
#include <string>
#include <memory>
#include <optional>
#include <functional>
#include <unordered_set>
#include <mutex>
#include <unordered_map>

#undef AddJob

namespace prosper {
	class IPrContext;
};
namespace ufile {
	struct IFile;
};
namespace util {
	using AssetLoadJobId = uint64_t;
	using AssetLoadJobPriority = int32_t;
	class IAssetProcessor;
	enum class AssetLoaderWaitMode : uint8_t { None = 0, Single, All };
	enum class AssetLoadResult : uint8_t {
		Succeeded = 0,
		Failed,
		Cancelled,

		NotStarted,
		Loading
	};
	enum class AssetLoadState : uint8_t { NotQueued = 0, Loading, LoadedAndPendingForCompletion };
	class DLLSHUTIL IAssetLoader {
	  public:
		IAssetLoader(std::string name = "");
		virtual ~IAssetLoader();
		void Poll(const std::function<void(const AssetLoadJob &, AssetLoadResult, std::optional<std::string>)> &onComplete, AssetLoaderWaitMode wait = AssetLoaderWaitMode::None);
		std::optional<AssetLoadJobId> AddJob(const std::string &identifier, std::unique_ptr<IAssetProcessor> &&processor, AssetLoadJobPriority priority = 0);
		std::optional<AssetLoadJobId> FindJobId(const std::string &identifier) const;
		bool InvalidateLoadJob(const std::string &identifier);

		AssetLoadState GetLoadState(const std::string &identifier) const;

		void SetMultiThreadingEnabled(bool enabled);
		bool IsMultiThreadingEnabled() const;

		void SetLogHandler(const util::LogHandler &logHandler);
		bool ShouldLog() const;

		const std::string &GetName() const { return m_name; }
		std::recursive_mutex &GetJobGuardMutex() { return m_assetIdToJobIdMutex; }
		bool IsJobPending(AssetLoadJobId jobId) const;

		// For debugging purposes only
		bool HasPendingJobs() const;
		bool HasCompletedJobs() const;
		//
	  private:
		std::atomic<bool> m_multiThreadingEnabled = true;
		std::string m_name;
		ctpl::thread_pool m_pool;
		std::mutex m_jobMutex;
		std::mutex m_queueMutex;
		std::priority_queue<AssetLoadJob, std::vector<AssetLoadJob>, CompareTextureLoadJob> m_jobs;

		mutable std::mutex m_completeQueueMutex;
		std::queue<AssetLoadJob> m_completeQueue;
		std::atomic<bool> m_hasCompletedJobs = false;
		std::condition_variable m_completeCondition;

		struct QueuedJobInfo {
			enum class State : uint8_t { Pending = 0, InProgress, Complete };
			AssetLoadJobId jobId;
			AssetLoadJobPriority priority;
			State state = State::Pending;
		};
		mutable std::recursive_mutex m_assetIdToJobIdMutex;
		std::unordered_map<std::string, QueuedJobInfo> m_assetIdToJobId;
		std::unordered_set<AssetLoadJobId> m_pendingAssetJobs;
		AssetLoadJobId m_nextJobId = 0;
		util::LogHandler m_logHandler;
	};
};

#endif
