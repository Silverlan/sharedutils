// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include <cinttypes>
#include <chrono>

module pragma.util;

import :chrono;

ChronoTime::ChronoTime() { Reset(); }

void ChronoTime::Reset(ChronoTimePoint &t)
{
	m_lastUpdate = t;
	m_time = 0;
}

void ChronoTime::Reset(double t)
{
	m_lastUpdate = std::chrono::steady_clock::now();
	//m_time = static_cast<long double>(static_cast<long double>(t) *1'000'000'000.0); // C++14
	m_time = static_cast<long double>(static_cast<long double>(t) * 1000000000.0);
}

void ChronoTime::Reset(std::int64_t t)
{
	m_lastUpdate = std::chrono::steady_clock::now();
	//m_time = static_cast<long double>(t) *1'000'000; // C++14
	m_time = static_cast<long double>(t) * 1000000;
}

void ChronoTime::Reset()
{
	ChronoTimePoint p = std::chrono::steady_clock::now();
	Reset(p);
}

long double ChronoTime::operator()() const { return m_time / 1'000'000'000.0; }
long double ChronoTime::GetTime() const { return m_time / 1'000'000.0; }

void ChronoTime::Update(double timeScale)
{
	ChronoTimePoint now = std::chrono::steady_clock::now();
	m_time += static_cast<long double>(std::chrono::duration_cast<std::chrono::nanoseconds>(now - m_lastUpdate).count()) * timeScale;
	m_lastUpdate = now;
}

void ChronoTime::UpdateByDelta(long double dt)
{
	ChronoTimePoint now = std::chrono::steady_clock::now();
	m_time += dt * 1'000'000'000.0;
	m_lastUpdate = now;
}
