/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "sharedutils/util_shared_handle.hpp"

util::PtrSharedHandleData::PtrSharedHandleData(const std::shared_ptr<void> &data)
	: m_data{data}
{}
void util::PtrSharedHandleData::Invalidate() {m_data = nullptr;}
void *util::PtrSharedHandleData::GetData() {return m_data ? m_data.get() : nullptr;}
