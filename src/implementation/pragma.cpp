// SPDX-FileCopyrightText: (c) 2020 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;



module pragma.util;

import :pragma;

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
