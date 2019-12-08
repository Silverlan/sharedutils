/* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __UTIL_SHADERINFO_HPP__
#define __UTIL_SHADERINFO_HPP__

#include "utildefinitions.h"
#include <string>
#include <memory>

namespace util
{
	class DLLSHUTIL ShaderInfo
	{
	public:
		ShaderInfo(const std::string &identifier);
		const std::shared_ptr<void> &GetShader() const;
		void SetShader(const std::shared_ptr<void> &shader);

		const std::string &GetIdentifier() const;
	private:
		std::shared_ptr<void> m_shader = nullptr;
		std::string m_identifier;
	};
};

#endif