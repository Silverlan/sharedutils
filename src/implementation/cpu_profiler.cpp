// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include <chrono>

module pragma.util;

import :cpu_profiler;

using namespace util;

CPUProfiler::Stage::Stage() : duration(0) {}

CPUProfiler::CPUProfiler() {}

const std::unordered_map<uint32_t, std::shared_ptr<CPUProfiler::Stage>> &CPUProfiler::GetProfilingStages() const { return m_stages; }

const CPUProfiler::Stage &CPUProfiler::StartStageProfiling(uint32_t stage)
{
	auto it = m_stages.find(stage);
	if(it == m_stages.end())
		it = m_stages.insert(decltype(m_stages)::value_type(stage, std::make_unique<Stage>())).first;
	it->second->start = std::chrono::high_resolution_clock::now();
	return *it->second;
}
std::chrono::nanoseconds CPUProfiler::EndStageProfiling(uint32_t stage, bool addDuration)
{
	auto t = std::chrono::high_resolution_clock::now();
	auto it = m_stages.find(stage);
	if(it == m_stages.end())
		return std::chrono::nanoseconds(0);
	auto tDelta = t - it->second->start;
	if(addDuration == false)
		it->second->duration = tDelta;
	else
		it->second->duration += tDelta;
	return it->second->duration;
}
std::chrono::nanoseconds CPUProfiler::GetStageProfilingTime(uint32_t stage) const { return GetStageProfilingTime<decltype(GetStageProfilingTime(stage))>(stage); }
std::chrono::milliseconds CPUProfiler::GetStageProfilingTimeMs(uint32_t stage) const { return GetStageProfilingTime<decltype(GetStageProfilingTimeMs(stage))>(stage); }
std::chrono::seconds CPUProfiler::GetStageProfilingTimeS(uint32_t stage) const { return GetStageProfilingTime<decltype(GetStageProfilingTimeS(stage))>(stage); }

////////////////////

CPUTimer::CPUTimer() : m_start(std::chrono::high_resolution_clock::now()), m_end(std::chrono::high_resolution_clock::now()) {}
void CPUTimer::Begin() { m_start = std::chrono::high_resolution_clock::now(); }
void CPUTimer::End() { m_end = std::chrono::high_resolution_clock::now(); }
std::chrono::high_resolution_clock::time_point CPUTimer::GetStartTime() const { return m_start; }
std::chrono::high_resolution_clock::time_point CPUTimer::GetEndTime() const { return m_end; }
std::chrono::high_resolution_clock::duration CPUTimer::GetDeltaTime() const { return m_end - m_start; }
double CPUTimer::GetDeltaTimeMs() const { return GetDeltaTime<std::chrono::nanoseconds>() / 1'000'000.0; }
