// SPDX-FileCopyrightText: (c) 2025 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

export module pragma.util:shared_ptr;

import std.compat;

export {
	// Simple wrappers around std::shared_ptr and std::weak_ptr with convenience utility methods
	namespace util {
		template<typename T>
		struct SharedPtrHandle;
		template<typename T>
		struct WeakPtrHandle {
			using SHARED_TYPE = SharedPtrHandle<T>;
			WeakPtrHandle()=default;
			WeakPtrHandle(const SharedPtrHandle<T> &ptr);
			SharedPtrHandle<T> lock() { return SharedPtrHandle<T>{m_ptr.lock()}; }
			bool IsValid() const {
				return !m_ptr.expired();
			}
			operator bool() const { return IsValid(); }
			T *Get() { return m_ptr.lock().get(); }
			const T *Get() const { return const_cast<WeakPtrHandle<T>*>(this)->Get(); }

			T *get() { return Get(); }
			const T *get() const { return Get(); }

			bool operator==(std::nullptr_t) const { return !IsValid(); }
			bool operator==(const WeakPtrHandle<T> &other) const { return m_ptr == other.m_ptr; }
			bool operator==(const T *other) const {
				if (!IsValid())
					return other == nullptr;
				return m_ptr.get() == other;
			}
			bool operator!=(std::nullptr_t) const { return !operator==(nullptr); }
			bool operator!=(const WeakPtrHandle<T> &other) const { return !operator==(other); }
			bool operator!=(const T *other) const { return !operator==(other); }

			const std::weak_ptr<T> &GetPtr() const { return m_ptr; }
		private:
			std::weak_ptr<T> m_ptr;
		};

		template<typename T>
		struct SharedPtrHandle {
			using WEAK_TYPE = WeakPtrHandle<T>;
			SharedPtrHandle()=default;
			SharedPtrHandle(std::nullptr_t) : m_ptr{nullptr} {}
			SharedPtrHandle(const std::shared_ptr<T> &ptr) : m_ptr{ptr} {}
			bool IsValid() const {
				return (m_ptr != nullptr);
			}
			operator bool() const { return IsValid(); }
			T *operator->() { return m_ptr.get(); }
			const T *operator->() const { return const_cast<SharedPtrHandle<T>*>(this)->operator->(); }
			T &operator*() { return *m_ptr; }
			const T &operator*() const { return const_cast<SharedPtrHandle<T>*>(this)->operator*(); }
			T *Get() { return m_ptr.get(); }
			const T *Get() const { return const_cast<SharedPtrHandle<T>*>(this)->Get(); }

			T *get() { return Get(); }
			const T *get() const { return Get(); }

			size_t use_count() const { return m_ptr.use_count(); }

			bool operator==(std::nullptr_t) const { return !IsValid(); }
			bool operator==(const SharedPtrHandle<T> &other) const { return m_ptr == other.m_ptr; }
			bool operator==(const T *other) const {
				if (!IsValid())
					return other == nullptr;
				return m_ptr.get() == other;
			}
			bool operator!=(std::nullptr_t) const { return !operator==(nullptr); }
			bool operator!=(const SharedPtrHandle<T> &other) const { return !operator==(other); }
			bool operator!=(const T *other) const { return !operator==(other); }

			SharedPtrHandle<T> &operator=(std::nullptr_t) {
				m_ptr = nullptr;
				return *this;
			}

			const std::shared_ptr<T> &GetPtr() const { return m_ptr; }
		private:
			std::shared_ptr<T> m_ptr;
		};

		template<typename T>
		WeakPtrHandle<T>::WeakPtrHandle(const SharedPtrHandle<T> &ptr) : m_ptr{ptr.GetPtr()} {}
	}
}
