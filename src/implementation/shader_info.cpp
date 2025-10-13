// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

module pragma.util;

import :shader_info;

using namespace util;

ShaderInfo::ShaderInfo(const std::string &identifier) : m_identifier(identifier), m_shader(nullptr) {}
const std::shared_ptr<void> &ShaderInfo::GetShader() const { return m_shader; }
void ShaderInfo::SetShader(const std::shared_ptr<void> &shader) { m_shader = shader; }

const std::string &ShaderInfo::GetIdentifier() const { return m_identifier; }
