// SPDX-FileCopyrightText: (c) 2026 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

export module pragma.util:heap;

export import pragma.string;

export {
	namespace pragma::util {
		class HeapGroup;
		class HeapScope;
		class HeapManager;
		class DLLSHUTIL Heap {
		  public:
			~Heap();
			const size_t CalcMemoryUsage() const;
			void SetDefaultForThread(bool threadpool = false) const;
		  private:
			friend HeapGroup;
			friend HeapScope;
			friend HeapManager;
			Heap(void *heap);
			void *m_heap;
		};

		class DLLSHUTIL HeapGroup {
		  public:
			~HeapGroup();
			std::string_view GetName() const;
			const Heap *GetHeap(size_t index) const;
			size_t GetHeapCount() const;
			const size_t CalcMemoryUsage() const;
			void Ensure(size_t heapCount);
		  private:
			friend HeapManager;
			HeapGroup(std::string_view name, size_t heapCount = 0);
			std::vector<std::unique_ptr<Heap>> m_heaps;
			std::string m_name;
		};

		class DLLSHUTIL HeapScope {
		  public:
			HeapScope(const Heap *newHeap);
			~HeapScope();
		  private:
			void *m_oldHeap = nullptr;
		};

		class DLLSHUTIL HeapManager {
		  public:
			HeapManager();
			~HeapManager();

			HeapGroup *CreateHeapGroup(std::string_view name, size_t heapCount = 0);
			HeapGroup *FindHeapGroup(std::string_view name) const;
			const string::StringMap<std::unique_ptr<HeapGroup>> &GetHeapGroups() const;
			void DestroyHeapGroup(std::string_view name);
			void DestroyHeapGroup(const HeapGroup &heap);

			const Heap *CreateHeap(std::string_view name);
			const Heap *FindHeap(std::string_view name) const;
			void DestroyHeap(std::string_view name);
		  private:
			string::StringMap<std::unique_ptr<HeapGroup>> m_heapGroups;
		};
	};
}
