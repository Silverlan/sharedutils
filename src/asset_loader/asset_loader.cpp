/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "sharedutils/asset_loader/asset_loader.hpp"
#include "sharedutils/asset_loader/asset_processor.hpp"
#include <cassert>
#pragma optimize("",off)
util::IAssetLoader::IAssetLoader()
	: m_pool{5}
{}

util::IAssetLoader::~IAssetLoader()
{}

void util::IAssetLoader::InvalidateLoadJob(const std::string &identifier)
{
	std::scoped_lock lock {m_assetIdToJobIdMutex};
	auto it = m_assetIdToJobId.find(identifier);
	if(it == m_assetIdToJobId.end())
		return;
	m_assetIdToJobId.erase(it);
}
std::optional<util::AssetLoadJobId> util::IAssetLoader::FindJobId(const std::string &identifier) const
{
	std::scoped_lock lock {m_assetIdToJobIdMutex};
	auto it = m_assetIdToJobId.find(identifier);
	return (it != m_assetIdToJobId.end()) ? it->second.jobId : std::optional<util::AssetLoadJobId>{};
}

std::optional<util::AssetLoadJobId> util::IAssetLoader::AddJob(
	const std::string &identifier,std::unique_ptr<IAssetProcessor> &&processor,AssetLoadJobPriority priority
)
{
	if(!identifier.empty())
	{
		std::scoped_lock lock {m_assetIdToJobIdMutex};
		auto itJob = m_assetIdToJobId.find(identifier);
		if(itJob != m_assetIdToJobId.end())
		{
			// Already queued up
			if(priority <= itJob->second.priority || itJob->second.complete)
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
		m_assetIdToJobIdMutex.lock();
			m_assetIdToJobId[identifier] = {jobId,priority};
		m_assetIdToJobIdMutex.unlock();
	}
	else
	{
		std::string identifier = "#anonymous_" +std::to_string(jobId);
		job.identifier = identifier;
		m_assetIdToJobIdMutex.lock();
			m_assetIdToJobId[identifier] = {jobId,priority};
		m_assetIdToJobIdMutex.unlock();
	}

	m_queueMutex.lock();
		m_jobs.emplace(std::move(job));
	m_queueMutex.unlock();

	m_pool.push([this](int id) {
		m_queueMutex.lock();
			auto job = std::move(const_cast<AssetLoadJob&>(m_jobs.top()));
			m_jobs.pop();
		m_queueMutex.unlock();
		
		m_assetIdToJobIdMutex.lock();
			auto it = m_assetIdToJobId.find(job.identifier);
			auto isValid = (it != m_assetIdToJobId.end() && it->second.jobId == job.jobId);
		m_assetIdToJobIdMutex.unlock();
		job.taskStartTime = std::chrono::high_resolution_clock::now();
		job.state = isValid ? (job.processor->Load() ? AssetLoadJob::State::Succeeded : AssetLoadJob::State::Failed) : AssetLoadJob::State::Cancelled;
		job.completionTime = std::chrono::high_resolution_clock::now();

		m_completeQueueMutex.lock();
			m_completeQueue.push(job);
			m_hasCompletedJobs = true;
			if(isValid)
			{
				m_assetIdToJobIdMutex.lock();
					auto it = m_assetIdToJobId.find(job.identifier);
					if(it != m_assetIdToJobId.end() && it->second.jobId == job.jobId)
						it->second.complete = true;
				m_assetIdToJobIdMutex.unlock();
			}
		m_completeQueueMutex.unlock();
		m_completeCondition.notify_one();
	});
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
		if(valid)
		{
			m_assetIdToJobId.erase(it);
			m_assetIdToJobIdMutex.unlock();
			if(job.state == AssetLoadJob::State::Succeeded && job.processor->Finalize())
			{
				success = true;
				onComplete(job,AssetLoadResult::Succeeded);
			}
			else
				onComplete(job,AssetLoadResult::Failed);
		}
		else
		{
			m_assetIdToJobIdMutex.unlock();
			onComplete(job,AssetLoadResult::Cancelled);
		}
		completeQueue.pop();
	}
}
#pragma optimize("",on)
