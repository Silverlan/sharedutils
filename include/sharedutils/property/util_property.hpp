/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __UTIL_PROPERTY_HPP__
#define __UTIL_PROPERTY_HPP__

#include "../utildefinitions.h"
#include "../functioncallback.h"
#include <vector>
#include <cinttypes>
#include <mathutil/umath.h>

#pragma warning(push)
#pragma warning(disable : 4251)
namespace util
{
	class DLLSHUTIL BaseProperty
	{
	public:
		void AddCallback(const CallbackHandle &hCallback);
		virtual ~BaseProperty()=default;
	protected:
		BaseProperty()=default;
		std::vector<CallbackHandle> m_callbacks;
	};

	enum class SimplePropertyFlags : uint8_t
	{
		None = 0u,
		Active = 1u,
		Locked = Active<<1u,
		LockedLink = Locked<<1u, // If this flag isn't set, the property can still be changed by its link even if the Locked flag is set
		TemporarilyUnlocked = LockedLink<<1u // If this flag is set, the property will be unlocked for the next change (and only the next change) if it is locked
	};
	template<class TProperty,class T>
		class SimpleProperty
			: public BaseProperty,
			public std::enable_shared_from_this<SimpleProperty<TProperty,T>>
	{
	public:
		static std::shared_ptr<TProperty> Create(const T &value={});
		SimpleProperty()=default;
		SimpleProperty(const T &init);
		SimpleProperty(const SimpleProperty<TProperty,T> &prop);
		virtual ~SimpleProperty() override;
		SimpleProperty &operator=(const T &val);
		SimpleProperty &operator=(const SimpleProperty<TProperty,T> &other);
		bool operator==(const T &val) const;
		bool operator==(const SimpleProperty<TProperty,T> &prop) const;
		bool operator!=(const T &val) const;
		bool operator!=(const SimpleProperty<TProperty,T> &prop) const;
		operator const T&() const;
		operator T&();
		T &operator*();
		const T &operator*() const;
		T *operator->();
		const T *operator->() const;
		const T &GetValue() const;
		T &GetValue();
		CallbackHandle AddCallback(const std::function<void(std::reference_wrapper<const T>,std::reference_wrapper<const T>)> &callback);
		CallbackHandle AddModifier(const std::function<void(std::reference_wrapper<T>)> &fModifier);
		void InvokeCallbacks();

		void SetLocked(bool bLocked,bool bLinkLocked=true);
		bool IsLocked() const;

		template<class TPropertyOther,class TOther>
			void Link(SimpleProperty<TPropertyOther,TOther> &other,const std::function<T(const TOther&)> &converter);
		template<class TPropertyOther,class TOther>
			void Link(SimpleProperty<TPropertyOther,TOther> &other);
		void Unlink();
	protected:
		virtual void ApplyValue(const T &newValue);
		void OnChanged(const T &oldValue,const T &newValue);
		T m_value = {};
		SimplePropertyFlags m_flags = SimplePropertyFlags::None;
		std::vector<CallbackHandle> m_modifiers;
		CallbackHandle m_linkedCallback = {};
	};

	class DLLSHUTIL BoolProperty
		: public SimpleProperty<BoolProperty,bool>
	{
	public:
		using SimpleProperty<BoolProperty,bool>::SimpleProperty;
		using SimpleProperty<BoolProperty,bool>::operator=;
		virtual void ApplyValue(const bool &newValue) override;
	};
	using PBoolProperty = std::shared_ptr<BoolProperty>;
};
REGISTER_BASIC_BITWISE_OPERATORS(util::SimplePropertyFlags);
#pragma warning(pop)

#include "sharedutils/impl/impl_util_number_property.hpp"
#include "sharedutils/impl/impl_util_string_property.hpp"

#define DEFINE_SIMPLE_NUMBER_PROPERTY(name,type) \
	class DLLSHUTIL name##Property \
		: public SimpleNumberProperty<name##Property,type> \
	{ \
	public: \
		using SimpleNumberProperty<name##Property,type>::SimpleNumberProperty; \
		using SimpleNumberProperty<name##Property,type>::operator=; \
	}; \
	using P##name##Property = std::shared_ptr<name##Property>;

#pragma warning(push)
#pragma warning(disable : 4251)
namespace util
{
	DEFINE_SIMPLE_NUMBER_PROPERTY(Int8,int8_t);
	DEFINE_SIMPLE_NUMBER_PROPERTY(UInt8,uint8_t);
	DEFINE_SIMPLE_NUMBER_PROPERTY(Int16,int16_t);
	DEFINE_SIMPLE_NUMBER_PROPERTY(UInt16,uint16_t);
	DEFINE_SIMPLE_NUMBER_PROPERTY(Int32,int32_t);
	DEFINE_SIMPLE_NUMBER_PROPERTY(UInt32,uint32_t);
	DEFINE_SIMPLE_NUMBER_PROPERTY(Int64,int64_t);
	DEFINE_SIMPLE_NUMBER_PROPERTY(UInt64,uint64_t);

	DEFINE_SIMPLE_NUMBER_PROPERTY(Float,float);
	DEFINE_SIMPLE_NUMBER_PROPERTY(Double,double);
	DEFINE_SIMPLE_NUMBER_PROPERTY(LongDouble,long double);

	template<typename TEnum>
		class TEnumProperty
			: public SimpleNumberProperty<TEnumProperty<TEnum>,TEnum>
		{
		public:
			using SimpleNumberProperty<TEnumProperty<TEnum>,TEnum>::SimpleNumberProperty;
			using SimpleNumberProperty<TEnumProperty<TEnum>,TEnum>::operator=;
		};

	template<typename TEnum>
		using PEnumProperty = std::shared_ptr<TEnumProperty<TEnum>>;
};
#pragma warning(pop)

template<class TProperty,class T>
	std::shared_ptr<TProperty> util::SimpleProperty<TProperty,T>::Create(const T &value) {return std::make_shared<TProperty>(value);};

template<class TProperty,class T>
	util::SimpleProperty<TProperty,T>::SimpleProperty(const T &init)
		: BaseProperty(),m_value(init)
{}

template<class TProperty,class T>
	util::SimpleProperty<TProperty,T>::SimpleProperty(const util::SimpleProperty<TProperty,T> &prop)
		: BaseProperty(),m_value(prop.m_value)
{}

template<class TProperty,class T>
	void util::SimpleProperty<TProperty,T>::SetLocked(bool bLocked,bool bLinkLocked)
{
	if(bLocked)
	{
		m_flags |= SimplePropertyFlags::Locked;
		if(bLinkLocked)
			m_flags |= SimplePropertyFlags::LockedLink;
	}
	else
		m_flags &= ~(SimplePropertyFlags::Locked | SimplePropertyFlags::LockedLink);
}
template<class TProperty,class T>
	bool util::SimpleProperty<TProperty,T>::IsLocked() const {return (m_flags &SimplePropertyFlags::Locked) != SimplePropertyFlags::None;}

template<class TProperty,class T>
	util::SimpleProperty<TProperty,T>::~SimpleProperty()
{
	Unlink();
}

template<class TProperty,class T>
	template<class TPropertyOther,class TOther>
		void util::SimpleProperty<TProperty,T>::Link(SimpleProperty<TPropertyOther,TOther> &other,const std::function<T(const TOther&)> &converter)
{
	Unlink();
	if((m_flags &SimplePropertyFlags::LockedLink) == SimplePropertyFlags::None)
		m_flags |= SimplePropertyFlags::TemporarilyUnlocked;
	*this = converter(*other);
	m_linkedCallback = other.AddCallback([this,converter](std::reference_wrapper<const TOther>,std::reference_wrapper<const TOther> newVal) {
		if((m_flags &SimplePropertyFlags::Active) != SimplePropertyFlags::None)
			return; // Prevent recursion
		m_flags |= SimplePropertyFlags::Active;
		if((m_flags &SimplePropertyFlags::LockedLink) == SimplePropertyFlags::None)
			m_flags |= SimplePropertyFlags::TemporarilyUnlocked;
		*this = converter(newVal.get());
		m_flags &= ~SimplePropertyFlags::Active;
	});
}

template<class TProperty,class T>
	template<class TPropertyOther,class TOther>
		void util::SimpleProperty<TProperty,T>::Link(SimpleProperty<TPropertyOther,TOther> &other)
{
	Link<TPropertyOther,TOther>(other,[](const TOther &v) -> T {
		return static_cast<T>(v);
	});
}

template<class TProperty,class T>
	void util::SimpleProperty<TProperty,T>::Unlink()
{
	if(m_linkedCallback.IsValid())
		m_linkedCallback.Remove();
}

template<class TProperty,class T>
	void util::SimpleProperty<TProperty,T>::ApplyValue(const T &newValue)
{
	m_value = newValue;
}

template<class TProperty,class T>
	util::SimpleProperty<TProperty,T> &util::SimpleProperty<TProperty,T>::operator=(const T &val)
{
	if(IsLocked() && (m_flags &SimplePropertyFlags::TemporarilyUnlocked) == SimplePropertyFlags::None)
		return *this;
	m_flags &= ~SimplePropertyFlags::TemporarilyUnlocked;
	auto newVal = val;
	for(auto it=m_modifiers.begin();it!=m_modifiers.end();)
	{
		auto &cbMod = *it;
		if(cbMod.IsValid() == false)
		{
			it = m_modifiers.erase(it);
			continue;
		}
		cbMod.Call<void,std::reference_wrapper<T>>(std::reference_wrapper<T>(newVal));
		++it;
	}
	if(m_value == newVal)
		return *this;
	auto old = m_value;
	ApplyValue(newVal);
	OnChanged(old,m_value);
	return *this;
}

template<class TProperty,class T>
	void util::SimpleProperty<TProperty,T>::InvokeCallbacks()
{
	OnChanged(m_value,m_value);
}

template<class TProperty,class T>
	util::SimpleProperty<TProperty,T> &util::SimpleProperty<TProperty,T>::operator=(const SimpleProperty<TProperty,T> &other) {return operator=(other.GetValue());}

template<class TProperty,class T>
	bool util::SimpleProperty<TProperty,T>::operator==(const T &val) const {return (GetValue() == val) ? true : false;}

template<class TProperty,class T>
	bool util::SimpleProperty<TProperty,T>::operator==(const SimpleProperty<TProperty,T> &prop) const {return (*this == prop.GetValue());}

template<class TProperty,class T>
	bool util::SimpleProperty<TProperty,T>::operator!=(const T &val) const {return (*this == val) ? false : true;}

template<class TProperty,class T>
	bool util::SimpleProperty<TProperty,T>::operator!=(const SimpleProperty<TProperty,T> &prop) const {return (*this != prop.GetValue());}

template<class TProperty,class T>
	util::SimpleProperty<TProperty,T>::operator const T&() const {return GetValue();}

template<class TProperty,class T>
	util::SimpleProperty<TProperty,T>::operator T&() {return GetValue();}

template<class TProperty,class T>
	T &util::SimpleProperty<TProperty,T>::operator*() {return m_value;}
template<class TProperty,class T>
	const T &util::SimpleProperty<TProperty,T>::operator*() const {return m_value;}

template<class TProperty,class T>
	T *util::SimpleProperty<TProperty,T>::operator->() {return &m_value;}
template<class TProperty,class T>
	const T *util::SimpleProperty<TProperty,T>::operator->() const {return &m_value;}

template<class TProperty,class T>
const T &util::SimpleProperty<TProperty,T>::GetValue() const {return m_value;}

template<class TProperty,class T>
	T &util::SimpleProperty<TProperty,T>::GetValue() {return m_value;}

template<class TProperty,class T>
	void util::SimpleProperty<TProperty,T>::OnChanged(const T &oldValue,const T &newValue)
{
	for(auto it=m_callbacks.begin();it!=m_callbacks.end();)
	{
		auto &cb = *it;
		if(cb.IsValid() == false)
			it = m_callbacks.erase(it);
		else
		{
			cb(std::ref(oldValue),std::ref(newValue));
			++it;
		}
	}
}

template<class TProperty,class T>
	CallbackHandle util::SimpleProperty<TProperty,T>::AddCallback(const std::function<void(std::reference_wrapper<const T>,std::reference_wrapper<const T>)> &callback)
{
	auto hCb = FunctionCallback<void,std::reference_wrapper<const T>,std::reference_wrapper<const T>>::Create(callback);
	BaseProperty::AddCallback(hCb);
	return hCb;
}

template<class TProperty,class T>
	CallbackHandle util::SimpleProperty<TProperty,T>::AddModifier(const std::function<void(std::reference_wrapper<T>)> &fModifier)
{
	auto cb = FunctionCallback<void,std::reference_wrapper<T>>::Create(fModifier);
	m_modifiers.push_back(cb);
	return cb;
}

#endif
