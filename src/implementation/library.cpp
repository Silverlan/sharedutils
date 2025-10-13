// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include <cstring>

#ifdef __linux
#include <dlfcn.h>
#else
#include <Windows.h>
#endif

module pragma.util;

import :library;

using namespace util;
std::shared_ptr<Library> Library::Get(const std::string &name, std::string *outErr)
{
#ifdef _WIN32
	auto nName = name;
	ufile::remove_extension_from_filename(nName);
	nName += ".dll";
	ustring::replace(nName, "/", "\\");
	auto hModule = GetModuleHandleA(nName.c_str());
	if(hModule == nullptr)
		return nullptr;
#else
	auto hModule = dlopen(name.c_str(), RTLD_LAZY | RTLD_GLOBAL);
	if(hModule == nullptr) {
		if(outErr != nullptr)
			*outErr = dlerror();
		return nullptr;
	}
#endif
	auto lib = std::shared_ptr<Library>(new Library(name, hModule));
	lib->m_freeOnDestruct = false;
	return lib;
}
std::shared_ptr<Library> Library::Load(const std::string &name, const std::vector<std::string> &additionalSearchDirectories, std::string *outErr)
{
#ifdef _WIN32
	std::vector<DLL_DIRECTORY_COOKIE> dirCookies;
	dirCookies.reserve(additionalSearchDirectories.size());
	for(auto &searchPath : additionalSearchDirectories) {
		auto nSearchPath = searchPath;
		ustring::replace(nSearchPath, "/", "\\");
		auto cookie = AddDllDirectory(ustring::string_to_wstring(nSearchPath).c_str());
		if(cookie == nullptr)
			continue;
		dirCookies.push_back(cookie);
	}
	auto dllGuard = ScopeGuard([dirCookies]() {
		for(auto &cookie : dirCookies)
			RemoveDllDirectory(cookie);
	});
	auto nName = name;
	ufile::remove_extension_from_filename(nName);
	nName += ".dll";
	ustring::replace(nName, "/", "\\");
	auto hModule = LoadLibraryEx(nName.c_str(), nullptr, LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR | LOAD_LIBRARY_SEARCH_DEFAULT_DIRS);
	if(hModule == nullptr) {
		if(outErr != nullptr) {
			auto errCode = GetLastError();
			if(errCode != 0) {
				LPTSTR lpErrorText = NULL;
				auto size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, errCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&lpErrorText, 0, NULL);
				*outErr = std::string(lpErrorText, size);
				::LocalFree(lpErrorText);
			}
			else
				*outErr = "Unknown error.";
		}
		return nullptr;
	}
#else
	auto curLibPath = get_env_variable("LD_LIBRARY_PATH");
	if(curLibPath.has_value()) {
		auto newLibPath = *curLibPath + ":" + ustring::implode(additionalSearchDirectories, ":"); //Those are loaded LAST.
		set_env_variable("LD_LIBRARY_PATH", newLibPath);
	}

	util::ScopeGuard sgResetLibPath {[curLibPath = std::move(curLibPath)]() {
		if(curLibPath.has_value())
			set_env_variable("LD_LIBRARY_PATH", *curLibPath);
	}};

	std::string soName = name;
	// Some libraries may have the format *.so.1 or similar
	if(soName.length() >= 3 && soName.substr(soName.length() - 3) != ".so" && soName.find(".so.") == std::string::npos)
		soName += ".so";
	auto hModule = dlopen(soName.c_str(), RTLD_LAZY | RTLD_GLOBAL);
	if(hModule == nullptr) {
		if(outErr != nullptr)
			*outErr = dlerror();
		return nullptr;
	}
#endif
	return std::shared_ptr<Library>(new Library(name, hModule));
}

Library::Library(const std::string &name, LibraryModule hModule) : m_module {hModule}, m_name {name} {}

Library::~Library()
{
	if(m_freeOnDestruct == false)
		return;
#ifdef _WIN32
	FreeLibrary(m_module);
#else
	dlclose(m_module);
#endif
}

void Library::SetDontFreeLibraryOnDestruct() { m_freeOnDestruct = false; }

void *Library::FindSymbolAddress(const std::string &name) const
{
#ifdef _WIN32
	return GetProcAddress(m_module, name.c_str());
#else
	return dlsym(m_module, name.c_str());
#endif
}
