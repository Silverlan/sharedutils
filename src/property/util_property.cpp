// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

#include "sharedutils/property/util_property.hpp"

using namespace util;

void util::BaseProperty::AddCallback(const CallbackHandle &hCallback) { m_callbacks.push_back(hCallback); }

void util::BoolProperty::ApplyValue(const bool &newValue)
{
	m_value = newValue != 0; // Make absolutely sure the value is either 1 or 0
}
