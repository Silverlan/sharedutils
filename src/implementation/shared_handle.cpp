// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;


module pragma.util;

import :shared_handle;

util::PtrSharedHandleData::PtrSharedHandleData(const std::shared_ptr<void> &data) : m_data {data} {}
void util::PtrSharedHandleData::Invalidate() { m_data = nullptr; }
void *util::PtrSharedHandleData::GetData() { return m_data ? m_data.get() : nullptr; }
