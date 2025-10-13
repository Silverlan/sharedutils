// SPDX-FileCopyrightText: (c) 2020 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include "sharedutils/utildefinitions.h"
#include <cinttypes>

export module pragma.util:pragma;

export namespace pragma {
	constexpr inline double units_to_metres(double units) { return units * 0.025; } // 1.75 (Avg. Height in meters) /72 (Player Height in units) ~= 0.025
	constexpr inline double metres_to_units(double meters) { return meters / 0.025; }

	enum class LightType : uint8_t { Undefined = 0, Spot, Point, Directional };

	// Converts a candela light intensity value to watts.
	// Note that the conversion is arbitrary and is not based on
	// a conventional formula, as conventional conversion attempts
	// have not resulted in good looking results.
	DLLSHUTIL float light_intensity_to_watts(float candelaOrLux, LightType lightType);
};
