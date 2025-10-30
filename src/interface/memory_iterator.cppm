// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;


export module pragma.util:memory_iterator;

export import std.compat;

export {
	namespace util {
		template<class T>
		class MemoryIteratorBase {
		public:
			using iterator_category = std::forward_iterator_tag;
			using value_type = T;
			using difference_type = value_type;
			using pointer = value_type *;
			using reference = value_type &;

			MemoryIteratorBase(T &value);

			const value_type &operator++();
			const value_type &operator++(int);
			const value_type &operator--();
			const value_type &operator--(int);
			const value_type &operator*() const;

			bool operator==(const MemoryIteratorBase<T> &other) const;
			bool operator!=(const MemoryIteratorBase<T> &other) const;
		private:
			T *m_value = nullptr;
		};

		template<class T>
		class TMemoryIterator {
		public:
			TMemoryIterator(T *t, size_t count);
			MemoryIteratorBase<T> begin() const;
			MemoryIteratorBase<T> end() const;
		private:
			T *m_base = nullptr;
			size_t m_count = 0;
		};
		template<class T>
		MemoryIteratorBase<T>::MemoryIteratorBase(T &value) : m_value {&value}
		{
		}

		template<class T>
		const MemoryIteratorBase<T>::value_type &MemoryIteratorBase<T>::operator++()
		{
			m_value++;
			return *m_value;
		}
		template<class T>
		const MemoryIteratorBase<T>::value_type &MemoryIteratorBase<T>::operator++(int)
		{
			m_value++;
			return *m_value;
		}
		template<class T>
		const MemoryIteratorBase<T>::value_type &MemoryIteratorBase<T>::operator--()
		{
			m_value--;
			return *m_value;
		}
		template<class T>
		const MemoryIteratorBase<T>::value_type &MemoryIteratorBase<T>::operator--(int)
		{
			m_value--;
			return *m_value;
		}
		template<class T>
		const MemoryIteratorBase<T>::value_type &MemoryIteratorBase<T>::operator*() const
		{
			return *m_value;
		}

		template<class T>
		bool MemoryIteratorBase<T>::operator==(const MemoryIteratorBase<T> &other) const
		{
			return m_value == other.m_value;
		}
		template<class T>
		bool MemoryIteratorBase<T>::operator!=(const MemoryIteratorBase<T> &other) const
		{
			return !operator==(other);
		}

		template<class T>
		TMemoryIterator<T>::TMemoryIterator(T *t, size_t count) : m_base {t}, m_count {count}
		{
		}
		template<class T>
		MemoryIteratorBase<T> TMemoryIterator<T>::begin() const
		{
			return MemoryIteratorBase<T> {*m_base};
		}
		template<class T>
		MemoryIteratorBase<T> TMemoryIterator<T>::end() const
		{
			return MemoryIteratorBase<T> {*(m_base + m_count)};
		}
	};
}
