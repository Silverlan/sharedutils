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