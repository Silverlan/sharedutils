/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "sharedutils/chronotime.h"

ChronoTime::ChronoTime()
{
	Reset();
}

void ChronoTime::Reset(ChronoTimePoint &t)
{
	m_lastUpdate = t;
	m_time = 0;
}

void ChronoTime::Reset(double t)
{
	m_lastUpdate = std::chrono::high_resolution_clock::now();
	//m_time = static_cast<long double>(static_cast<long double>(t) *1'000'000'000.0); // C++14
	m_time = static_cast<long double>(static_cast<long double>(t) *1000000000.0);
}

void ChronoTime::Reset(long long t)
{
	m_lastUpdate = std::chrono::high_resolution_clock::now();
	//m_time = static_cast<long double>(t) *1'000'000; // C++14
	m_time = static_cast<long double>(t) *1000000;
}

void ChronoTime::Reset()
{
	ChronoTimePoint p = std::chrono::high_resolution_clock::now();
	Reset(p);
}

long double ChronoTime::operator()() const {return m_time /1'000'000'000.0;}
long double ChronoTime::GetTime() const {return m_time /1'000'000.0;}

void ChronoTime::Update(double timeScale)
{
	ChronoTimePoint now = std::chrono::high_resolution_clock::now();
	m_time += static_cast<long double>(std::chrono::duration_cast<std::chrono::nanoseconds>(now -m_lastUpdate).count()) *timeScale;
	m_lastUpdate = now;
}

void ChronoTime::UpdateByDelta(long double dt)
{
	ChronoTimePoint now = std::chrono::high_resolution_clock::now();
	m_time += dt *1'000'000'000.0;
	m_lastUpdate = now;
}
