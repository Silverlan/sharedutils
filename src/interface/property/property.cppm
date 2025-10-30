// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include "sharedutils/utildefinitions.h"

export module pragma.util:property;

export import :function_callback;
export import pragma.math;

export {
	#pragma warning(push)
	#pragma warning(disable : 4251)
	namespace util {
		class DLLSHUTIL BaseProperty {
		public:
			void AddCallback(const CallbackHandle &hCallback);
			virtual ~BaseProperty() = default;
		protected:
			BaseProperty() = default;
			std::vector<CallbackHandle> m_callbacks;
		};

		enum class SimplePropertyFlags : uint8_t {
			None = 0u,
			Active = 1u,
			Locked = Active << 1u,
			LockedLink = Locked << 1u,             // If this flag isn't set, the property can still be changed by its link even if the Locked flag is set
			TemporarilyUnlocked = LockedLink << 1u // If this flag is set, the property will be unlocked for the next change (and only the next change) if it is locked
		};
		template<class TProperty, class T>
		class SimpleProperty : public BaseProperty, public std::enable_shared_from_this<SimpleProperty<TProperty, T>> {
		public:
			static std::shared_ptr<TProperty> Create(const T &value = {});
			SimpleProperty() = default;
			SimpleProperty(const T &init);
			SimpleProperty(const SimpleProperty &prop);
			virtual ~SimpleProperty() override;
			SimpleProperty &operator=(const T &val);
			SimpleProperty &operator=(const SimpleProperty &other);
			bool operator==(const T &val) const;
			bool operator==(const SimpleProperty &prop) const;
			bool operator!=(const T &val) const;
			bool operator!=(const SimpleProperty &prop) const;
			operator const T &() const;
			operator T &();
			T &operator*();
			const T &operator*() const;
			T *operator->();
			const T *operator->() const;
			const T &GetValue() const;
			T &GetValue();
			CallbackHandle AddCallback(const std::function<void(std::reference_wrapper<const T>, std::reference_wrapper<const T>)> &callback);
			CallbackHandle AddModifier(const std::function<void(std::reference_wrapper<T>)> &fModifier);
			void InvokeCallbacks();

			void SetLocked(bool bLocked, bool bLinkLocked = true);
			bool IsLocked() const;

			template<class TPropertyOther, class TOther>
			void Link(SimpleProperty<TPropertyOther, TOther> &other, const std::function<T(const TOther &)> &converter);
			template<class TPropertyOther, class TOther>
			void Link(SimpleProperty<TPropertyOther, TOther> &other);
			void Unlink();
		protected:
			virtual void ApplyValue(const T &newValue);
			void OnChanged(const T &oldValue, const T &newValue);
			T m_value = {};
			SimplePropertyFlags m_flags = SimplePropertyFlags::None;
			std::vector<CallbackHandle> m_modifiers;
			CallbackHandle m_linkedCallback = {};
		};

		class DLLSHUTIL BoolProperty : public SimpleProperty<BoolProperty, bool> {
		public:
			BoolProperty();
			BoolProperty(const bool &init);
			BoolProperty(const BoolProperty &prop);

			BoolProperty &operator=(const bool &val);
			BoolProperty &operator=(const BoolProperty &other);

			virtual void ApplyValue(const bool &newValue) override;
		};
		using PBoolProperty = std::shared_ptr<BoolProperty>;

		using namespace umath::scoped_enum::bitwise;
	};
	namespace umath::scoped_enum::bitwise {
		template<>
		struct enable_bitwise_operators<util::SimplePropertyFlags> : std::true_type {};
	}
	#pragma warning(pop)

	namespace util {
		template<class TProperty, class T>
		std::shared_ptr<TProperty> SimpleProperty<TProperty, T>::Create(const T &value)
		{
			return std::make_shared<TProperty>(value);
		};

		template<class TProperty, class T>
		SimpleProperty<TProperty, T>::SimpleProperty(const T &init) : BaseProperty(), m_value(init)
		{
		}

		template<class TProperty, class T>
		SimpleProperty<TProperty, T>::SimpleProperty(const SimpleProperty<TProperty, T> &prop) : BaseProperty(), m_value(prop.m_value)
		{
		}

		template<class TProperty, class T>
		void SimpleProperty<TProperty, T>::SetLocked(bool bLocked, bool bLinkLocked)
		{
			if(bLocked) {
				m_flags |= SimplePropertyFlags::Locked;
				if(bLinkLocked)
					m_flags |= SimplePropertyFlags::LockedLink;
			}
			else
				m_flags &= ~(SimplePropertyFlags::Locked | SimplePropertyFlags::LockedLink);
		}
		template<class TProperty, class T>
		bool SimpleProperty<TProperty, T>::IsLocked() const
		{
			return (m_flags & SimplePropertyFlags::Locked) != SimplePropertyFlags::None;
		}

		template<class TProperty, class T>
		SimpleProperty<TProperty, T>::~SimpleProperty()
		{
			Unlink();
		}

		template<class TProperty, class T>
		template<class TPropertyOther, class TOther>
		void SimpleProperty<TProperty, T>::Link(SimpleProperty<TPropertyOther, TOther> &other, const std::function<T(const TOther &)> &converter)
		{
			Unlink();
			if((m_flags & SimplePropertyFlags::LockedLink) == SimplePropertyFlags::None)
				m_flags |= SimplePropertyFlags::TemporarilyUnlocked;
			*this = converter(*other);
			m_linkedCallback = other.AddCallback([this, converter](std::reference_wrapper<const TOther>, std::reference_wrapper<const TOther> newVal) {
				if((m_flags & SimplePropertyFlags::Active) != SimplePropertyFlags::None)
					return; // Prevent recursion
				m_flags |= SimplePropertyFlags::Active;
				if((m_flags & SimplePropertyFlags::LockedLink) == SimplePropertyFlags::None)
					m_flags |= SimplePropertyFlags::TemporarilyUnlocked;
				*this = converter(newVal.get());
				m_flags &= ~SimplePropertyFlags::Active;
			});
		}

		template<class TProperty, class T>
		template<class TPropertyOther, class TOther>
		void SimpleProperty<TProperty, T>::Link(SimpleProperty<TPropertyOther, TOther> &other)
		{
			Link<TPropertyOther, TOther>(other, [](const TOther &v) -> T { return static_cast<T>(v); });
		}

		template<class TProperty, class T>
		void SimpleProperty<TProperty, T>::Unlink()
		{
			if(m_linkedCallback.IsValid())
				m_linkedCallback.Remove();
		}

		template<class TProperty, class T>
		void SimpleProperty<TProperty, T>::ApplyValue(const T &newValue)
		{
			m_value = newValue;
		}

		template<class TProperty, class T>
		SimpleProperty<TProperty, T> &SimpleProperty<TProperty, T>::operator=(const T &val)
		{
			if(IsLocked() && (m_flags & SimplePropertyFlags::TemporarilyUnlocked) == SimplePropertyFlags::None)
				return *this;
			m_flags &= ~SimplePropertyFlags::TemporarilyUnlocked;
			auto newVal = val;
			for(auto it = m_modifiers.begin(); it != m_modifiers.end();) {
				auto &cbMod = *it;
				if(cbMod.IsValid() == false) {
					it = m_modifiers.erase(it);
					continue;
				}
				if constexpr(!std::is_enum_v<T>)
					cbMod.Call<void, std::reference_wrapper<T>>(std::reference_wrapper<T>(newVal));
				else {
					// Enum modifiers can be defined with either their actual type or their underlying type
					if(dynamic_cast<Callback<void, std::reference_wrapper<T>> *>(cbMod.get()))
						cbMod.Call<void, std::reference_wrapper<T>>(std::reference_wrapper<T>(newVal));
					else {
						using TUnderlying = std::underlying_type_t<T>;
						cbMod.Call<void, std::reference_wrapper<TUnderlying>>(std::reference_wrapper<TUnderlying>(reinterpret_cast<TUnderlying &>(newVal)));
					}
				}
				++it;
			}
			if constexpr(std::equality_comparable<T>) {
				if(m_value == newVal)
					return *this;
			}
			auto old = m_value;
			ApplyValue(newVal);
			OnChanged(old, m_value);
			return *this;
		}

		template<class TProperty, class T>
		void SimpleProperty<TProperty, T>::InvokeCallbacks()
		{
			OnChanged(m_value, m_value);
		}

		template<class TProperty, class T>
		SimpleProperty<TProperty, T> &SimpleProperty<TProperty, T>::operator=(const SimpleProperty<TProperty, T> &other)
		{
			return operator=(other.GetValue());
		}

		template<class TProperty, class T>
		bool SimpleProperty<TProperty, T>::operator==(const T &val) const
		{
			if constexpr(std::equality_comparable<T>)
				return (GetValue() == val) ? true : false;
			return false;
		}

		template<class TProperty, class T>
		bool SimpleProperty<TProperty, T>::operator==(const SimpleProperty<TProperty, T> &prop) const
		{
			if constexpr(std::equality_comparable<T>)
				return (*this == prop.GetValue());
			return false;
		}

		template<class TProperty, class T>
		bool SimpleProperty<TProperty, T>::operator!=(const T &val) const
		{
			return (*this == val) ? false : true;
		}

		template<class TProperty, class T>
		bool SimpleProperty<TProperty, T>::operator!=(const SimpleProperty<TProperty, T> &prop) const
		{
			return (*this != prop.GetValue());
		}

		template<class TProperty, class T>
		SimpleProperty<TProperty, T>::operator const T &() const
		{
			return GetValue();
		}

		template<class TProperty, class T>
		SimpleProperty<TProperty, T>::operator T &()
		{
			return GetValue();
		}

		template<class TProperty, class T>
		T &SimpleProperty<TProperty, T>::operator*()
		{
			return m_value;
		}
		template<class TProperty, class T>
		const T &SimpleProperty<TProperty, T>::operator*() const
		{
			return m_value;
		}

		template<class TProperty, class T>
		T *SimpleProperty<TProperty, T>::operator->()
		{
			return &m_value;
		}
		template<class TProperty, class T>
		const T *SimpleProperty<TProperty, T>::operator->() const
		{
			return &m_value;
		}

		template<class TProperty, class T>
		const T &SimpleProperty<TProperty, T>::GetValue() const
		{
			return m_value;
		}

		template<class TProperty, class T>
		T &SimpleProperty<TProperty, T>::GetValue()
		{
			return m_value;
		}

		template<class TProperty, class T>
		void SimpleProperty<TProperty, T>::OnChanged(const T &oldValue, const T &newValue)
		{
			for(auto it = m_callbacks.begin(); it != m_callbacks.end();) {
				auto &cb = *it;
				if(cb.IsValid() == false)
					it = m_callbacks.erase(it);
				else {
					if constexpr(!std::is_enum_v<T>)
						cb(std::ref(oldValue), std::ref(newValue));
					else {
						// Enum callbacks can be defined with either their actual type or their underlying type
						if(dynamic_cast<Callback<void, std::reference_wrapper<const T>, std::reference_wrapper<const T>> *>(cb.get()))
							cb(std::ref(oldValue), std::ref(newValue));
						else {
							using TUnderlying = std::underlying_type_t<const T>;
							cb(std::ref(reinterpret_cast<const TUnderlying &>(oldValue)), std::ref(reinterpret_cast<const TUnderlying &>(newValue)));
						}
					}
					++it;
				}
			}
		}

		template<class TProperty, class T>
		CallbackHandle SimpleProperty<TProperty, T>::AddCallback(const std::function<void(std::reference_wrapper<const T>, std::reference_wrapper<const T>)> &callback)
		{
			auto hCb = FunctionCallback<void, std::reference_wrapper<const T>, std::reference_wrapper<const T>>::Create(callback);
			BaseProperty::AddCallback(hCb);
			return hCb;
		}

		template<class TProperty, class T>
		CallbackHandle SimpleProperty<TProperty, T>::AddModifier(const std::function<void(std::reference_wrapper<T>)> &fModifier)
		{
			auto cb = FunctionCallback<void, std::reference_wrapper<T>>::Create(fModifier);
			m_modifiers.push_back(cb);
			return cb;
		}
	}
}
