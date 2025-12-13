// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

module pragma.util;

import :handle;

pragma::util::PtrHandleData::PtrHandleData(void *data) : m_data {data} {}
void pragma::util::PtrHandleData::Invalidate() { m_data = nullptr; }
void *pragma::util::PtrHandleData::GetData() { return m_data; }
