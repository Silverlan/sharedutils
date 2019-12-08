/* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __UTIL_MEMORY_ITERATOR_HPP__
#define __UTIL_MEMORY_ITERATOR_HPP__

namespace util
{
	template<class T>
		class MemoryIteratorBase
	{
	public:
		using iterator_category = std::forward_iterator_tag;
		using value_type = T;
		using difference_type = value_type;
		using pointer = value_type*;
		using reference = value_type&;

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
		class TMemoryIterator
	{
	public:
		TMemoryIterator(T *t,size_t count);
		MemoryIteratorBase<T> begin() const;
		MemoryIteratorBase<T> end() const;
	private:
		T *m_base = nullptr;
		size_t m_count = 0;
	};
};

template<class T>
	util::MemoryIteratorBase<T>::MemoryIteratorBase(T &value)
	: m_value{&value}
{}

template<class T>
	const util::MemoryIteratorBase<T>::value_type &util::MemoryIteratorBase<T>::operator++()
{
	m_value++;
	return *m_value;
}
template<class T>
	const util::MemoryIteratorBase<T>::value_type &util::MemoryIteratorBase<T>::operator++(int)
{
	m_value++;
	return *m_value;
}
template<class T>
	const util::MemoryIteratorBase<T>::value_type &util::MemoryIteratorBase<T>::operator--()
{
	m_value--;
	return *m_value;
}
template<class T>
	const util::MemoryIteratorBase<T>::value_type &util::MemoryIteratorBase<T>::operator--(int)
{
	m_value--;
	return *m_value;
}
template<class T>
	const util::MemoryIteratorBase<T>::value_type &util::MemoryIteratorBase<T>::operator*() const
{
	return *m_value;
}

template<class T>
	bool util::MemoryIteratorBase<T>::operator==(const MemoryIteratorBase<T> &other) const
{
	return m_value == other.m_value;
}
template<class T>
	bool util::MemoryIteratorBase<T>::operator!=(const MemoryIteratorBase<T> &other) const
{
	return !operator==(other);
}

template<class T>
	util::TMemoryIterator<T>::TMemoryIterator(T *t,size_t count)
	: m_base{t},m_count{count}
{}
template<class T>
	util::MemoryIteratorBase<T> util::TMemoryIterator<T>::begin() const
{
	return MemoryIteratorBase<T>{*m_base};
}
template<class T>
	util::MemoryIteratorBase<T> util::TMemoryIterator<T>::end() const
{
	return MemoryIteratorBase<T>{*(m_base +m_count)};
}

#endif
