/* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*
* Copyright (c) 2020 Florian Weischer
*/

#ifndef __UTIL_PRAGMA_HPP__
#define __UTIL_PRAGMA_HPP__

#include "utildefinitions.h"
#include <cinttypes>

namespace util::pragma
{
	constexpr inline double units_to_metres(double units) {return units *0.025;} // 1.75 (Avg. Height in meters) /72 (Player Height in units) ~= 0.025
	constexpr inline double metres_to_units(double meters) {return meters /0.025;}

	enum class LightType : uint8_t
	{
		Undefined = 0,
		Spot,
		Point,
		Directional
	};

	// Converts a candela light intensity value to watts.
	// Note that the conversion is arbitrary and is not based on
	// a conventional formula, as conventional conversion attempts
	// have not resulted in good looking results.
	DLLSHUTIL float light_intensity_to_watts(float candelaOrLux,LightType lightType);
};

#endif
