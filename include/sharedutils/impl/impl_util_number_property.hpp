/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __IMPL_UTIL_NUMBER_PROPERTY_HPP__
#define __IMPL_UTIL_NUMBER_PROPERTY_HPP__

#pragma warning(push)
#pragma warning(disable : 4544)
namespace util
{
	template<class TProperty,class T>
		class SimpleNumberProperty
			: public SimpleProperty<TProperty,T>
		{
		public:
			using SimpleProperty<TProperty,T>::SimpleProperty;
			using SimpleProperty<TProperty,T>::GetValue;
			using SimpleProperty<TProperty,T>::operator*;
			using SimpleProperty<TProperty,T>::operator=;

			// Assignment
			SimpleNumberProperty &operator+=(const T &val);
			SimpleNumberProperty &operator+=(const SimpleNumberProperty<TProperty,T> &prop);
			SimpleNumberProperty &operator-=(const T &val);
			SimpleNumberProperty &operator-=(const SimpleNumberProperty<TProperty,T> &prop);
			SimpleNumberProperty &operator*=(const T &val);
			SimpleNumberProperty &operator*=(const SimpleNumberProperty<TProperty,T> &prop);
			SimpleNumberProperty &operator/=(const T &val);
			SimpleNumberProperty &operator/=(const SimpleNumberProperty<TProperty,T> &prop);
			template<typename T1=T,typename=typename std::enable_if_t<std::is_integral<T1>::value>>
				SimpleNumberProperty &operator%=(const T &val);
			template<typename T1=T,typename=typename std::enable_if_t<std::is_integral<T1>::value>>
				SimpleNumberProperty &operator%=(const SimpleNumberProperty<TProperty,T> &prop);
			template<typename T1=T,typename=typename std::enable_if_t<std::is_integral<T1>::value>>
				SimpleNumberProperty &operator&=(const T &val);
			template<typename T1=T,typename=typename std::enable_if_t<std::is_integral<T1>::value>>
				SimpleNumberProperty &operator&=(const SimpleNumberProperty<TProperty,T> &prop);
			template<typename T1=T,typename=typename std::enable_if_t<std::is_integral<T1>::value>>
				SimpleNumberProperty &operator|=(const T &val);
			template<typename T1=T,typename=typename std::enable_if_t<std::is_integral<T1>::value>>
				SimpleNumberProperty &operator|=(const SimpleNumberProperty<TProperty,T> &prop);
			template<typename T1=T,typename=typename std::enable_if_t<std::is_integral<T1>::value>>
				SimpleNumberProperty &operator^=(const T &val);
			template<typename T1=T,typename=typename std::enable_if_t<std::is_integral<T1>::value>>
				SimpleNumberProperty &operator^=(const SimpleNumberProperty<TProperty,T> &prop);
			template<typename T1=T,typename=typename std::enable_if_t<std::is_integral<T1>::value>>
				SimpleNumberProperty &operator<<=(const T &val);
			template<typename T1=T,typename=typename std::enable_if_t<std::is_integral<T1>::value>>
				SimpleNumberProperty &operator<<=(const SimpleNumberProperty<TProperty,T> &prop);
			template<typename T1=T,typename=typename std::enable_if_t<std::is_integral<T1>::value>>
				SimpleNumberProperty &operator>>=(const T &val);
			template<typename T1=T,typename=typename std::enable_if_t<std::is_integral<T1>::value>>
				SimpleNumberProperty &operator>>=(const SimpleNumberProperty<TProperty,T> &prop);

			// Increment / Decrement
			SimpleNumberProperty &operator++();
			SimpleNumberProperty &operator--();
			T operator++(int);
			T operator--(int);

			// Arithmetic
			T operator+() const;
			template<typename T1=T,typename=typename std::enable_if_t<std::is_unsigned<T1>::value>>
				T operator-() const;
			T operator+(const T &val) const;
			T operator+(const SimpleNumberProperty<TProperty,T> &prop) const;
			T operator-(const T &val) const;
			T operator-(const SimpleNumberProperty<TProperty,T> &prop) const;
			T operator*(const T &val) const;
			T operator*(const SimpleNumberProperty<TProperty,T> &prop) const;
			T operator/(const T &val) const;
			T operator/(const SimpleNumberProperty<TProperty,T> &prop) const;
			template<typename T1=T,typename=typename std::enable_if_t<std::is_integral<T1>::value>>
				T operator%(const T &val) const;
			template<typename T1=T,typename=typename std::enable_if_t<std::is_integral<T1>::value>>
				T operator%(const SimpleNumberProperty<TProperty,T> &prop) const;
			template<typename T1=T,typename=typename std::enable_if_t<std::is_integral<T1>::value>>
				T operator~() const;
			template<typename T1=T,typename=typename std::enable_if_t<std::is_integral<T1>::value>>
				T operator&(const T &val) const;
			template<typename T1=T,typename=typename std::enable_if_t<std::is_integral<T1>::value>>
				T operator&(const SimpleNumberProperty<TProperty,T> &prop) const;
			template<typename T1=T,typename=typename std::enable_if_t<std::is_integral<T1>::value>>
				T operator|(const T &val) const;
			template<typename T1=T,typename=typename std::enable_if_t<std::is_integral<T1>::value>>
				T operator|(const SimpleNumberProperty<TProperty,T> &prop) const;
			template<typename T1=T,typename=typename std::enable_if_t<std::is_integral<T1>::value>>
				T operator^(const T &val) const;
			template<typename T1=T,typename=typename std::enable_if_t<std::is_integral<T1>::value>>
				T operator^(const SimpleNumberProperty<TProperty,T> &prop) const;
			template<typename T1=T,typename=typename std::enable_if_t<std::is_integral<T1>::value>>
				T operator<<(const T &val) const;
			template<typename T1=T,typename=typename std::enable_if_t<std::is_integral<T1>::value>>
				T operator<<(const SimpleNumberProperty<TProperty,T> &prop) const;
			template<typename T1=T,typename=typename std::enable_if_t<std::is_integral<T1>::value>>
				T operator>>(const T &val) const;
			template<typename T1=T,typename=typename std::enable_if_t<std::is_integral<T1>::value>>
				T operator>>(const SimpleNumberProperty<TProperty,T> &prop) const;

			// Logical
			bool operator!() const;
			bool operator&&(const T &val) const;
			bool operator&&(const SimpleNumberProperty<TProperty,T> &prop) const;
			bool operator||(const T &val) const;
			bool operator||(const SimpleNumberProperty<TProperty,T> &prop) const;

			// Comparison
			bool operator<(const T &val) const;
			bool operator<(const SimpleProperty<TProperty,T> &prop) const;
			bool operator>(const T &val) const;
			bool operator>(const SimpleProperty<TProperty,T> &prop) const;
			bool operator<=(const T &val) const;
			bool operator<=(const SimpleProperty<TProperty,T> &prop) const;
			bool operator>=(const T &val) const;
			bool operator>=(const SimpleProperty<TProperty,T> &prop) const;

			operator const T&() const;
			operator T&();
		};
};

template<class TProperty,class T>
	util::SimpleNumberProperty<TProperty,T> &util::SimpleNumberProperty<TProperty,T>::operator+=(const T &val)
{
	*this = GetValue() +val;
	return *this;
}

template<class TProperty,class T>
	util::SimpleNumberProperty<TProperty,T> &util::SimpleNumberProperty<TProperty,T>::operator+=(const SimpleNumberProperty<TProperty,T> &prop) {return *this +=prop.GetValue();}

template<class TProperty,class T>
	util::SimpleNumberProperty<TProperty,T> &util::SimpleNumberProperty<TProperty,T>::operator-=(const T &val)
{
	*this = GetValue() -val;
	return *this;
}

template<class TProperty,class T>
	util::SimpleNumberProperty<TProperty,T> &util::SimpleNumberProperty<TProperty,T>::operator-=(const SimpleNumberProperty<TProperty,T> &prop) {return *this -=prop.GetValue();}

template<class TProperty,class T>
	util::SimpleNumberProperty<TProperty,T> &util::SimpleNumberProperty<TProperty,T>::operator*=(const T &val)
{
	*this = GetValue() *val;
	return *this;
}

template<class TProperty,class T>
	util::SimpleNumberProperty<TProperty,T> &util::SimpleNumberProperty<TProperty,T>::operator*=(const SimpleNumberProperty<TProperty,T> &prop) {return *this *=prop.GetValue();}

template<class TProperty,class T>
	util::SimpleNumberProperty<TProperty,T> &util::SimpleNumberProperty<TProperty,T>::operator/=(const T &val)
{
	*this = GetValue() /val;
	return *this;
}

template<class TProperty,class T>
	util::SimpleNumberProperty<TProperty,T> &util::SimpleNumberProperty<TProperty,T>::operator/=(const SimpleNumberProperty<TProperty,T> &prop) {return *this /=prop.GetValue();}

template<class TProperty,class T>
	template<typename,typename>
		util::SimpleNumberProperty<TProperty,T> &util::SimpleNumberProperty<TProperty,T>::operator%=(const T &val)
{
	*this = GetValue() %val;
	return *this;
}

template<class TProperty,class T>
	template<typename,typename>
		util::SimpleNumberProperty<TProperty,T> &util::SimpleNumberProperty<TProperty,T>::operator%=(const SimpleNumberProperty<TProperty,T> &prop) {return *this %=prop.GetValue();}

template<class TProperty,class T>
	template<typename,typename>
		util::SimpleNumberProperty<TProperty,T> &util::SimpleNumberProperty<TProperty,T>::operator&=(const T &val)
{
	*this = GetValue() &val;
	return *this;
}

template<class TProperty,class T>
	template<typename,typename>
		util::SimpleNumberProperty<TProperty,T> &util::SimpleNumberProperty<TProperty,T>::operator&=(const SimpleNumberProperty<TProperty,T> &prop) {return *this &=prop.GetValue();}

template<class TProperty,class T>
	template<typename,typename>
		util::SimpleNumberProperty<TProperty,T> &util::SimpleNumberProperty<TProperty,T>::operator|=(const T &val)
{
	*this = GetValue() |val;
	return *this;
}

template<class TProperty,class T>
	template<typename,typename>
		util::SimpleNumberProperty<TProperty,T> &util::SimpleNumberProperty<TProperty,T>::operator|=(const SimpleNumberProperty<TProperty,T> &prop) {return *this |=prop.GetValue();}

template<class TProperty,class T>
	template<typename,typename>
		util::SimpleNumberProperty<TProperty,T> &util::SimpleNumberProperty<TProperty,T>::operator^=(const T &val)
{
	*this = GetValue() ^val;
	return *this;
}

template<class TProperty,class T>
	template<typename,typename>
		util::SimpleNumberProperty<TProperty,T> &util::SimpleNumberProperty<TProperty,T>::operator^=(const SimpleNumberProperty<TProperty,T> &prop) {return *this ^=prop.GetValue();}

template<class TProperty,class T>
	template<typename,typename>
		util::SimpleNumberProperty<TProperty,T> &util::SimpleNumberProperty<TProperty,T>::operator<<=(const T &val)
{
	*this = GetValue() <<val;
	return *this;
}

template<class TProperty,class T>
	template<typename,typename>
		util::SimpleNumberProperty<TProperty,T> &util::SimpleNumberProperty<TProperty,T>::operator<<=(const SimpleNumberProperty<TProperty,T> &prop) {return *this <<=prop.GetValue();}

template<class TProperty,class T>
	template<typename,typename>
		util::SimpleNumberProperty<TProperty,T> &util::SimpleNumberProperty<TProperty,T>::operator>>=(const T &val)
{
	*this = GetValue() >>val;
	return *this;
}

template<class TProperty,class T>
	template<typename,typename>
		util::SimpleNumberProperty<TProperty,T> &util::SimpleNumberProperty<TProperty,T>::operator>>=(const SimpleNumberProperty<TProperty,T> &prop) {return *this >>=prop.GetValue();}

template<class TProperty,class T>
	util::SimpleNumberProperty<TProperty,T> &util::SimpleNumberProperty<TProperty,T>::operator++()
{
	++GetValue();
	return *this;
}

template<class TProperty,class T>
	util::SimpleNumberProperty<TProperty,T> &util::SimpleNumberProperty<TProperty,T>::operator--()
{
	--GetValue();
	return *this;
}

template<class TProperty,class T>
	T util::SimpleNumberProperty<TProperty,T>::operator++(int) {return GetValue()++;}

template<class TProperty,class T>
	T util::SimpleNumberProperty<TProperty,T>::operator--(int) {return GetValue()--;}

template<class TProperty,class T>
	T util::SimpleNumberProperty<TProperty,T>::operator+() const {return +GetValue();}

template<class TProperty,class T>
	template<typename,typename>
		T util::SimpleNumberProperty<TProperty,T>::operator-() const {return -GetValue();}

template<class TProperty,class T>
	T util::SimpleNumberProperty<TProperty,T>::operator+(const T &val) const {return GetValue() +val;}

template<class TProperty,class T>
	T util::SimpleNumberProperty<TProperty,T>::operator+(const SimpleNumberProperty<TProperty,T> &prop) const {return *this +prop.GetValue();}

template<class TProperty,class T>
	T util::SimpleNumberProperty<TProperty,T>::operator-(const T &val) const {return GetValue() -val;}

template<class TProperty,class T>
	T util::SimpleNumberProperty<TProperty,T>::operator-(const SimpleNumberProperty<TProperty,T> &prop) const {return *this -prop.GetValue();}

template<class TProperty,class T>
	T util::SimpleNumberProperty<TProperty,T>::operator*(const T &val) const {return GetValue() *val;}

template<class TProperty,class T>
	T util::SimpleNumberProperty<TProperty,T>::operator*(const SimpleNumberProperty<TProperty,T> &prop) const {return *this *prop.GetValue();}

template<class TProperty,class T>
	T util::SimpleNumberProperty<TProperty,T>::operator/(const T &val) const {return GetValue() /val;}

template<class TProperty,class T>
	T util::SimpleNumberProperty<TProperty,T>::operator/(const SimpleNumberProperty<TProperty,T> &prop) const {return *this /prop.GetValue();}

template<class TProperty,class T>
	template<typename,typename>
		T util::SimpleNumberProperty<TProperty,T>::operator%(const T &val) const {return GetValue() %val;}

template<class TProperty,class T>
	template<typename,typename>
		T util::SimpleNumberProperty<TProperty,T>::operator%(const SimpleNumberProperty<TProperty,T> &prop) const {return *this %prop.GetValue();}

template<class TProperty,class T>
	template<typename,typename>
		T util::SimpleNumberProperty<TProperty,T>::operator~() const {return ~GetValue();}

template<class TProperty,class T>
	template<typename,typename>
		T util::SimpleNumberProperty<TProperty,T>::operator&(const T &val) const {return GetValue() &val;}

template<class TProperty,class T>
	template<typename,typename>
		T util::SimpleNumberProperty<TProperty,T>::operator&(const SimpleNumberProperty<TProperty,T> &prop) const {return *this &prop.GetValue();}

template<class TProperty,class T>
	template<typename,typename>
		T util::SimpleNumberProperty<TProperty,T>::operator|(const T &val) const {return GetValue() |val;}

template<class TProperty,class T>
	template<typename,typename>
		T util::SimpleNumberProperty<TProperty,T>::operator|(const SimpleNumberProperty<TProperty,T> &prop) const {return *this |prop.GetValue();}

template<class TProperty,class T>
	template<typename,typename>
		T util::SimpleNumberProperty<TProperty,T>::operator^(const T &val) const {return GetValue() ^val;}

template<class TProperty,class T>
	template<typename,typename>
		T util::SimpleNumberProperty<TProperty,T>::operator^(const SimpleNumberProperty<TProperty,T> &prop) const {return *this ^prop.GetValue();}

template<class TProperty,class T>
	template<typename,typename>
		T util::SimpleNumberProperty<TProperty,T>::operator<<(const T &val) const {return GetValue() <<val;}

template<class TProperty,class T>
	template<typename,typename>
		T util::SimpleNumberProperty<TProperty,T>::operator<<(const SimpleNumberProperty<TProperty,T> &prop) const {return *this <<prop.GetValue();}

template<class TProperty,class T>
	template<typename,typename>
		T util::SimpleNumberProperty<TProperty,T>::operator>>(const T &val) const {return GetValue() >>val;}

template<class TProperty,class T>
	template<typename,typename>
		T util::SimpleNumberProperty<TProperty,T>::operator>>(const SimpleNumberProperty<TProperty,T> &prop) const {return *this >>prop.GetValue();}

template<class TProperty,class T>
	bool util::SimpleNumberProperty<TProperty,T>::operator!() const {return !GetValue();}

template<class TProperty,class T>
	bool util::SimpleNumberProperty<TProperty,T>::operator&&(const T &val) const {return (GetValue() && val);}

template<class TProperty,class T>
	bool util::SimpleNumberProperty<TProperty,T>::operator&&(const SimpleNumberProperty<TProperty,T> &prop) const {return (*this && prop.GetValue());}

template<class TProperty,class T>
	bool util::SimpleNumberProperty<TProperty,T>::operator||(const T &val) const {return (GetValue() || val);}

template<class TProperty,class T>
	bool util::SimpleNumberProperty<TProperty,T>::operator||(const SimpleNumberProperty<TProperty,T> &prop) const {return (*this || prop.GetValue());}

template<class TProperty,class T>
	bool util::SimpleNumberProperty<TProperty,T>::operator<(const T &val) const {return (GetValue() < val);}

template<class TProperty,class T>
	bool util::SimpleNumberProperty<TProperty,T>::operator<(const SimpleProperty<TProperty,T> &prop) const {return (*this < prop.GetValue());}

template<class TProperty,class T>
	bool util::SimpleNumberProperty<TProperty,T>::operator>(const T &val) const {return (GetValue() > val);}

template<class TProperty,class T>
	bool util::SimpleNumberProperty<TProperty,T>::operator>(const SimpleProperty<TProperty,T> &prop) const {return (*this > prop.GetValue());}

template<class TProperty,class T>
	bool util::SimpleNumberProperty<TProperty,T>::operator<=(const T &val) const {return (GetValue() <= val);}

template<class TProperty,class T>
	bool util::SimpleNumberProperty<TProperty,T>::operator<=(const SimpleProperty<TProperty,T> &prop) const {return (*this <= prop.GetValue());}

template<class TProperty,class T>
	bool util::SimpleNumberProperty<TProperty,T>::operator>=(const T &val) const {return (GetValue() >= val);}

template<class TProperty,class T>
	bool util::SimpleNumberProperty<TProperty,T>::operator>=(const SimpleProperty<TProperty,T> &prop) const {return (*this >= prop.GetValue());}

template<class TProperty,class T>
	util::SimpleNumberProperty<TProperty,T>::operator const T&() const {return SimpleProperty<TProperty,T>::operator const T&();}

template<class TProperty,class T>
	util::SimpleNumberProperty<TProperty,T>::operator T&() {return SimpleProperty<TProperty,T>::operator T&();}

#pragma warning(pop)

#endif
