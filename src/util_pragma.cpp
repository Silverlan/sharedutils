/* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <memory>
#include "sharedutils/util_pragma.hpp"
#include <mathutil/umath_lighting.hpp>

float pragma::light_intensity_to_watts(float candelaOrLux, LightType lightType)
{
	if(lightType == LightType::Directional) {
		static auto intensity = 7.5f;
		auto watt = candelaOrLux / intensity; // Lux
		return watt;
	}
	// Candela
	auto watt = ulighting::lumens_to_watts(candelaOrLux, ulighting::get_luminous_efficacy(ulighting::LightSourceType::LEDLamp));
	static auto mulFactor = 30.f;
	watt *= mulFactor; // Arbitrary, but results in a closer match
	return watt;
}
