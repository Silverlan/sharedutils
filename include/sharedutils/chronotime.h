/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __CHRONOTIME_H__
#define __CHRONOTIME_H__

#include "utildefinitions.h"
#include "chronoutil.h"

#pragma warning(push)
#pragma warning(disable : 4251)
class DLLSHUTIL ChronoTime
{
protected:
	ChronoTimePoint m_lastUpdate;
	long double m_time;
public:
	ChronoTime();
	long double operator()();
	long double GetTime() const;
	void Update();
	void Reset(ChronoTimePoint &t);
	void Reset(double t);
	void Reset(long long t);
	void Reset();
	void Update(double timeScale);
};
#pragma warning(pop)

#endif
