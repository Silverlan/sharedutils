// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include "definitions.hpp"
#if defined(_WIN32) || defined(__linux__)

#ifdef _WIN32
#include <Windows.h>
#endif
#endif

export module pragma.util:library;

export import std.compat;

#if defined(_WIN32) || defined(__linux__)
export {
	namespace util {
#ifdef _WIN32
		using LibraryModule = HMODULE;
#else
		using LibraryModule = void *;
#endif
		class DLLSHUTIL Library {
		  public:
			static std::shared_ptr<Library> Load(const std::string &name, const std::vector<std::string> &additionalSearchDirectories = {}, std::string *outErr = nullptr);
			static std::shared_ptr<Library> Get(const std::string &name, std::string *outErr = nullptr);

			void *FindSymbolAddress(const std::string &name) const;
			template<typename TFunc>
			inline TFunc FindSymbolAddress(const std::string &name) const;
			void SetDontFreeLibraryOnDestruct();
			Library(const Library &) = delete;
			Library &operator=(const Library &) = delete;
			~Library();
		  private:
			Library(const std::string &name, LibraryModule hModule);
			LibraryModule m_module = nullptr;
			bool m_freeOnDestruct = true;
			std::string m_name;
		};
		template<typename TFunc>
		TFunc Library::FindSymbolAddress(const std::string &name) const
		{
			return reinterpret_cast<TFunc>(FindSymbolAddress(name));
		}
	};
}

#endif
