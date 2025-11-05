// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include "definitions.hpp"

export module pragma.util:virtual_shared_from_this;

export import std.compat;

// Source: http://stackoverflow.com/a/16083526/2482983
#pragma warning(push)
#pragma warning(disable : 4251)
namespace util {
	namespace impl {
		class DLLSHUTIL MultipleInheritableEnableSharedFromThis : public std::enable_shared_from_this<MultipleInheritableEnableSharedFromThis> {
		  public:
			virtual ~MultipleInheritableEnableSharedFromThis() = default;
		};
	};
	export {
		template<class T>
		class inheritable_enable_shared_from_this : virtual public impl::MultipleInheritableEnableSharedFromThis {
		  public:
			std::shared_ptr<T> shared_from_this() { return std::dynamic_pointer_cast<T>(MultipleInheritableEnableSharedFromThis::shared_from_this()); }
			/* Utility method to easily downcast.
			* Useful when a child doesn't inherit directly from enable_shared_from_this
			* but wants to use the feature.
			*/
			template<class Down>
			std::shared_ptr<Down> downcasted_shared_from_this()
			{
				return std::dynamic_pointer_cast<Down>(MultipleInheritableEnableSharedFromThis::shared_from_this());
			}
		};
	}
};
#pragma warning(pop)
