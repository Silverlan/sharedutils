// SPDX-FileCopyrightText: (c) 2020 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

#ifndef __ALPHA_MODE_HPP__
#define __ALPHA_MODE_HPP__

#include <cinttypes>

enum class AlphaMode : uint32_t {
	Opaque = 0,
	Mask,
	Blend,

	Count
};

#endif
