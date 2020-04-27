/* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __UTIL_PARALLEL_JOB_HPP__
#define __UTIL_PARALLEL_JOB_HPP__

#include "utildefinitions.h"
#include <memory>
#include <optional>
#include <functional>
#include <thread>
#include <atomic>
#include <mutex>
#include <any>

namespace util
{
	enum class JobStatus : uint8_t
	{
		Failed = 0,
		Successful,
		Initial,
		Cancelled,
		Pending,
		
		Invalid = std::numeric_limits<uint8_t>::max()
	};
	class BaseParallelJob;
	class DLLSHUTIL BaseParallelWorker
		: public std::enable_shared_from_this<BaseParallelWorker>
	{
	public:
		BaseParallelWorker()=default;
		BaseParallelWorker(const BaseParallelWorker&)=default;
		virtual ~BaseParallelWorker();
		BaseParallelWorker &operator=(const BaseParallelWorker&)=default;
		void Cancel();
		virtual void Cancel(const std::string &resultMsg);
		virtual void Wait();
		virtual void Start();
		virtual bool IsValid() const;
		bool IsThreadActive() const;
		bool IsComplete() const;
		bool IsCancelled() const;
		bool IsSuccessful() const;
		float GetProgress() const;
		std::string GetResultMessage() const;
		JobStatus GetStatus() const;
		void SetProgressCallback(const std::function<void(float)> &progressCallback);
		const std::function<void(float)> &GetProgressCallback() const;

		bool IsPending() const;

		// Note: In general the worker should assign its status on its own, but in some cases
		// it may be useful to be able to change it from outside.
		void SetStatus(JobStatus jobStatus,const std::optional<std::string> &resultMsg={});
	protected:
		friend BaseParallelJob;
		void SetResultMessage(const std::string &resultMsg);
		void AddThread(const std::function<void()> &fThread);
		void UpdateProgress(float progress);
		virtual void CallCompletionHandler()=0;
	private:
		using ThreadId = uint32_t;
		struct ThreadInfo
		{
			ThreadInfo(ThreadId id,std::thread &&thread)
				: id{id},thread{std::move(thread)}
			{}
			ThreadId id;
			std::thread thread;
		};
		std::string m_resultMessage = "";
		std::function<void(float)> m_progressCallback = nullptr;
		std::atomic<JobStatus> m_status = JobStatus::Initial;
		std::atomic<bool> m_bHasActiveThreads = false;
		std::vector<std::function<void()>> m_pendingThreads = {};
		std::vector<ThreadInfo> m_threads = {};
		ThreadId m_threadId = 0u;
		mutable std::mutex m_threadMutex = {};
		mutable std::mutex m_msgMutex = {};
		std::atomic<float> m_progress = 0.f;
		std::atomic<uint32_t> m_threadCompleteCount = 0u;
	};
	template<typename T>
		class ParallelWorker
			: public BaseParallelWorker
	{
	public:
		using RESULT_TYPE = T;
		ParallelWorker()=default;
		ParallelWorker(const ParallelWorker&)=default;
		virtual ~ParallelWorker()=default;
		ParallelWorker &operator=(const ParallelWorker&)=default;

		virtual T GetResult()=0;
		void SetCompletionHandler(const std::function<void(ParallelWorker<T>&)> &fOnComplete) {m_fOnComplete = fOnComplete;}
		const std::function<void(ParallelWorker<T>&)> &GetCompletionHandler() const {return m_fOnComplete;}
	private:
		virtual void CallCompletionHandler() override
		{
			if(m_fOnComplete == nullptr)
				return;
			auto fOnComplete = m_fOnComplete;
			m_fOnComplete = nullptr;
			fOnComplete(*this);
		}
		std::function<void(ParallelWorker<T>&)> m_fOnComplete = nullptr;
	};

	class DLLSHUTIL BaseParallelJob
	{
	public:
		BaseParallelJob(BaseParallelWorker &worker);
		BaseParallelJob()=default;
		void Cancel();
		void Wait();
		void Start();
		bool IsComplete() const;
		bool IsPending() const;
		bool IsCancelled() const;
		bool IsSuccessful() const;
		bool IsThreadActive() const;
		float GetProgress() const;
		JobStatus GetStatus() const;
		std::string GetResultMessage() const;
		void SetProgressCallback(const std::function<void(float)> &progressCallback);
		const std::function<void(float)> &GetProgressCallback() const;

		bool IsValid() const;
		bool Poll();
	protected:
		void CallCompletionHandler();
		std::shared_ptr<BaseParallelWorker> m_worker = nullptr;
	};

	class DLLSHUTIL ParallelJobWrapper
	{
	public:
		template<class TJob>
			static ParallelJobWrapper Create(TJob &job)
		{
			ParallelJobWrapper wrapper {};
			wrapper.m_job = std::static_pointer_cast<BaseParallelJob>(std::make_shared<TJob>(job));
			return wrapper;
		}
		BaseParallelJob &operator*();
		const BaseParallelJob &operator*() const;
		BaseParallelJob *operator->();
		const BaseParallelJob *operator->() const;
	private:
		std::shared_ptr<BaseParallelJob> m_job = nullptr;
	};

	template<typename T>
		class ParallelJob final
			: public BaseParallelJob
	{
	public:
		ParallelJob(ParallelWorker<T> &worker);
		ParallelJob();
		T GetResult();
		ParallelWorker<T> &GetWorker();
		const ParallelWorker<T> &GetWorker() const;

		operator ParallelJobWrapper();
		void SetCompletionHandler(const std::function<void(ParallelWorker<T>&)> &fOnComplete) {static_cast<ParallelWorker<T>&>(*m_worker).SetCompletionHandler(fOnComplete);}
		const std::function<void(ParallelWorker<T>&)> &GetCompletionHandler() const {return static_cast<const ParallelWorker<T>&>(*m_worker).GetCompletionHandler();}
	};

	template<typename TJob,typename... TARGS>
		ParallelJob<typename TJob::RESULT_TYPE> create_parallel_job(TARGS&& ...args);
};

template<typename TJob,typename... TARGS>
	util::ParallelJob<typename TJob::RESULT_TYPE> util::create_parallel_job(TARGS&& ...args)
{
	auto job = std::shared_ptr<TJob>{new TJob{std::forward<TARGS>(args)...}};
	if(job->IsValid() == false)
		return {};
	return util::ParallelJob<TJob::RESULT_TYPE>{*job};
}

template<typename T>
	util::ParallelJob<T>::ParallelJob(ParallelWorker<T> &worker)
		: BaseParallelJob{worker}
{}

template<typename T>
	util::ParallelJob<T>::ParallelJob()
		: BaseParallelJob{}
	{}

template<typename T>
	T util::ParallelJob<T>::GetResult() {return static_cast<ParallelWorker<T>&>(*m_worker).GetResult();}

template<typename T>
	util::ParallelWorker<T> &util::ParallelJob<T>::GetWorker() {return static_cast<ParallelWorker<T>&>(*m_worker);}
template<typename T>
	const util::ParallelWorker<T> &util::ParallelJob<T>::GetWorker() const {return const_cast<util::ParallelJob<T>*>(this)->GetWorker();}

template<typename T>
	util::ParallelJob<T>::operator util::ParallelJobWrapper()
{
	return ParallelJobWrapper::Create(*this);
}

#endif
