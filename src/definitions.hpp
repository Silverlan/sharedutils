// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

#pragma once

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
