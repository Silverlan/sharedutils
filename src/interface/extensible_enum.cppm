// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include "definitions.hpp"

export module pragma.util:extensible_enum;

export import std.compat;

export namespace pragma::util {
	template<typename T>
	class TExtensibleEnum {
	  public:
		bool operator==(const TExtensibleEnum<T> &other) const { return m_value == other.m_value; }
		bool operator!=(const TExtensibleEnum<T> &other) const { return m_value != other.m_value; }
		bool operator<(const TExtensibleEnum<T> &other) const { return m_value < other.m_value; }
		bool operator>(const TExtensibleEnum<T> &other) const { return m_value > other.m_value; }
		bool operator<=(const TExtensibleEnum<T> &other) const { return m_value <= other.m_value; }
		bool operator>=(const TExtensibleEnum<T> &other) const { return m_value >= other.m_value; }

		operator bool() const { return !!m_value; }
		operator T() const { return m_value; }
		bool operator!() const { return !m_value; }

		T operator+(const TExtensibleEnum<T> &other) const { return m_value + other.m_value; }
		T operator-(const TExtensibleEnum<T> &other) const { return m_value - other.m_value; }
		T operator/(const TExtensibleEnum<T> &other) const { return m_value / other.m_value; }
		T operator*(const TExtensibleEnum<T> &other) const { return m_value * other.m_value; }
		T operator|(const TExtensibleEnum<T> &other) const { return m_value | other.m_value; }
		T operator&(const TExtensibleEnum<T> &other) const { return m_value & other.m_value; }
		T operator<<(const TExtensibleEnum<T> &other) const { return m_value << other.m_value; }
		T operator>>(const TExtensibleEnum<T> &other) const { return m_value >> other.m_value; }
		T operator~() const { return ~m_value; }
		const T &operator*() const { return m_value; }
		T &operator*() { return m_value; }

		std::size_t Hash() const { return std::hash<T>()(m_value); }
	  protected:
		TExtensibleEnum(const T &val) : m_value(val) {}
		// This constructor is dangerous and should only be used if ALL enums are defined in the same compilation unit
		TExtensibleEnum() : m_value(s_max++) {}
		static inline uint32_t s_max = 0u;
		T m_value;
	};

	class DLLSHUTIL ExtensibleEnum : public TExtensibleEnum<uint32_t> {
	  public:
		using TExtensibleEnum<uint32_t>::TExtensibleEnum;
	};
};
//DEFINE_STD_HASH_SPECIALIZATION(pragma::util::ExtensibleEnum);
