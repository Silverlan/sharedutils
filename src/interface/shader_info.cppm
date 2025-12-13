// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include "definitions.hpp"

export module pragma.util:shader_info;

export import std.compat;

export namespace pragma::util {
	class DLLSHUTIL ShaderInfo {
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
