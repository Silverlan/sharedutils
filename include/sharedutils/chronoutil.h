/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __CHRONOUTIL_H__
#define __CHRONOUTIL_H__

#include <chrono>
using ChronoTimePoint = std::chrono::time_point<std::chrono::high_resolution_clock>;
using ChronoDuration = std::chrono::duration<std::chrono::high_resolution_clock::rep,std::chrono::high_resolution_clock::period>;

#endif