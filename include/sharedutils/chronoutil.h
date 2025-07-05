// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

#ifndef __CHRONOUTIL_H__
#define __CHRONOUTIL_H__

#include <chrono>
using ChronoTimePoint = std::chrono::time_point<std::chrono::steady_clock>;
using ChronoDuration = std::chrono::duration<std::chrono::steady_clock::rep, std::chrono::steady_clock::period>;

#endif
