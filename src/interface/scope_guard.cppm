// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include "sharedutils/utildefinitions.h"
#include <functional>

export module pragma.util:scope_guard;

export {
	namespace util {
		class DLLSHUTIL ScopeGuard {
		private:
			std::function<void()> f;
		public:
			template<class Callable>
			ScopeGuard(Callable &&undo_func);
			ScopeGuard(ScopeGuard &&other);
			ScopeGuard();
			~ScopeGuard();
			void dismiss() throw();
			void release() throw();
			ScopeGuard(const ScopeGuard &) = delete;
			template<class Callable>
			ScopeGuard &operator=(Callable &&undo_func);
			ScopeGuard &operator=(ScopeGuard &&other);
		};
		template<class Callable>
		util::ScopeGuard &ScopeGuard::operator=(Callable &&undo_func)
		{
			if(f != nullptr)
				f();
			f = std::forward<Callable>(undo_func);
			return *this;
		}

		template<class Callable>
		ScopeGuard::ScopeGuard(Callable &&undo_func) : f(std::forward<Callable>(undo_func))
		{
		}
	};
}
