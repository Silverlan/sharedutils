// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

#include "sharedutils/util_handle.hpp"

util::PtrHandleData::PtrHandleData(void *data) : m_data {data} {}
void util::PtrHandleData::Invalidate() { m_data = nullptr; }
void *util::PtrHandleData::GetData() { return m_data; }
