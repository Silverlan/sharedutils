/* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*
* Copyright (c) 2021 Silverlan
*/

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
