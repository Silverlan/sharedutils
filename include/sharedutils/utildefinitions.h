/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __UTILDEFINITIONS_H__
#define __UTILDEFINITIONS_H__

#ifdef SHUTIL_STATIC
	#define DLLSHUTIL
#elif SHUTIL_DLL
	#ifdef __linux__
		#define DLLSHUTIL __attribute__((visibility("default")))
	#else
		#define DLLSHUTIL __declspec(dllexport)
	#endif
#else
	#ifdef __linux__
		#define DLLSHUTIL
	#else
		#define DLLSHUTIL __declspec(dllimport)
	#endif
#endif
#ifndef UNUSED
	#define UNUSED(x) x
#endif

#endif
