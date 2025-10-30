// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include "sharedutils/utildefinitions.h"



export module pragma.util:enum_register;

export import :function_callback;

export namespace util {
	class DLLSHUTIL EnumRegister {
	  public:
		uint32_t RegisterEnum(const std::string &name);
		bool GetEnumValue(const std::string &name, uint32_t &val) const;
		uint32_t GetEnumValue(const std::string &name) const;
		const std::string *GetEnumName(uint32_t val) const;
		const std::vector<std::string> &GetEnums() const;
		CallbackHandle CallOnRegister(const std::function<void(std::reference_wrapper<const std::string>, uint32_t)> &f);
		static const uint32_t InvalidEnum;

		void Lock();
		void Unlock();
	  protected:
		std::vector<std::string> m_enums;
		std::vector<CallbackHandle> m_onRegisterCallbacks;
		mutable std::mutex m_enumMutex;
	};
};
