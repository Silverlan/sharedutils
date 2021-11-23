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

bool util::IAssetLoader::AddJob(
	const std::string &identifier,std::unique_ptr<IAssetProcessor> &&processor,AssetLoadJobPriority priority
)
{
	auto itJob = m_assetIdToJobId.find(identifier);
	if(itJob != m_assetIdToJobId.end() && itJob->second.priority >= priority)
		return true; // Already queued up with the same (or higher) priority, no point in adding it again
	
	auto jobId = m_nextJobId++;
	AssetLoadJob job {};
	job.processor = std::move(processor);
	job.priority = priority;
	job.jobId = jobId;
	job.identifier = identifier;
	job.queueStartTime = std::chrono::high_resolution_clock::now();
	m_assetIdToJobId[identifier] = {jobId,priority};

	m_queueMutex.lock();
		m_jobs.emplace(std::move(job));
	m_queueMutex.unlock();

	m_pool.push([this](int id) {
		m_queueMutex.lock();
			auto job = std::move(const_cast<AssetLoadJob&>(m_jobs.top()));
			m_jobs.pop();
		m_queueMutex.unlock();
		
		job.taskStartTime = std::chrono::high_resolution_clock::now();
		job.state = job.processor->Load() ? AssetLoadJob::State::Succeeded : AssetLoadJob::State::Failed;
		job.completionTime = std::chrono::high_resolution_clock::now();

		m_completeQueueMutex.lock();
			m_completeQueue.push(job);
			m_hasCompletedJobs = true;
		m_completeQueueMutex.unlock();
	});
	return true;
}

void util::IAssetLoader::Poll(
	const std::function<void(const AssetLoadJob&)> &onComplete,
	const std::function<void(const AssetLoadJob&)> &onFailed
)
{
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
		if(job.state == AssetLoadJob::State::Succeeded)
		{
			assert(job.processor != nullptr);
			auto it = m_assetIdToJobId.find(job.identifier);
			// If an asset is queued up multiple times, we only care about the latest job
			// and disregard previous ones.
			auto valid = (it != m_assetIdToJobId.end() && it->second.jobId == job.jobId);
			if(valid)
			{
				m_assetIdToJobId.erase(it);
				if(job.processor->Finalize())
				{
					success = true;
					onComplete(job);
				}
			}
		}
		if(!success)
			onFailed(job);
		completeQueue.pop();
	}
}
#pragma optimize("",on)
