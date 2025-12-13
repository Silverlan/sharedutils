// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include "definitions.hpp"

export module pragma.util:cpu_profiler;

export import std.compat;

export {
	namespace pragma::util {
		class DLLSHUTIL CPUTimer {
		  private:
			std::chrono::high_resolution_clock::time_point m_start;
			std::chrono::high_resolution_clock::time_point m_end;
		  public:
			CPUTimer();
			void Begin();
			void End();
			std::chrono::high_resolution_clock::time_point GetStartTime() const;
			std::chrono::high_resolution_clock::time_point GetEndTime() const;
			std::chrono::high_resolution_clock::duration GetDeltaTime() const;
			double GetDeltaTimeMs() const;
			template<class T>
			long long GetDeltaTime() const;
		};

		class DLLSHUTIL CPUProfiler {
		  public:
			struct DLLSHUTIL Stage {
				Stage();
				std::chrono::high_resolution_clock::time_point start;
				std::chrono::nanoseconds duration;
			};
		  private:
			std::unordered_map<uint32_t, std::shared_ptr<Stage>> m_stages;
		  public:
			CPUProfiler();
			const Stage &StartStageProfiling(uint32_t stage);
			std::chrono::nanoseconds EndStageProfiling(uint32_t stage, bool addDuration = false);
			template<class T>
			T GetStageProfilingTime(uint32_t stage) const;
			std::chrono::nanoseconds GetStageProfilingTime(uint32_t stage) const;
			std::chrono::milliseconds GetStageProfilingTimeMs(uint32_t stage) const;
			std::chrono::seconds GetStageProfilingTimeS(uint32_t stage) const;
			const std::unordered_map<uint32_t, std::shared_ptr<Stage>> &GetProfilingStages() const;
		};

		template<class T>
		long long CPUTimer::GetDeltaTime() const
		{
			return std::chrono::duration_cast<T>(GetDeltaTime()).count();
		}

		template<class T>
		T CPUProfiler::GetStageProfilingTime(uint32_t stage) const
		{
			auto it = m_stages.find(stage);
			if(it == m_stages.end())
				return T(0);
			return std::chrono::duration_cast<T>(it->second->duration);
		}
	};
}
