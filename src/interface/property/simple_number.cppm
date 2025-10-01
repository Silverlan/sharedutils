// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include "sharedutils/utildefinitions.h"
#include "sharedutils/def_handle.h"
#include <cinttypes>

export module pragma.util:property_simple_number;

export import :property;

export {
	#pragma warning(push)
	#pragma warning(disable : 4544)
	namespace util {
		template<class TProperty, class T>
		class SimpleNumberProperty : public SimpleProperty<TProperty, T> {
		public:
			SimpleNumberProperty() : SimpleProperty<TProperty, T>{} {}
			SimpleNumberProperty(const T &init) : SimpleProperty<TProperty, T>{init} {}
			SimpleNumberProperty(const SimpleNumberProperty &prop) : SimpleProperty<TProperty, T>{prop} {}
			SimpleNumberProperty &operator=(const T &val);
			SimpleNumberProperty &operator=(const SimpleNumberProperty &other);
			T &operator*() {return SimpleProperty<TProperty, T>::operator*();}
			const T &operator*() const {return SimpleProperty<TProperty, T>::operator*();}
			const T &GetValue() const {return SimpleProperty<TProperty, T>::GetValue();}
			T &GetValue() {return SimpleProperty<TProperty, T>::GetValue();}

			// Assignment
			SimpleNumberProperty &operator+=(const T &val);
			SimpleNumberProperty &operator+=(const SimpleNumberProperty<TProperty, T> &prop);
			SimpleNumberProperty &operator-=(const T &val);
			SimpleNumberProperty &operator-=(const SimpleNumberProperty<TProperty, T> &prop);
			SimpleNumberProperty &operator*=(const T &val);
			SimpleNumberProperty &operator*=(const SimpleNumberProperty<TProperty, T> &prop);
			SimpleNumberProperty &operator/=(const T &val);
			SimpleNumberProperty &operator/=(const SimpleNumberProperty<TProperty, T> &prop);
			template<typename T1 = T, typename = typename std::enable_if_t<std::is_integral<T1>::value>>
			SimpleNumberProperty &operator%=(const T &val);
			template<typename T1 = T, typename = typename std::enable_if_t<std::is_integral<T1>::value>>
			SimpleNumberProperty &operator%=(const SimpleNumberProperty<TProperty, T> &prop);
			template<typename T1 = T, typename = typename std::enable_if_t<std::is_integral<T1>::value>>
			SimpleNumberProperty &operator&=(const T &val);
			template<typename T1 = T, typename = typename std::enable_if_t<std::is_integral<T1>::value>>
			SimpleNumberProperty &operator&=(const SimpleNumberProperty<TProperty, T> &prop);
			template<typename T1 = T, typename = typename std::enable_if_t<std::is_integral<T1>::value>>
			SimpleNumberProperty &operator|=(const T &val);
			template<typename T1 = T, typename = typename std::enable_if_t<std::is_integral<T1>::value>>
			SimpleNumberProperty &operator|=(const SimpleNumberProperty<TProperty, T> &prop);
			template<typename T1 = T, typename = typename std::enable_if_t<std::is_integral<T1>::value>>
			SimpleNumberProperty &operator^=(const T &val);
			template<typename T1 = T, typename = typename std::enable_if_t<std::is_integral<T1>::value>>
			SimpleNumberProperty &operator^=(const SimpleNumberProperty<TProperty, T> &prop);
			template<typename T1 = T, typename = typename std::enable_if_t<std::is_integral<T1>::value>>
			SimpleNumberProperty &operator<<=(const T &val);
			template<typename T1 = T, typename = typename std::enable_if_t<std::is_integral<T1>::value>>
			SimpleNumberProperty &operator<<=(const SimpleNumberProperty<TProperty, T> &prop);
			template<typename T1 = T, typename = typename std::enable_if_t<std::is_integral<T1>::value>>
			SimpleNumberProperty &operator>>=(const T &val);
			template<typename T1 = T, typename = typename std::enable_if_t<std::is_integral<T1>::value>>
			SimpleNumberProperty &operator>>=(const SimpleNumberProperty<TProperty, T> &prop);

			// Increment / Decrement
			SimpleNumberProperty &operator++();
			SimpleNumberProperty &operator--();
			T operator++(int);
			T operator--(int);

			// Arithmetic
			T operator+() const;
			template<typename T1 = T, typename = typename std::enable_if_t<std::is_unsigned<T1>::value>>
			T operator-() const;
			T operator+(const T &val) const;
			T operator+(const SimpleNumberProperty<TProperty, T> &prop) const;
			T operator-(const T &val) const;
			T operator-(const SimpleNumberProperty<TProperty, T> &prop) const;
			T operator*(const T &val) const;
			T operator*(const SimpleNumberProperty<TProperty, T> &prop) const;
			T operator/(const T &val) const;
			T operator/(const SimpleNumberProperty<TProperty, T> &prop) const;
			template<typename T1 = T, typename = typename std::enable_if_t<std::is_integral<T1>::value>>
			T operator%(const T &val) const;
			template<typename T1 = T, typename = typename std::enable_if_t<std::is_integral<T1>::value>>
			T operator%(const SimpleNumberProperty<TProperty, T> &prop) const;
			template<typename T1 = T, typename = typename std::enable_if_t<std::is_integral<T1>::value>>
			T operator~() const;
			template<typename T1 = T, typename = typename std::enable_if_t<std::is_integral<T1>::value>>
			T operator&(const T &val) const;
			template<typename T1 = T, typename = typename std::enable_if_t<std::is_integral<T1>::value>>
			T operator&(const SimpleNumberProperty<TProperty, T> &prop) const;
			template<typename T1 = T, typename = typename std::enable_if_t<std::is_integral<T1>::value>>
			T operator|(const T &val) const;
			template<typename T1 = T, typename = typename std::enable_if_t<std::is_integral<T1>::value>>
			T operator|(const SimpleNumberProperty<TProperty, T> &prop) const;
			template<typename T1 = T, typename = typename std::enable_if_t<std::is_integral<T1>::value>>
			T operator^(const T &val) const;
			template<typename T1 = T, typename = typename std::enable_if_t<std::is_integral<T1>::value>>
			T operator^(const SimpleNumberProperty<TProperty, T> &prop) const;
			template<typename T1 = T, typename = typename std::enable_if_t<std::is_integral<T1>::value>>
			T operator<<(const T &val) const;
			template<typename T1 = T, typename = typename std::enable_if_t<std::is_integral<T1>::value>>
			T operator<<(const SimpleNumberProperty<TProperty, T> &prop) const;
			template<typename T1 = T, typename = typename std::enable_if_t<std::is_integral<T1>::value>>
			T operator>>(const T &val) const;
			template<typename T1 = T, typename = typename std::enable_if_t<std::is_integral<T1>::value>>
			T operator>>(const SimpleNumberProperty<TProperty, T> &prop) const;

			// Logical
			bool operator!() const;
			bool operator&&(const T &val) const;
			bool operator&&(const SimpleNumberProperty<TProperty, T> &prop) const;
			bool operator||(const T &val) const;
			bool operator||(const SimpleNumberProperty<TProperty, T> &prop) const;

			// Comparison
			bool operator<(const T &val) const;
			bool operator<(const SimpleProperty<TProperty, T> &prop) const;
			bool operator>(const T &val) const;
			bool operator>(const SimpleProperty<TProperty, T> &prop) const;
			bool operator<=(const T &val) const;
			bool operator<=(const SimpleProperty<TProperty, T> &prop) const;
			bool operator>=(const T &val) const;
			bool operator>=(const SimpleProperty<TProperty, T> &prop) const;

			operator const T &() const;
			operator T &();
		};
	};

	template<class TProperty, class T>
	util::SimpleNumberProperty<TProperty, T> &util::SimpleNumberProperty<TProperty, T>::operator=(const T &val) {return static_cast<util::SimpleNumberProperty<TProperty, T>&>(util::SimpleProperty<TProperty, T>::operator=(val));}

	template<class TProperty, class T>
	util::SimpleNumberProperty<TProperty, T> &util::SimpleNumberProperty<TProperty, T>::operator=(const util::SimpleNumberProperty<TProperty, T> &other) {return static_cast<util::SimpleNumberProperty<TProperty, T>&>(util::SimpleProperty<TProperty, T>::operator=(other));}

	template<class TProperty, class T>
	util::SimpleNumberProperty<TProperty, T> &util::SimpleNumberProperty<TProperty, T>::operator+=(const T &val)
	{
		*this = GetValue() + val;
		return *this;
	}

	template<class TProperty, class T>
	util::SimpleNumberProperty<TProperty, T> &util::SimpleNumberProperty<TProperty, T>::operator+=(const SimpleNumberProperty<TProperty, T> &prop)
	{
		return *this += prop.GetValue();
	}

	template<class TProperty, class T>
	util::SimpleNumberProperty<TProperty, T> &util::SimpleNumberProperty<TProperty, T>::operator-=(const T &val)
	{
		*this = GetValue() - val;
		return *this;
	}

	template<class TProperty, class T>
	util::SimpleNumberProperty<TProperty, T> &util::SimpleNumberProperty<TProperty, T>::operator-=(const SimpleNumberProperty<TProperty, T> &prop)
	{
		return *this -= prop.GetValue();
	}

	template<class TProperty, class T>
	util::SimpleNumberProperty<TProperty, T> &util::SimpleNumberProperty<TProperty, T>::operator*=(const T &val)
	{
		*this = GetValue() * val;
		return *this;
	}

	template<class TProperty, class T>
	util::SimpleNumberProperty<TProperty, T> &util::SimpleNumberProperty<TProperty, T>::operator*=(const SimpleNumberProperty<TProperty, T> &prop)
	{
		return *this *= prop.GetValue();
	}

	template<class TProperty, class T>
	util::SimpleNumberProperty<TProperty, T> &util::SimpleNumberProperty<TProperty, T>::operator/=(const T &val)
	{
		*this = GetValue() / val;
		return *this;
	}

	template<class TProperty, class T>
	util::SimpleNumberProperty<TProperty, T> &util::SimpleNumberProperty<TProperty, T>::operator/=(const SimpleNumberProperty<TProperty, T> &prop)
	{
		return *this /= prop.GetValue();
	}

	template<class TProperty, class T>
	template<typename, typename>
	util::SimpleNumberProperty<TProperty, T> &util::SimpleNumberProperty<TProperty, T>::operator%=(const T &val)
	{
		*this = GetValue() % val;
		return *this;
	}

	template<class TProperty, class T>
	template<typename, typename>
	util::SimpleNumberProperty<TProperty, T> &util::SimpleNumberProperty<TProperty, T>::operator%=(const SimpleNumberProperty<TProperty, T> &prop)
	{
		return *this %= prop.GetValue();
	}

	template<class TProperty, class T>
	template<typename, typename>
	util::SimpleNumberProperty<TProperty, T> &util::SimpleNumberProperty<TProperty, T>::operator&=(const T &val)
	{
		*this = GetValue() & val;
		return *this;
	}

	template<class TProperty, class T>
	template<typename, typename>
	util::SimpleNumberProperty<TProperty, T> &util::SimpleNumberProperty<TProperty, T>::operator&=(const SimpleNumberProperty<TProperty, T> &prop)
	{
		return *this &= prop.GetValue();
	}

	template<class TProperty, class T>
	template<typename, typename>
	util::SimpleNumberProperty<TProperty, T> &util::SimpleNumberProperty<TProperty, T>::operator|=(const T &val)
	{
		*this = GetValue() | val;
		return *this;
	}

	template<class TProperty, class T>
	template<typename, typename>
	util::SimpleNumberProperty<TProperty, T> &util::SimpleNumberProperty<TProperty, T>::operator|=(const SimpleNumberProperty<TProperty, T> &prop)
	{
		return *this |= prop.GetValue();
	}

	template<class TProperty, class T>
	template<typename, typename>
	util::SimpleNumberProperty<TProperty, T> &util::SimpleNumberProperty<TProperty, T>::operator^=(const T &val)
	{
		*this = GetValue() ^ val;
		return *this;
	}

	template<class TProperty, class T>
	template<typename, typename>
	util::SimpleNumberProperty<TProperty, T> &util::SimpleNumberProperty<TProperty, T>::operator^=(const SimpleNumberProperty<TProperty, T> &prop)
	{
		return *this ^= prop.GetValue();
	}

	template<class TProperty, class T>
	template<typename, typename>
	util::SimpleNumberProperty<TProperty, T> &util::SimpleNumberProperty<TProperty, T>::operator<<=(const T &val)
	{
		*this = GetValue() << val;
		return *this;
	}

	template<class TProperty, class T>
	template<typename, typename>
	util::SimpleNumberProperty<TProperty, T> &util::SimpleNumberProperty<TProperty, T>::operator<<=(const SimpleNumberProperty<TProperty, T> &prop)
	{
		return *this <<= prop.GetValue();
	}

	template<class TProperty, class T>
	template<typename, typename>
	util::SimpleNumberProperty<TProperty, T> &util::SimpleNumberProperty<TProperty, T>::operator>>=(const T &val)
	{
		*this = GetValue() >> val;
		return *this;
	}

	template<class TProperty, class T>
	template<typename, typename>
	util::SimpleNumberProperty<TProperty, T> &util::SimpleNumberProperty<TProperty, T>::operator>>=(const SimpleNumberProperty<TProperty, T> &prop)
	{
		return *this >>= prop.GetValue();
	}

	template<class TProperty, class T>
	util::SimpleNumberProperty<TProperty, T> &util::SimpleNumberProperty<TProperty, T>::operator++()
	{
		++GetValue();
		return *this;
	}

	template<class TProperty, class T>
	util::SimpleNumberProperty<TProperty, T> &util::SimpleNumberProperty<TProperty, T>::operator--()
	{
		--GetValue();
		return *this;
	}

	template<class TProperty, class T>
	T util::SimpleNumberProperty<TProperty, T>::operator++(int)
	{
		return GetValue()++;
	}

	template<class TProperty, class T>
	T util::SimpleNumberProperty<TProperty, T>::operator--(int)
	{
		return GetValue()--;
	}

	template<class TProperty, class T>
	T util::SimpleNumberProperty<TProperty, T>::operator+() const
	{
		return +GetValue();
	}

	template<class TProperty, class T>
	template<typename, typename>
	T util::SimpleNumberProperty<TProperty, T>::operator-() const
	{
		return -GetValue();
	}

	template<class TProperty, class T>
	T util::SimpleNumberProperty<TProperty, T>::operator+(const T &val) const
	{
		return GetValue() + val;
	}

	template<class TProperty, class T>
	T util::SimpleNumberProperty<TProperty, T>::operator+(const SimpleNumberProperty<TProperty, T> &prop) const
	{
		return *this + prop.GetValue();
	}

	template<class TProperty, class T>
	T util::SimpleNumberProperty<TProperty, T>::operator-(const T &val) const
	{
		return GetValue() - val;
	}

	template<class TProperty, class T>
	T util::SimpleNumberProperty<TProperty, T>::operator-(const SimpleNumberProperty<TProperty, T> &prop) const
	{
		return *this - prop.GetValue();
	}

	template<class TProperty, class T>
	T util::SimpleNumberProperty<TProperty, T>::operator*(const T &val) const
	{
		return GetValue() * val;
	}

	template<class TProperty, class T>
	T util::SimpleNumberProperty<TProperty, T>::operator*(const SimpleNumberProperty<TProperty, T> &prop) const
	{
		return *this * prop.GetValue();
	}

	template<class TProperty, class T>
	T util::SimpleNumberProperty<TProperty, T>::operator/(const T &val) const
	{
		return GetValue() / val;
	}

	template<class TProperty, class T>
	T util::SimpleNumberProperty<TProperty, T>::operator/(const SimpleNumberProperty<TProperty, T> &prop) const
	{
		return *this / prop.GetValue();
	}

	template<class TProperty, class T>
	template<typename, typename>
	T util::SimpleNumberProperty<TProperty, T>::operator%(const T &val) const
	{
		return GetValue() % val;
	}

	template<class TProperty, class T>
	template<typename, typename>
	T util::SimpleNumberProperty<TProperty, T>::operator%(const SimpleNumberProperty<TProperty, T> &prop) const
	{
		return *this % prop.GetValue();
	}

	template<class TProperty, class T>
	template<typename, typename>
	T util::SimpleNumberProperty<TProperty, T>::operator~() const
	{
		return ~GetValue();
	}

	template<class TProperty, class T>
	template<typename, typename>
	T util::SimpleNumberProperty<TProperty, T>::operator&(const T &val) const
	{
		return GetValue() & val;
	}

	template<class TProperty, class T>
	template<typename, typename>
	T util::SimpleNumberProperty<TProperty, T>::operator&(const SimpleNumberProperty<TProperty, T> &prop) const
	{
		return *this & prop.GetValue();
	}

	template<class TProperty, class T>
	template<typename, typename>
	T util::SimpleNumberProperty<TProperty, T>::operator|(const T &val) const
	{
		return GetValue() | val;
	}

	template<class TProperty, class T>
	template<typename, typename>
	T util::SimpleNumberProperty<TProperty, T>::operator|(const SimpleNumberProperty<TProperty, T> &prop) const
	{
		return *this | prop.GetValue();
	}

	template<class TProperty, class T>
	template<typename, typename>
	T util::SimpleNumberProperty<TProperty, T>::operator^(const T &val) const
	{
		return GetValue() ^ val;
	}

	template<class TProperty, class T>
	template<typename, typename>
	T util::SimpleNumberProperty<TProperty, T>::operator^(const SimpleNumberProperty<TProperty, T> &prop) const
	{
		return *this ^ prop.GetValue();
	}

	template<class TProperty, class T>
	template<typename, typename>
	T util::SimpleNumberProperty<TProperty, T>::operator<<(const T &val) const
	{
		return GetValue() << val;
	}

	template<class TProperty, class T>
	template<typename, typename>
	T util::SimpleNumberProperty<TProperty, T>::operator<<(const SimpleNumberProperty<TProperty, T> &prop) const
	{
		return *this << prop.GetValue();
	}

	template<class TProperty, class T>
	template<typename, typename>
	T util::SimpleNumberProperty<TProperty, T>::operator>>(const T &val) const
	{
		return GetValue() >> val;
	}

	template<class TProperty, class T>
	template<typename, typename>
	T util::SimpleNumberProperty<TProperty, T>::operator>>(const SimpleNumberProperty<TProperty, T> &prop) const
	{
		return *this >> prop.GetValue();
	}

	template<class TProperty, class T>
	bool util::SimpleNumberProperty<TProperty, T>::operator!() const
	{
		return !GetValue();
	}

	template<class TProperty, class T>
	bool util::SimpleNumberProperty<TProperty, T>::operator&&(const T &val) const
	{
		return (GetValue() && val);
	}

	template<class TProperty, class T>
	bool util::SimpleNumberProperty<TProperty, T>::operator&&(const SimpleNumberProperty<TProperty, T> &prop) const
	{
		return (*this && prop.GetValue());
	}

	template<class TProperty, class T>
	bool util::SimpleNumberProperty<TProperty, T>::operator||(const T &val) const
	{
		return (GetValue() || val);
	}

	template<class TProperty, class T>
	bool util::SimpleNumberProperty<TProperty, T>::operator||(const SimpleNumberProperty<TProperty, T> &prop) const
	{
		return (*this || prop.GetValue());
	}

	template<class TProperty, class T>
	bool util::SimpleNumberProperty<TProperty, T>::operator<(const T &val) const
	{
		return (GetValue() < val);
	}

	template<class TProperty, class T>
	bool util::SimpleNumberProperty<TProperty, T>::operator<(const SimpleProperty<TProperty, T> &prop) const
	{
		return (*this < prop.GetValue());
	}

	template<class TProperty, class T>
	bool util::SimpleNumberProperty<TProperty, T>::operator>(const T &val) const
	{
		return (GetValue() > val);
	}

	template<class TProperty, class T>
	bool util::SimpleNumberProperty<TProperty, T>::operator>(const SimpleProperty<TProperty, T> &prop) const
	{
		return (*this > prop.GetValue());
	}

	template<class TProperty, class T>
	bool util::SimpleNumberProperty<TProperty, T>::operator<=(const T &val) const
	{
		return (GetValue() <= val);
	}

	template<class TProperty, class T>
	bool util::SimpleNumberProperty<TProperty, T>::operator<=(const SimpleProperty<TProperty, T> &prop) const
	{
		return (*this <= prop.GetValue());
	}

	template<class TProperty, class T>
	bool util::SimpleNumberProperty<TProperty, T>::operator>=(const T &val) const
	{
		return (GetValue() >= val);
	}

	template<class TProperty, class T>
	bool util::SimpleNumberProperty<TProperty, T>::operator>=(const SimpleProperty<TProperty, T> &prop) const
	{
		return (*this >= prop.GetValue());
	}

	template<class TProperty, class T>
	util::SimpleNumberProperty<TProperty, T>::operator const T &() const
	{
		return SimpleProperty<TProperty, T>::operator const T &();
	}

	template<class TProperty, class T>
	util::SimpleNumberProperty<TProperty, T>::operator T &()
	{
		return SimpleProperty<TProperty, T>::operator T &();
	}
	#pragma warning(pop)

	#define DEFINE_SIMPLE_NUMBER_PROPERTY(name, type)                                                                                                                                                                                                                                                \
		class DLLSHUTIL name##Property : public SimpleNumberProperty<name##Property, type> {                                                                                                                                                                                                         \
		public:                                                                                                                                                                                                                                                                                    \
	name##Property() : SimpleNumberProperty<name##Property, type>{} {} \
	name##Property(const type &init) : SimpleNumberProperty<name##Property, type>{init} {} \
	name##Property(const SimpleNumberProperty<name##Property, type> &prop) : SimpleNumberProperty<name##Property, type>{prop} {} \
	\
	name##Property &operator=(const type &val) { return static_cast<name##Property&>(SimpleNumberProperty<name##Property, type>::operator=(val)); } \
	name##Property &operator=(const name##Property &other) { return static_cast<name##Property&>(SimpleNumberProperty<name##Property, type>::operator=(other)); } \
		};                                                                                                                                                                                                                                                                                           \
		using P##name##Property = std::shared_ptr<name##Property>;

	#pragma warning(push)
	#pragma warning(disable : 4251)
	namespace util {
		DEFINE_SIMPLE_NUMBER_PROPERTY(Int8, int8_t);
		DEFINE_SIMPLE_NUMBER_PROPERTY(UInt8, uint8_t);
		DEFINE_SIMPLE_NUMBER_PROPERTY(Int16, int16_t);
		DEFINE_SIMPLE_NUMBER_PROPERTY(UInt16, uint16_t);
		DEFINE_SIMPLE_NUMBER_PROPERTY(Int32, int32_t);
		DEFINE_SIMPLE_NUMBER_PROPERTY(UInt32, uint32_t);
		DEFINE_SIMPLE_NUMBER_PROPERTY(Int64, int64_t);
		DEFINE_SIMPLE_NUMBER_PROPERTY(UInt64, uint64_t);

		DEFINE_SIMPLE_NUMBER_PROPERTY(Float, float);
		DEFINE_SIMPLE_NUMBER_PROPERTY(Double, double);
		DEFINE_SIMPLE_NUMBER_PROPERTY(LongDouble, long double);

		template<typename TEnum>
		class TEnumProperty : public SimpleNumberProperty<TEnumProperty<TEnum>, TEnum> {
		public:
			using SimpleNumberProperty<TEnumProperty<TEnum>, TEnum>::SimpleNumberProperty;
			using SimpleNumberProperty<TEnumProperty<TEnum>, TEnum>::operator=;
		};

		template<typename TEnum>
		using PEnumProperty = std::shared_ptr<TEnumProperty<TEnum>>;
	};
	#pragma warning(pop)
}
