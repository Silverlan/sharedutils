/* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "sharedutils/util_shaderinfo.hpp"

using namespace util;

ShaderInfo::ShaderInfo(const std::string &identifier)
	: m_identifier(identifier),m_shader(nullptr)
{}
const std::shared_ptr<void> &ShaderInfo::GetShader() const {return m_shader;}
void ShaderInfo::SetShader(const std::shared_ptr<void> &shader) {m_shader = shader;}

const std::string &ShaderInfo::GetIdentifier() const {return m_identifier;}