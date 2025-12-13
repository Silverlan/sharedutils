// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

module pragma.util;

import :parallel_job;
import magic_enum;

pragma::util::BaseParallelWorker::~BaseParallelWorker()
{
	// (These are already called by the shared_ptr deleter, calling them here would be too late!)
	// Cancel();
	// Wait();
}
std::string pragma::util::BaseParallelWorker::GetResultMessage() const
{
	m_msgMutex.lock();
	auto msg = m_resultMessage;
	m_msgMutex.unlock();
	return msg;
}
std::optional<int32_t> pragma::util::BaseParallelWorker::GetResultCode() const
{
	m_msgMutex.lock();
	auto code = m_resultCode;
	m_msgMutex.unlock();
	return code;
}
void pragma::util::BaseParallelWorker::SetProgressCallback(const std::function<void(float)> &progressCallback) { m_progressCallback = progressCallback; }
const std::function<void(float)> &pragma::util::BaseParallelWorker::GetProgressCallback() const { return m_progressCallback; }
bool pragma::util::BaseParallelWorker::IsPending() const { return IsComplete() == false; }
void pragma::util::BaseParallelWorker::SetResultMessage(const std::string &resultMsg, std::optional<int32_t> resultCode)
{
	m_msgMutex.lock();
	m_resultMessage = resultMsg;
	m_resultCode = resultCode;
	m_msgMutex.unlock();
}
void pragma::util::BaseParallelWorker::AddThread(const std::function<void()> &fThread) { m_pendingThreads.push_back(fThread); }
void pragma::util::BaseParallelWorker::SetStatus(JobStatus jobStatus, const std::optional<std::string> &resultMsg, std::optional<int32_t> resultCode)
{
	m_status = jobStatus;
	if(resultMsg.has_value())
		SetResultMessage(*resultMsg, resultCode);
	else {
		switch(m_status) {
		case JobStatus::Failed:
			SetResultMessage("Failed", resultCode);
			return;
		case JobStatus::Successful:
			SetResultMessage("Successful", resultCode);
			return;
		case JobStatus::Initial:
			SetResultMessage("Initial", resultCode);
			return;
		case JobStatus::Cancelled:
			SetResultMessage("Cancelled", resultCode);
			return;
		case JobStatus::Pending:
			SetResultMessage("Pending", resultCode);
			return;
		case JobStatus::Invalid:
			SetResultMessage("Invalid", resultCode);
			return;
		}
	}
}
pragma::util::JobStatus pragma::util::BaseParallelWorker::GetStatus() const { return m_status; }
bool pragma::util::BaseParallelWorker::IsThreadActive() const { return m_threadCompleteCount < m_threads.size(); }
bool pragma::util::BaseParallelWorker::IsComplete() const { return !IsThreadActive(); }
bool pragma::util::BaseParallelWorker::IsCancelled() const { return m_status == JobStatus::Cancelled || m_status == JobStatus::Failed; }
bool pragma::util::BaseParallelWorker::IsSuccessful() const { return m_status == JobStatus::Successful; }
bool pragma::util::BaseParallelWorker::IsValid() const { return true; }
float pragma::util::BaseParallelWorker::GetProgress() const { return m_progress; }
void pragma::util::BaseParallelWorker::UpdateProgress(float progress)
{
	m_progress = progress;
	if(m_progressCallback == nullptr)
		return;
	m_progressCallback(progress);
}
void pragma::util::BaseParallelWorker::Cancel() { Cancel("Job has been cancelled!"); }
void pragma::util::BaseParallelWorker::Cancel(const std::string &resultMsg, std::optional<int32_t> resultCode)
{
	if(IsValid() == false || IsComplete() || IsCancelled())
		return;
	SetStatus(JobStatus::Cancelled, resultMsg, resultCode);
	DoCancel(resultMsg, resultCode);
}
void pragma::util::BaseParallelWorker::DoCancel(const std::string &resultMsg, std::optional<int32_t> resultCode) {}
void pragma::util::BaseParallelWorker::Start()
{
	if(m_pendingThreads.empty()) {
		SetStatus(JobStatus::Successful);
		return;
	}
	m_bHasActiveThreads = true;
	SetStatus(JobStatus::Pending);
	m_threadMutex.lock();
	for(auto &fThread : m_pendingThreads) {
		auto threadId = m_threadId++;
		m_threads.emplace_back(threadId, std::thread {[this, fThread]() {
			fThread();
			if(++m_threadCompleteCount == m_threads.size() && m_status == JobStatus::Pending)
				SetStatus(JobStatus::Successful); // Status hasn't been set internally, so we'll assume it was successful.
		}});
		util::set_thread_name(m_threads.back().thread, "ParallelJob");
	}
	m_pendingThreads.clear();
	m_threadMutex.unlock();
}
void pragma::util::BaseParallelWorker::Wait()
{
	m_threadMutex.lock();
	for(auto &threadInfo : m_threads) {
		if(threadInfo.thread.joinable())
			threadInfo.thread.join();
	}
	m_threadMutex.unlock();
}

/////////

pragma::util::BaseParallelJob::BaseParallelJob(BaseParallelWorker &worker) : m_worker {worker.shared_from_this()} {}
void pragma::util::BaseParallelJob::Cancel()
{
	if(IsValid() == false || IsComplete() || IsCancelled())
		return;
	m_worker->Cancel();
}
void pragma::util::BaseParallelJob::Wait()
{
	if(IsValid() == false)
		return;
	m_worker->Wait();
	Poll();
}
void pragma::util::BaseParallelJob::Start()
{
	if(IsValid() == false)
		return;
	m_worker->Start();
}
bool pragma::util::BaseParallelJob::IsComplete() const
{
	if(IsValid() == false)
		return true;
	return m_worker->IsComplete();
}
bool pragma::util::BaseParallelJob::IsPending() const
{
	if(IsValid() == false)
		return false;
	return m_worker->IsPending();
}
bool pragma::util::BaseParallelJob::IsCancelled() const
{
	if(IsValid() == false)
		return false;
	return m_worker->IsCancelled();
}
bool pragma::util::BaseParallelJob::IsSuccessful() const
{
	if(IsValid() == false)
		return false;
	return m_worker->IsSuccessful();
}
bool pragma::util::BaseParallelJob::IsThreadActive() const
{
	if(IsValid() == false)
		return false;
	return m_worker->IsThreadActive();
}
float pragma::util::BaseParallelJob::GetProgress() const
{
	if(IsValid() == false)
		return 0.f;
	return m_worker->GetProgress();
}
pragma::util::JobStatus pragma::util::BaseParallelJob::GetStatus() const
{
	if(IsValid() == false)
		return JobStatus::Invalid;
	return m_worker->GetStatus();
}
std::string pragma::util::BaseParallelJob::GetResultMessage() const
{
	if(IsValid() == false)
		return "";
	return m_worker->GetResultMessage();
}
std::optional<int32_t> pragma::util::BaseParallelJob::GetResultCode() const
{
	if(IsValid() == false)
		return {};
	return m_worker->GetResultCode();
}
void pragma::util::BaseParallelJob::SetProgressCallback(const std::function<void(float)> &progressCallback)
{
	if(IsValid() == false)
		return;
	m_worker->SetProgressCallback(progressCallback);
}
const std::function<void(float)> &pragma::util::BaseParallelJob::GetProgressCallback() const
{
	if(IsValid() == false) {
		static std::function<void(float)> nptr = nullptr;
		return nptr;
	}
	return m_worker->GetProgressCallback();
}
bool pragma::util::BaseParallelJob::IsValid() const { return m_worker != nullptr; }
bool pragma::util::BaseParallelJob::Poll()
{
	if(IsComplete() == false)
		return false;
	CallCompletionHandler();
	return true;
}
void pragma::util::BaseParallelJob::CallCompletionHandler() { m_worker->CallCompletionHandler(); }

/////////

pragma::util::BaseParallelJob &pragma::util::ParallelJobWrapper::operator*() { return *m_job; }
const pragma::util::BaseParallelJob &pragma::util::ParallelJobWrapper::operator*() const { return const_cast<ParallelJobWrapper *>(this)->operator*(); }
pragma::util::BaseParallelJob *pragma::util::ParallelJobWrapper::operator->() { return m_job.get(); }
const pragma::util::BaseParallelJob *pragma::util::ParallelJobWrapper::operator->() const { return const_cast<ParallelJobWrapper *>(this)->operator->(); }

std::ostream &operator<<(std::ostream &out, const pragma::util::BaseParallelJob &o)
{
	out << "ParallelJob";
	if(!o.IsValid()) {
		out << "[invalid]";
		return out;
	}
	out << "[Status:";
	out << magic_enum::enum_name(o.GetStatus());
	out << "]";

	out << "[Thread:";
	if(o.IsThreadActive())
		out << "active";
	else
		out << "inactive";
	out << "]";

	out << "[Progress:" << o.GetProgress() << "]";
	out << "[Result:" << o.GetResultMessage() << "]";
	return out;
}
