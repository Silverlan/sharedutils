// SPDX-FileCopyrightText: (c) 2026 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#ifdef PRAGMA_WITH_MIMALLOC
#include <mimalloc.h>
#endif

module pragma.util;

import :heap;

#undef PRAGMA_WITH_MIMALLOC

#ifdef PRAGMA_WITH_MIMALLOC
static bool heap_block_visitor(const mi_heap_t *heap, const mi_heap_area_t *area, void *block, size_t block_size, void *arg)
{
	auto *total_size = static_cast<size_t *>(arg);
	*total_size += block_size;
	return true;
}

static size_t get_memory_usage(mi_heap_t *heap)
{
	size_t total_size = 0;
	mi_heap_visit_blocks(heap, false, heap_block_visitor, &total_size);
	return total_size;
}
#endif

pragma::util::Heap::Heap(void *heap) : m_heap {heap} {}
pragma::util::Heap::~Heap()
{
#ifdef PRAGMA_WITH_MIMALLOC
	mi_heap_destroy(static_cast<mi_heap_t *>(m_heap));
#endif
}
const size_t pragma::util::Heap::CalcMemoryUsage() const
{
#ifdef PRAGMA_WITH_MIMALLOC
	return get_memory_usage(static_cast<mi_heap_t *>(m_heap));
#else
	return 0;
#endif
}

void pragma::util::Heap::SetDefaultForThread(bool threadpool) const
{
	//if(threadpool)
	//	mi_thread_set_in_threadpool();
	//mi_heap_set_default(static_cast<mi_heap_t *>(m_heap));
}

pragma::util::HeapGroup::~HeapGroup() {}
void pragma::util::HeapGroup::Ensure(size_t heapCount)
{
	if(m_heaps.size() >= heapCount)
		return;
	auto curSize = m_heaps.size();
	m_heaps.resize(heapCount);
	for(size_t i = curSize; i < heapCount; ++i) {
#ifdef PRAGMA_WITH_MIMALLOC
		auto *miheap = mi_heap_new();
#else
		void *miheap = nullptr;
#endif
		m_heaps[i] = std::unique_ptr<Heap> {new Heap {miheap}};
	}
}
std::string_view pragma::util::HeapGroup::GetName() const { return m_name; }
pragma::util::HeapGroup::HeapGroup(std::string_view name, size_t heapCount) : m_name {name}
{
	m_heaps.resize(heapCount);
	for(auto &heap : m_heaps) {
#ifdef PRAGMA_WITH_MIMALLOC
		auto *miheap = mi_heap_new();
#else
		void *miheap = nullptr;
#endif
		heap = std::unique_ptr<Heap> {new Heap {miheap}};
	}
}
const pragma::util::Heap *pragma::util::HeapGroup::GetHeap(size_t index) const
{
	if(index >= m_heaps.size())
		return nullptr;
	return m_heaps[index].get();
}
size_t pragma::util::HeapGroup::GetHeapCount() const { return m_heaps.size(); }
const size_t pragma::util::HeapGroup::CalcMemoryUsage() const
{
	size_t memUsage = 0;
	for(auto &heap : m_heaps)
		memUsage += heap->CalcMemoryUsage();
	return memUsage;
}

pragma::util::HeapScope::HeapScope(const Heap *newHeap)
{
	if(!newHeap)
		return;
#ifdef PRAGMA_WITH_MIMALLOC
	m_oldHeap = mi_heap_get_default();
#endif
	newHeap->SetDefaultForThread();
}

pragma::util::HeapScope::~HeapScope()
{
	//if(m_oldHeap)
	//	mi_heap_set_default(static_cast<mi_heap_t *>(m_oldHeap));
}

pragma::util::HeapManager::HeapManager() {}
pragma::util::HeapManager::~HeapManager() {}

pragma::util::HeapGroup *pragma::util::HeapManager::CreateHeapGroup(std::string_view name, size_t heapCount)
{
	auto *existingGroup = FindHeapGroup(name);
	if(existingGroup)
		return existingGroup;
	auto heapGroup = std::unique_ptr<HeapGroup> {new HeapGroup {name, heapCount}};
	auto *pheapGroup = heapGroup.get();
	m_heapGroups.insert(std::make_pair(name, std::move(heapGroup)));
	return pheapGroup;
}
pragma::util::HeapGroup *pragma::util::HeapManager::FindHeapGroup(std::string_view name) const
{
	auto it = m_heapGroups.find(name);
	if(it == m_heapGroups.end())
		return nullptr;
	return it->second.get();
}
const pragma::string::StringMap<std::unique_ptr<pragma::util::HeapGroup>> &pragma::util::HeapManager::GetHeapGroups() const { return m_heapGroups; }
void pragma::util::HeapManager::DestroyHeapGroup(std::string_view name)
{
	auto it = m_heapGroups.find(name);
	if(it == m_heapGroups.end())
		return;
	m_heapGroups.erase(it);
}
void pragma::util::HeapManager::DestroyHeapGroup(const HeapGroup &heapGroup) { DestroyHeapGroup(heapGroup.GetName()); }

const pragma::util::Heap *pragma::util::HeapManager::CreateHeap(std::string_view name)
{
	auto *group = CreateHeapGroup(name, 1);
	if(!group)
		return nullptr;
	return group->GetHeap(0);
}
const pragma::util::Heap *pragma::util::HeapManager::FindHeap(std::string_view name) const
{
	auto *group = FindHeapGroup(name);
	if(!group)
		return nullptr;
	return group->GetHeap(0);
}
void pragma::util::HeapManager::DestroyHeap(std::string_view name) { DestroyHeapGroup(name); }
