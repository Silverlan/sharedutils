/* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "sharedutils/util_parallel_job.hpp"
#include <algorithm>

util::BaseParallelWorker::~BaseParallelWorker()
{
	Cancel();
	Wait();
}
std::string util::BaseParallelWorker::GetResultMessage() const
{
	m_msgMutex.lock();
	auto msg = m_resultMessage;
	m_msgMutex.unlock();
	return msg;
}
void util::BaseParallelWorker::SetProgressCallback(const std::function<void(float)> &progressCallback) {m_progressCallback = progressCallback;}
const std::function<void(float)> &util::BaseParallelWorker::GetProgressCallback() const {return m_progressCallback;}
bool util::BaseParallelWorker::IsPending() const {return IsComplete() == false;}
void util::BaseParallelWorker::SetResultMessage(const std::string &resultMsg)
{
	m_msgMutex.lock();
		m_resultMessage = resultMsg;
	m_msgMutex.unlock();
}
void util::BaseParallelWorker::AddThread(const std::function<void()> &fThread) {m_pendingThreads.push_back(fThread);}
void util::BaseParallelWorker::SetStatus(JobStatus jobStatus,const std::optional<std::string> &resultMsg)
{
	m_status = jobStatus;
	if(resultMsg.has_value())
		SetResultMessage(*resultMsg);
	else
	{
		switch(m_status)
		{
		case JobStatus::Failed:
			SetResultMessage("Failed");
			return;
		case JobStatus::Successful:
			SetResultMessage("Successful");
			return;
		case JobStatus::Initial:
			SetResultMessage("Initial");
			return;
		case JobStatus::Cancelled:
			SetResultMessage("Cancelled");
			return;
		case JobStatus::Pending:
			SetResultMessage("Pending");
			return;
		case JobStatus::Invalid:
			SetResultMessage("Invalid");
			return;
		}
	}
}
util::JobStatus util::BaseParallelWorker::GetStatus() const {return m_status;}
bool util::BaseParallelWorker::IsThreadActive() const {return m_threadCompleteCount < m_threads.size();}
bool util::BaseParallelWorker::IsComplete() const {return !IsThreadActive();}
bool util::BaseParallelWorker::IsCancelled() const {return m_status == JobStatus::Cancelled || m_status == JobStatus::Failed;}
bool util::BaseParallelWorker::IsSuccessful() const {return m_status == JobStatus::Successful;}
bool util::BaseParallelWorker::IsValid() const {return true;}
float util::BaseParallelWorker::GetProgress() const {return m_progress;}
void util::BaseParallelWorker::UpdateProgress(float progress)
{
	m_progress = progress;
	if(m_progressCallback == nullptr)
		return;
	m_progressCallback(progress);
}
void util::BaseParallelWorker::Cancel() {Cancel("Job has been cancelled!");}
void util::BaseParallelWorker::Cancel(const std::string &resultMsg) {SetStatus(JobStatus::Cancelled,resultMsg);}
void util::BaseParallelWorker::Start()
{
	if(m_pendingThreads.empty())
	{
		SetStatus(JobStatus::Successful);
		return;
	}
	m_bHasActiveThreads = true;
	SetStatus(JobStatus::Pending);
	m_threadMutex.lock();
	for(auto &fThread : m_pendingThreads)
	{
		auto threadId = m_threadId++;
		m_threads.emplace_back(threadId,std::thread{[this,fThread]() {
			fThread();
			if(++m_threadCompleteCount == m_threads.size() && m_status == JobStatus::Pending)
				SetStatus(JobStatus::Successful); // Status hasn't been set internally, so we'll assume it was successful.
		}});
	}
	m_pendingThreads.clear();
	m_threadMutex.unlock();
}
void util::BaseParallelWorker::Wait()
{
	m_threadMutex.lock();
	for(auto &threadInfo : m_threads)
	{
		if(threadInfo.thread.joinable())
			threadInfo.thread.join();
	}
	m_threadMutex.unlock();
}

/////////

util::BaseParallelJob::BaseParallelJob(BaseParallelWorker &worker)
	: m_worker{worker.shared_from_this()}
{}
void util::BaseParallelJob::Cancel()
{
	if(IsValid() == false || IsComplete() || IsCancelled())
		return;
	m_worker->Cancel();
}
void util::BaseParallelJob::Wait()
{
	if(IsValid() == false)
		return;
	m_worker->Wait();
	Poll();
}
void util::BaseParallelJob::Start()
{
	if(IsValid() == false)
		return;
	m_worker->Start();
}
bool util::BaseParallelJob::IsComplete() const {if(IsValid() == false) return true; return m_worker->IsComplete();}
bool util::BaseParallelJob::IsPending() const {if(IsValid() == false) return false; return m_worker->IsPending();}
bool util::BaseParallelJob::IsCancelled() const {if(IsValid() == false) return false; return m_worker->IsCancelled();}
bool util::BaseParallelJob::IsSuccessful() const {if(IsValid() == false) return false; return m_worker->IsSuccessful();}
bool util::BaseParallelJob::IsThreadActive() const {if(IsValid() == false) return false; return m_worker->IsThreadActive();}
float util::BaseParallelJob::GetProgress() const {if(IsValid() == false) return 0.f; return m_worker->GetProgress();}
util::JobStatus util::BaseParallelJob::GetStatus() const {if(IsValid() == false) return JobStatus::Invalid; return m_worker->GetStatus();}
std::string util::BaseParallelJob::GetResultMessage() const
{
	if(IsValid() == false)
		return "";
	return m_worker->GetResultMessage();
}
void util::BaseParallelJob::SetProgressCallback(const std::function<void(float)> &progressCallback)
{
	if(IsValid() == false)
		return;
	m_worker->SetProgressCallback(progressCallback);
}
const std::function<void(float)> &util::BaseParallelJob::GetProgressCallback() const
{
	if(IsValid() == false)
	{
		static std::function<void(float)> nptr = nullptr;
		return nptr;
	}
	return m_worker->GetProgressCallback();
}
bool util::BaseParallelJob::IsValid() const {return m_worker != nullptr;}
bool util::BaseParallelJob::Poll()
{
	if(IsComplete() == false)
		return false;
	CallCompletionHandler();
	return true;
}
void util::BaseParallelJob::CallCompletionHandler() {m_worker->CallCompletionHandler();}

/////////

util::BaseParallelJob &util::ParallelJobWrapper::operator*() {return *m_job;}
const util::BaseParallelJob &util::ParallelJobWrapper::operator*() const {return const_cast<ParallelJobWrapper*>(this)->operator*();}
util::BaseParallelJob *util::ParallelJobWrapper::operator->() {return m_job.get();}
const util::BaseParallelJob *util::ParallelJobWrapper::operator->() const {return const_cast<ParallelJobWrapper*>(this)->operator->();}
