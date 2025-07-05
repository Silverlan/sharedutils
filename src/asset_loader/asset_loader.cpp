// SPDX-FileCopyrightText: (c) 2021 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

#include "sharedutils/asset_loader/asset_loader.hpp"
#include "sharedutils/asset_loader/asset_processor.hpp"
#include <sharedutils/util.h>
#include <iostream>
#include <cassert>

#undef AddJob

util::IAssetLoader::IAssetLoader(std::string name) : m_pool {10}, m_name {std::move(name)}
{
	auto n = m_pool.size();
	std::string threadName = "asset_loader";
	if(!m_name.empty())
		threadName += '_' + m_name;
	for(auto i = decltype(n) {0u}; i < n; ++i)
		util::set_thread_name(m_pool.get_thread(i), threadName);
}

util::IAssetLoader::~IAssetLoader() { m_pool.stop(); }

bool util::IAssetLoader::InvalidateLoadJob(const std::string &identifier)
{
	std::scoped_lock lock {m_assetIdToJobIdMutex};
	auto it = m_assetIdToJobId.find(identifier);
	if(it == m_assetIdToJobId.end())
		return false;
	auto itPending = m_pendingAssetJobs.find(it->second.jobId);
	if(itPending != m_pendingAssetJobs.end())
		m_pendingAssetJobs.erase(itPending);
	m_assetIdToJobId.erase(it);
	return true;
}
std::optional<util::AssetLoadJobId> util::IAssetLoader::FindJobId(const std::string &identifier) const
{
	std::scoped_lock lock {m_assetIdToJobIdMutex};
	auto it = m_assetIdToJobId.find(identifier);
	return (it != m_assetIdToJobId.end()) ? it->second.jobId : std::optional<util::AssetLoadJobId> {};
}

void util::IAssetLoader::SetMultiThreadingEnabled(bool enabled) { m_multiThreadingEnabled = enabled; }
bool util::IAssetLoader::IsMultiThreadingEnabled() const { return m_multiThreadingEnabled; }

util::AssetLoadState util::IAssetLoader::GetLoadState(const std::string &identifier) const
{
	std::scoped_lock lock {m_assetIdToJobIdMutex};
	auto it = m_assetIdToJobId.find(identifier);
	if(it == m_assetIdToJobId.end())
		return AssetLoadState::NotQueued;
	auto &jobInfo = it->second;
	return (jobInfo.state == QueuedJobInfo::State::Complete) ? AssetLoadState::LoadedAndPendingForCompletion : AssetLoadState::Loading;
}

void util::IAssetLoader::SetLogHandler(const util::LogHandler &logHandler) { m_logHandler = logHandler; }
bool util::IAssetLoader::ShouldLog() const { return m_logHandler != nullptr; }

std::optional<util::AssetLoadJobId> util::IAssetLoader::AddJob(const std::string &identifier, std::unique_ptr<IAssetProcessor> &&processor, AssetLoadJobPriority priority)
{
	std::scoped_lock lock {m_assetIdToJobIdMutex};
	std::unique_lock jobLock {m_jobMutex};
	if(!identifier.empty()) {
		auto itJob = m_assetIdToJobId.find(identifier);
		if(itJob != m_assetIdToJobId.end()) {
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
	if(!identifier.empty()) {
		job.identifier = identifier;
		m_assetIdToJobId[identifier] = {jobId, priority};
		m_pendingAssetJobs.insert(jobId);
	}
	else {
		std::string identifier = "#anonymous_" + std::to_string(jobId);
		job.identifier = identifier;
		m_assetIdToJobId[identifier] = {jobId, priority};
		m_pendingAssetJobs.insert(jobId);
	}

	m_queueMutex.lock();
	m_jobs.emplace(std::move(job));
	m_queueMutex.unlock();

	auto f = [this](int id) {
		m_queueMutex.lock();
		auto job = std::move(const_cast<AssetLoadJob &>(m_jobs.top()));
		m_jobs.pop();
		m_queueMutex.unlock();

		m_assetIdToJobIdMutex.lock();
		auto it = m_assetIdToJobId.find(job.identifier);
		auto isValid = (it != m_assetIdToJobId.end() && it->second.jobId == job.jobId);
		if(it != m_assetIdToJobId.end())
			it->second.state = QueuedJobInfo::State::InProgress;
		m_assetIdToJobIdMutex.unlock();
		job.taskStartTime = std::chrono::high_resolution_clock::now();

		if(ShouldLog())
			m_logHandler("Running job " + std::to_string(job.jobId) + "...!", util::LogSeverity::Debug);

		if(!isValid)
			job.state = AssetLoadJob::State::Cancelled;
		else {
			auto success = false;
			try {
				success = job.processor->Load();
			}
			catch(const std::exception &e) {
				success = false;
				std::cout << "Asset loader exception: " << e.what() << std::endl;
			}
			catch(...) {
				success = false;
				std::cout << "Unknown exception in asset loader!" << std::endl;
			}
			job.state = success ? AssetLoadJob::State::Succeeded : AssetLoadJob::State::Failed;
		}
		job.completionTime = std::chrono::high_resolution_clock::now();

		m_completeQueueMutex.lock();

		if(ShouldLog())
			m_logHandler("Job " + std::to_string(job.jobId) + " has completed with result: " + std::to_string(+umath::to_integral(job.state)) + "!", util::LogSeverity::Debug);

		m_completeQueue.push(job);
		m_hasCompletedJobs = true;
		if(isValid) {
			m_assetIdToJobIdMutex.lock();
			auto it = m_assetIdToJobId.find(job.identifier);
			if(it != m_assetIdToJobId.end() && it->second.jobId == job.jobId) {
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

bool util::IAssetLoader::IsJobPending(AssetLoadJobId jobId) const
{
	m_assetIdToJobIdMutex.lock();
	auto res = (m_pendingAssetJobs.find(jobId) != m_pendingAssetJobs.end());
	m_assetIdToJobIdMutex.unlock();
	return res;
}
bool util::IAssetLoader::HasPendingJobs() const
{
	m_assetIdToJobIdMutex.lock();
	auto res = !m_assetIdToJobId.empty();
	m_assetIdToJobIdMutex.unlock();
	return res;
}
bool util::IAssetLoader::HasCompletedJobs() const
{
	m_completeQueueMutex.lock();
	auto res = !m_completeQueue.empty();
	m_completeQueueMutex.unlock();
	return res;
}

void util::IAssetLoader::Poll(const std::function<void(const AssetLoadJob &, AssetLoadResult, std::optional<std::string>)> &onComplete, AssetLoaderWaitMode wait)
{
	if(wait == AssetLoaderWaitMode::All) {
		for(;;) {
			m_queueMutex.lock();
			auto hasJobsRemaining = !m_jobs.empty();
			m_queueMutex.unlock();
			if(!hasJobsRemaining && !m_hasCompletedJobs)
				return;
			Poll(onComplete, AssetLoaderWaitMode::Single);
		}
		return; // Unreachable
	}
	if(wait == AssetLoaderWaitMode::Single) {
		std::unique_lock<std::mutex> lock {m_completeQueueMutex};

		if(ShouldLog())
			m_logHandler("Poll: Waiting until completed jobs are available!", util::LogSeverity::Debug);

		m_completeCondition.wait(lock, [this]() { return m_hasCompletedJobs == true; });
	}
	if(!m_hasCompletedJobs)
		return;
	auto hasJobsRemaining = true;
	while(hasJobsRemaining) {
		// We must only pop one job at a time, in case there are recursive calls to Poll()
		m_completeQueueMutex.lock();
		// We have to re-check if there actually are any jobs
		m_hasCompletedJobs = !m_completeQueue.empty();
		if(!m_hasCompletedJobs) {
			m_completeQueueMutex.unlock();
			break;
		}

		if(ShouldLog())
			m_logHandler("Poll: Popping available job!", util::LogSeverity::Debug);

		auto job = std::move(m_completeQueue.front());
		m_completeQueue.pop();

		m_hasCompletedJobs = !m_completeQueue.empty();
		hasJobsRemaining = m_hasCompletedJobs;

		if(ShouldLog())
			m_logHandler("Poll: Jobs remaining: " + std::to_string(hasJobsRemaining), util::LogSeverity::Debug);

		m_completeQueueMutex.unlock();

		job.completionTime = std::chrono::high_resolution_clock::now();
		auto success = false;

		assert(job.processor != nullptr);

		m_assetIdToJobIdMutex.lock();
		auto it = m_assetIdToJobId.find(job.identifier);
		// If an asset is queued up multiple times, we only care about the latest job
		// and disregard previous ones.
		auto valid = (it != m_assetIdToJobId.end() && it->second.jobId == job.jobId);
		m_assetIdToJobIdMutex.unlock();

		auto errorMsg = job.processor->GetErrorMessage();
		if(valid) {
			if(job.state == AssetLoadJob::State::Succeeded && job.processor->Finalize()) {
				success = true;
				onComplete(job, AssetLoadResult::Succeeded, errorMsg);
			}
			else
				onComplete(job, AssetLoadResult::Failed, errorMsg);
		}
		else
			onComplete(job, AssetLoadResult::Cancelled, errorMsg);
		{
			m_assetIdToJobIdMutex.lock();

			if(ShouldLog())
				m_logHandler("Poll: Erasing job '" + job.identifier + "'...", util::LogSeverity::Debug);

			auto it = m_assetIdToJobId.find(job.identifier);
			if(it != m_assetIdToJobId.end()) {
				auto itPending = m_pendingAssetJobs.find(it->second.jobId);
				if(itPending != m_pendingAssetJobs.end())
					m_pendingAssetJobs.erase(itPending);
				m_assetIdToJobId.erase(it);
			}
			m_assetIdToJobIdMutex.unlock();
		}
	}
}
