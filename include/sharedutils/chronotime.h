// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

#ifndef __CHRONOTIME_H__
#define __CHRONOTIME_H__

#include "utildefinitions.h"
#include "chronoutil.h"

#pragma warning(push)
#pragma warning(disable : 4251)
class DLLSHUTIL ChronoTime {
  protected:
	ChronoTimePoint m_lastUpdate;
	long double m_time;
  public:
	ChronoTime();
	long double operator()() const;
	long double GetTime() const;
	void Reset(ChronoTimePoint &t);
	void Reset(double t);
	void Reset(std::int64_t t);
	void Reset();
	void Update(double timeScale = 1.0);
	void UpdateByDelta(long double dt);
};
#pragma warning(pop)

#endif
