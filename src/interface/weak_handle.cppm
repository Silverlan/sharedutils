// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

export module pragma.util:weak_handle;

export import std.compat;

export {
	namespace pragma::util {
		template<class T>
		class WeakHandle {
		  public:
			using value_type = T;

			WeakHandle() = default;
			WeakHandle(const std::shared_ptr<T> &o);
			WeakHandle(const WeakHandle<T> &) = default;

			const T &operator*() const;
			T &operator*();
			const T *operator->() const;
			T *operator->();

			bool expired() const;
			// Alias for !expired()
			bool valid() const;
			T *get() const;
			void reset();
		  private:
			mutable std::weak_ptr<T> m_ptr = {};
		};
		template<class T>
		WeakHandle<T>::WeakHandle(const std::shared_ptr<T> &o) : m_ptr(o)
		{
		}

		template<class T>
		const T &WeakHandle<T>::operator*() const
		{
			return const_cast<WeakHandle<T> *>(this)->operator*();
		}
		template<class T>
		T &WeakHandle<T>::operator*()
		{
			return *get();
		}
		template<class T>
		const T *WeakHandle<T>::operator->() const
		{
			return const_cast<WeakHandle<T> *>(this)->operator->();
		}
		template<class T>
		T *WeakHandle<T>::operator->()
		{
			return get();
		}

		template<class T>
		bool WeakHandle<T>::expired() const
		{
			return m_ptr.expired();
		}
		template<class T>
		bool WeakHandle<T>::valid() const
		{
			return !m_ptr.expired();
		}
		template<class T>
		T *WeakHandle<T>::get() const
		{
			return m_ptr.lock().get();
		}
		template<class T>
		void WeakHandle<T>::reset()
		{
			m_ptr = {};
		}
	};
}
