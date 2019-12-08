/* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __UTIL_INDEXED_VECTOR_HPP__
#define __UTIL_INDEXED_VECTOR_HPP__

#include "utildefinitions.h"
#include <vector>
#include <queue>
#include <memory>
#include <cinttypes>

namespace util
{
	template<typename T,class TPtr=std::shared_ptr<T>>
		class IndexedVector
	{
	public:
		using Index = uint32_t;
		using Size = size_t;
		IndexedVector()=default;
		IndexedVector(const IndexedVector&)=default;
		IndexedVector &operator=(const IndexedVector&)=default;
		Index Add(const TPtr &item);
		void Remove(const T &item);
		void Remove(Index index);
		void Reserve(Size size);
		Size GetSize() const;
		T *Get(Index idx);
		const T *Get(Index idx) const;
		void Clear();
	private:
		std::vector<TPtr> m_data = {};
		std::queue<Index> m_freeIndices = {};
	};
};

template<typename T,class TPtr>
	util::IndexedVector<T,TPtr>::Index util::IndexedVector<T,TPtr>::Add(const TPtr &item)
{
	if(m_freeIndices.empty() == false)
	{
		auto index = m_freeIndices.front();
		m_freeIndices.pop();
		m_data.at(index) = item;
		return index;
	}
	m_data.push_back(item);
	return m_data.size() -1;
}

template<typename T,class TPtr>
	void util::IndexedVector<T,TPtr>::Remove(const T &item)
{
	auto it = std::find_if(m_data.begin(),m_data.end(),[&item](const TPtr &itemOther) {
		return itemOther.get() == &item;
	});
	if(it == m_data.end())
		return;
	Remove(static_cast<Index>(it -m_data.begin()));
}

template<typename T,class TPtr>
	void util::IndexedVector<T,TPtr>::Remove(Index index)
{
	if(index >= m_data.size())
		return;
	auto &ptr = m_data.at(index);
	if(ptr == nullptr)
		return;
	m_freeIndices.push(index);
	ptr = nullptr;
}

template<typename T,class TPtr>
	void util::IndexedVector<T,TPtr>::Reserve(Size size)
{
	m_data.reserve(size);
}

template<typename T,class TPtr>
	util::IndexedVector<T,TPtr>::Size util::IndexedVector<T,TPtr>::GetSize() const
{
	return m_data.size();
}

template<typename T,class TPtr>
	T *util::IndexedVector<T,TPtr>::Get(Index idx)
{
	return (idx < GetSize()) ? m_data.at(idx).get() : nullptr;
}

template<typename T,class TPtr>
	const T *util::IndexedVector<T,TPtr>::Get(Index idx) const
{
	return const_cast<IndexedVector<T>*>(this)->Get(idx);
}

template<typename T,class TPtr>
	void util::IndexedVector<T,TPtr>::Clear()
{
	m_data.clear();
	m_freeIndices = {};
}

#endif
