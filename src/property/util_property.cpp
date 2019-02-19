/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "sharedutils/property/util_property.hpp"

using namespace util;

void util::BaseProperty::AddCallback(const CallbackHandle &hCallback)
{
	m_callbacks.push_back(hCallback);
}

void util::BoolProperty::ApplyValue(const bool &newValue)
{
	m_value = newValue != 0; // Make absolutely sure the value is either 1 or 0
}
