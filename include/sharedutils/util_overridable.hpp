/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __UTIL_OVERRIDABLE_HPP__
#define __UTIL_OVERRIDABLE_HPP__

#include "utildefinitions.h"
#include "functioncallback.h"
#include <vector>
#include <memory>

namespace util
{
	template<typename T>
		class Overridable;
	namespace _impl
	{
		template<typename T>
			class OverridableImpl
				: public std::enable_shared_from_this<OverridableImpl<T>>
		{
		public:
			OverridableImpl<T> &operator=(const T &v)
			{
				if(m_bOverridden && m_overridden == v)
					return *this;
				auto oldVal = GetValue();
				m_overridden = v;
				m_bOverridden = true;
				CallChangeCallbacks(oldVal,m_overridden);
				return *this;
			}
			const T &operator*() const {return const_cast<OverridableImpl<T>*>(this)->operator*();}
			T &operator*() {return GetValue();}

			const T &GetValue() const {return const_cast<OverridableImpl<T>*>(this)->GetValue();}
			T &GetValue() {return m_bOverridden ? m_overridden : GetDefaultValue();}

			operator T() const {return **this;}

			const T &GetDefaultValue() const {return const_cast<OverridableImpl<T>*>(this)->GetDefaultValue();}
			T &GetDefaultValue()
			{
				if(m_parentDefault.expired() == false)
					return m_parentDefault.lock()->GetValue();
				return m_default;
			}

			const T &GetValueOr(const T &other) const {return const_cast<OverridableImpl<T>*>(this)->GetValueOr(other);}
			const T &GetValueOr(const T &other) {return m_bOverridden ? m_overridden : other;}
			T &GetValueOr(T &other) {return m_bOverridden ? m_overridden : other;}

			const T *operator->() const {return &GetValueOr(m_default);}
			T *operator->() {return &GetValueOr(m_default);}
			bool WasOverriden() const {return m_bOverridden;}
			void Reset() {m_bOverridden = false;}
			bool operator==(const T &v) const {return GetValue() == v;}
			bool operator!=(const T &v) const {return GetValue() != v;}

			CallbackHandle AddChangeCallback(const std::function<void(std::reference_wrapper<const T>,std::reference_wrapper<const T>)> &f)
			{
				m_changeCallbacks.push_back(FunctionCallback<void,std::reference_wrapper<const T>,std::reference_wrapper<const T>>::Create(f));
				return m_changeCallbacks.back();
			}
			void InvokeChangeCallbacks(bool bApplyToChildren=true)
			{
				auto &v = GetValue();
				CallChangeCallbacks(v,v,bApplyToChildren);
			}
		protected:
			static std::shared_ptr<OverridableImpl<T>> Create()
			{
				return std::shared_ptr<OverridableImpl<T>>(new OverridableImpl<T>());
			}
			static std::shared_ptr<OverridableImpl<T>> Create(const T &v)
			{
				return std::shared_ptr<OverridableImpl<T>>(new OverridableImpl<T>(v));
			}
			static std::shared_ptr<OverridableImpl<T>> Create(const Overridable<T> &parentDefault)
			{
				auto r = std::shared_ptr<OverridableImpl<T>>(new OverridableImpl<T>(parentDefault));
				parentDefault.m_impl->m_children.push_back(r);
				return r;
			}
			friend Overridable<T>;
		private:
			OverridableImpl<T>() : OverridableImpl<T>(T()) {}
			OverridableImpl<T>(const T &v) : m_default(v) {}
			OverridableImpl<T>(const Overridable<T> &parentDefault) : m_parentDefault(parentDefault.m_impl) {}
			void CallChangeCallbacks(const T &oldVal,const T &newVal,bool bApplyToChildren=true)
			{
				for(auto it=m_changeCallbacks.begin();it!=m_changeCallbacks.end();)
				{
					auto &hCb = *it;
					if(hCb.IsValid() == false)
					{
						it = m_changeCallbacks.erase(it);
						continue;
					}
					hCb(std::reference_wrapper<const T>(oldVal),std::reference_wrapper<const T>(newVal));
					++it;
				}
				if(bApplyToChildren == true)
				{
					for(auto it=m_children.begin();it!=m_children.end();)
					{
						auto &wpChild = *it;
						if(wpChild.expired())
						{
							it = m_children.erase(it);
							continue;
						}
						auto ptrChild = wpChild.lock();
						if(ptrChild->m_bOverridden == false)
							ptrChild->CallChangeCallbacks(oldVal,newVal);
						++it;
					}
				}
			}
			T m_default = {};
			T m_overridden = {};
			bool m_bOverridden = false;

			std::weak_ptr<OverridableImpl<T>> m_parentDefault = {};

			std::vector<std::weak_ptr<OverridableImpl<T>>> m_children = {};
			std::vector<CallbackHandle> m_changeCallbacks = {};
		};
	};

	template<typename T>
		class Overridable
	{
	public:
		Overridable<T>() : Overridable<T>(T()) {}
		Overridable<T>(const T &v) : m_impl(_impl::OverridableImpl<T>::Create(v)) {}
		Overridable<T>(const Overridable<T> &parentDefault) : m_impl(_impl::OverridableImpl<T>::Create(parentDefault)) {}
		Overridable<T> &operator=(const T &v)
		{
			*m_impl = v;
			return *this;
		}
		const T &operator*() const {return **m_impl;}
		T &operator*() {return **m_impl;}

		const T &GetValue() const {return m_impl->GetValue();}
		T &GetValue() {return m_impl->GetValue();}

		operator T() const {return static_cast<T>(*m_impl);}

		const T &GetDefaultValue() const {return m_impl->GetDefaultValue();}
		T &GetDefaultValue() {return m_impl->GetDefaultValue();}

		const T &GetValueOr(const T &other) const {return m_impl->GetValueOr(other);}
		const T &GetValueOr(const T &other) {return m_impl->GetValueOr(other);}
		T &GetValueOr(T &other) {return m_impl->GetValueOr(other);}

		const T *operator->() const {return m_impl->operator->();}
		T *operator->() {return m_impl->operator->();}
		bool WasOverriden() const {return m_impl->WasOverriden();}
		void Reset() {m_impl->Reset();}
		bool operator==(const T &v) const {return m_impl->operator==(v);}
		bool operator!=(const T &v) const {return m_impl->operator!=(v);}

		CallbackHandle AddChangeCallback(const std::function<void(std::reference_wrapper<const T>,std::reference_wrapper<const T>)> &f)
		{
			return m_impl->AddChangeCallback(f);
		}
		void InvokeChangeCallbacks(bool bApplyToChildren=true) {m_impl->InvokeChangeCallbacks(bApplyToChildren);}
	private:
		std::shared_ptr<_impl::OverridableImpl<T>> m_impl = nullptr;
		friend _impl::OverridableImpl<T>;
	};
};

#endif
