/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "sharedutils/asset_loader/asset_loader.hpp"
#include "sharedutils/asset_loader/asset_processor.hpp"
#include <sharedutils/util.h>
#include <iostream>
#include <cassert>

#undef AddJob

util::IAssetLoader::IAssetLoader(std::string name)
	: m_pool{10},m_name{std::move(name)}
{
	auto n = m_pool.size();
	std::string threadName = "asset_loader";
	if(!m_name.empty())
		threadName += '_' +m_name;
	for(auto i=decltype(n){0u};i<n;++i)
		util::set_thread_name(m_pool.get_thread(i),threadName);
}

util::IAssetLoader::~IAssetLoader()
{
	m_pool.stop();
}

bool util::IAssetLoader::InvalidateLoadJob(const std::string &identifier)
{
	std::scoped_lock lock {m_assetIdToJobIdMutex};
	auto it = m_assetIdToJobId.find(identifier);
	if(it == m_assetIdToJobId.end())
		return false;
	m_assetIdToJobId.erase(it);
	return true;
}
std::optional<util::AssetLoadJobId> util::IAssetLoader::FindJobId(const std::string &identifier) const
{
	std::scoped_lock lock {m_assetIdToJobIdMutex};
	auto it = m_assetIdToJobId.find(identifier);
	return (it != m_assetIdToJobId.end()) ? it->second.jobId : std::optional<util::AssetLoadJobId>{};
}

void util::IAssetLoader::SetMultiThreadingEnabled(bool enabled) {m_multiThreadingEnabled = enabled;}
bool util::IAssetLoader::IsMultiThreadingEnabled() const {return m_multiThreadingEnabled;}

util::AssetLoadState util::IAssetLoader::GetLoadState(const std::string &identifier) const
{
	std::scoped_lock lock {m_assetIdToJobIdMutex};
	auto it = m_assetIdToJobId.find(identifier);
	if(it == m_assetIdToJobId.end())
		return AssetLoadState::NotQueued;
	auto &jobInfo = it->second;
	return (jobInfo.state == QueuedJobInfo::State::Complete) ? AssetLoadState::LoadedAndPendingForCompletion : AssetLoadState::Loading;
}

std::optional<util::AssetLoadJobId> util::IAssetLoader::AddJob(
	const std::string &identifier,std::unique_ptr<IAssetProcessor> &&processor,AssetLoadJobPriority priority
)
{
	std::scoped_lock lock {m_assetIdToJobIdMutex};
	std::unique_lock jobLock {m_jobMutex};
	if(!identifier.empty())
	{
		auto itJob = m_assetIdToJobId.find(identifier);
		if(itJob != m_assetIdToJobId.end())
		{
			// Already queued up
			if(priority <= itJob->second.priority || itJob->second.state != QueuedJobInfo::State::Pending)
				return itJob->second.jobId;
			// New job has higher priority; The old job will be invalidated and the
			// new job added
		}
	}
	
	auto jobId = m_nextJobId++;
	AssetLoadJob job {};
	job.processor = std::move(processor);
	job.priority = priority;
	job.jobId = jobId;
	job.queueStartTime = std::chrono::high_resolution_clock::now();
	if(!identifier.empty())
	{
		job.identifier = identifier;
		m_assetIdToJobId[identifier] = {jobId,priority};
	}
	else
	{
		std::string identifier = "#anonymous_" +std::to_string(jobId);
		job.identifier = identifier;
		m_assetIdToJobId[identifier] = {jobId,priority};
	}

	m_queueMutex.lock();
		m_jobs.emplace(std::move(job));
	m_queueMutex.unlock();

	auto f = [this](int id) {
		m_queueMutex.lock();
			auto job = std::move(const_cast<AssetLoadJob&>(m_jobs.top()));
			m_jobs.pop();
		m_queueMutex.unlock();
		
		m_assetIdToJobIdMutex.lock();
			auto it = m_assetIdToJobId.find(job.identifier);
			auto isValid = (it != m_assetIdToJobId.end() && it->second.jobId == job.jobId);
			if(it != m_assetIdToJobId.end())
				it->second.state = QueuedJobInfo::State::InProgress;
		m_assetIdToJobIdMutex.unlock();
		job.taskStartTime = std::chrono::high_resolution_clock::now();
		if(!isValid)
			job.state = AssetLoadJob::State::Cancelled;
		else
		{
			auto success = false;
			try
			{
				success = job.processor->Load();
			}
			catch(const std::exception &e)
			{
				success = false;
				std::cout<<"Asset loader exception: "<<e.what()<<std::endl;
			}
			catch(...)
			{
				success = false;
				std::cout<<"Unknown exception in asset loader!"<<std::endl;
			}
			job.state = success ? AssetLoadJob::State::Succeeded : AssetLoadJob::State::Failed;
		}
		job.completionTime = std::chrono::high_resolution_clock::now();

		m_completeQueueMutex.lock();
			m_completeQueue.push(job);
			m_hasCompletedJobs = true;
			if(isValid)
			{
				m_assetIdToJobIdMutex.lock();
					auto it = m_assetIdToJobId.find(job.identifier);
					if(it != m_assetIdToJobId.end() && it->second.jobId == job.jobId)
					{
						it->second.state = QueuedJobInfo::State::Complete;
						if(job.processor)
							job.processor->Close();
					}
				m_assetIdToJobIdMutex.unlock();
			}
		m_completeQueueMutex.unlock();
		m_completeCondition.notify_one();
	};
	if(IsMultiThreadingEnabled())
		m_pool.push(std::move(f));
	else
		f(0);
	return jobId;
}

void util::IAssetLoader::Poll(
	const std::function<void(const AssetLoadJob&,AssetLoadResult)> &onComplete,
	AssetLoaderWaitMode wait
)
{
	if(wait == AssetLoaderWaitMode::All)
	{
		for(;;)
		{
			m_queueMutex.lock();
				auto hasJobsRemaining = !m_jobs.empty();
			m_queueMutex.unlock();
			if(!hasJobsRemaining && !m_hasCompletedJobs)
				return;
			Poll(onComplete,AssetLoaderWaitMode::Single);
		}
		return; // Unreachable
	}
	if(wait == AssetLoaderWaitMode::Single)
	{
		std::unique_lock<std::mutex> lock {m_completeQueueMutex};
		m_completeCondition.wait(lock,[this]() {
			return m_hasCompletedJobs == true;
		});
	}
	if(!m_hasCompletedJobs)
		return;
	m_completeQueueMutex.lock();
		m_hasCompletedJobs = false;
		auto completeQueue = std::move(m_completeQueue);
		m_completeQueue = {};
	m_completeQueueMutex.unlock();

	while(!completeQueue.empty())
	{
		auto &job = completeQueue.front();
		job.completionTime = std::chrono::high_resolution_clock::now();
		auto success = false;
		
		assert(job.processor != nullptr);

		m_assetIdToJobIdMutex.lock();
		auto it = m_assetIdToJobId.find(job.identifier);
		// If an asset is queued up multiple times, we only care about the latest job
		// and disregard previous ones.
		auto valid = (it != m_assetIdToJobId.end() && it->second.jobId == job.jobId);
		m_assetIdToJobIdMutex.unlock();

		if(valid)
		{
			if(job.state == AssetLoadJob::State::Succeeded && job.processor->Finalize())
			{
				success = true;
				onComplete(job,AssetLoadResult::Succeeded);

			}
			else
				onComplete(job,AssetLoadResult::Failed);
		}
		else
			onComplete(job,AssetLoadResult::Cancelled);
		{
			m_assetIdToJobIdMutex.lock();
				auto it = m_assetIdToJobId.find(job.identifier);
				if(it != m_assetIdToJobId.end())
					m_assetIdToJobId.erase(it);
			m_assetIdToJobIdMutex.unlock();
		}
		completeQueue.pop();
	}
}
