/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __UTIL_H__
#define __UTIL_H__

#include "utildefinitions.h"
#include <string>
#include <vector>
#include <array>
#include <unordered_map>
#include <memory>
#include <functional>
#include <optional>
#include <charconv>
#include "util_version.h"
#include <mathutil/umath.h>
#ifdef _WIN32
#include <winreg.h>
#else
#include <ios>
#endif
#include <sstream>

namespace std {class thread;};
namespace util
{
	using GUID = std::array<uint8_t,16>;
	DLLSHUTIL std::string get_pretty_bytes(unsigned long long bytes);
	DLLSHUTIL std::string get_pretty_duration(unsigned long long ms,int addSegments=0,bool bNoMs=true);
	DLLSHUTIL std::string get_program_path();
	DLLSHUTIL void set_program_path(const std::string &path);
	DLLSHUTIL std::string get_program_name();
	DLLSHUTIL std::optional<std::string> get_library_file_path(const void *ptrToAnyStaticLibFunc);
	DLLSHUTIL std::optional<std::string> get_path_to_library(const void *ptrToAnyStaticLibFunc);
	DLLSHUTIL unsigned long long get_process_id();
	DLLSHUTIL bool is_process_running(unsigned long long id);
	DLLSHUTIL bool is_process_running(const char *name);
	DLLSHUTIL bool start_process(const char *program,bool bGlobalPath=false);
	DLLSHUTIL bool start_process(const char *program,const std::string &args,bool bGlobalPath=false);
	DLLSHUTIL bool start_process(const char *program,const std::vector<std::string> &args,bool bGlobalPath=false);
#ifdef _WIN32
	DLLSHUTIL bool start_and_wait_for_process(const char *program,unsigned int *exitCode=nullptr,bool bGlobalPath=false,std::string *optOutput=nullptr);
#endif
	DLLSHUTIL std::string get_last_system_error_string();
	DLLSHUTIL bool is_x64_system();
	DLLSHUTIL bool is_x86_system();
	DLLSHUTIL bool is_windows_system();
	DLLSHUTIL bool is_linux_system();
#ifdef _WIN32
	DLLSHUTIL std::unordered_map<std::string,std::string> get_launch_parameters();
#endif
	DLLSHUTIL std::unordered_map<std::string,std::string> get_launch_parameters(int argc,char *argv[]);
	template<class T>
		T to_number(const std::string_view &str);
	template<class T>
		T to_float(const std::string_view &str);
	DLLSHUTIL float to_float(const std::string_view &str);
	template<class T>
		T to_int(const std::string_view &str);
	DLLSHUTIL int to_int(const std::string_view &str);
	inline uint32_t to_uint(const std::string_view &str);
	DLLSHUTIL uint64_t to_uint64(const std::string_view &str);
	DLLSHUTIL std::string round_string(double v,int places=0);
	DLLSHUTIL Version string_to_version(const std::string_view &str);
	DLLSHUTIL Float get_faded_time_factor(Float cur,Float dur,Float fadeIn=0.f,Float fadeOut=0.f);
	DLLSHUTIL Float get_scale_factor(Float val,Float range);
	DLLSHUTIL Float get_scale_factor(Float val,Float min,Float max);
#ifdef _WIN32
	enum class DLLSHUTIL HKey : uint64_t
	{
		ClassesRoot = reinterpret_cast<uint64_t>(HKEY_CLASSES_ROOT),
		CurrentUser = reinterpret_cast<uint64_t>(HKEY_CURRENT_USER),
		LocalMachine = reinterpret_cast<uint64_t>(HKEY_LOCAL_MACHINE),
		Users = reinterpret_cast<uint64_t>(HKEY_USERS),
#if (WINVER >= 0x0400)
		CurrentConfig = reinterpret_cast<uint64_t>(HKEY_CURRENT_CONFIG),
		DynData = reinterpret_cast<uint64_t>(HKEY_DYN_DATA),
		CurrentUserLocalSettings = reinterpret_cast<uint64_t>(HKEY_CURRENT_USER_LOCAL_SETTINGS)
#endif
	};
	DLLSHUTIL bool get_registry_key_value(HKey key,const std::string &path,const std::string &strValueName,std::string &strValue);
	DLLSHUTIL bool get_registry_key_value(HKey key,const std::string &path,const std::string &strValueName,uint64_t &intValue);
	DLLSHUTIL bool get_registry_key_value(HKey key,const std::string &path,const std::string &strValueName,bool &val);
#endif
	DLLSHUTIL bool set_env_variable(const std::string &varName,const std::string &value);
	DLLSHUTIL bool unset_env_variable(const std::string &varName);
	DLLSHUTIL std::optional<std::string> get_env_variable(const std::string &varName);
	DLLSHUTIL std::string get_date_time(const std::string &format="%Y-%m-%d %X");
	template<typename T, typename C>
		constexpr T declvalue(T C::* ptr);
	constexpr uint64_t get_max_unsigned_integer(uint32_t bitCount) {return (1<<bitCount) -1;}
	template<typename T>
		std::string to_hex_string(const T &o);
	template<typename T>
		uint64_t to_hex_number(const T &o);
	DLLSHUTIL bool to_boolean(const std::string_view &str);
	DLLSHUTIL bool get_current_working_directory(std::string &cwd);
	DLLSHUTIL std::string get_current_working_directory();
	DLLSHUTIL bool set_current_working_directory(const std::string &path);
#ifdef _WIN32
	DLLSHUTIL bool start_and_wait_for_command(const char *cmd,const char *cwd=nullptr,unsigned int *exitCode=nullptr,std::string *optOutput=nullptr);
#endif
	DLLSHUTIL void open_url_in_browser(const std::string &url);
	DLLSHUTIL void open_file_in_default_program(const std::string &filePath);
	DLLSHUTIL void open_path_in_explorer(const std::string &path,const std::optional<std::string> &selectFile={});

	DLLSHUTIL std::string guid_to_string(const GUID &guid);
	DLLSHUTIL bool compare_guid(const GUID &guid0,const GUID &guid1);

	using Uuid = std::array<uint64_t,2>;
	DLLSHUTIL Uuid generate_uuid_v4(const std::optional<uint32_t> seed={});
	DLLSHUTIL std::string uuid_to_string(const Uuid &uuid);
	DLLSHUTIL Uuid uuid_string_to_bytes(const std::string &uuid);
	DLLSHUTIL bool is_uuid(const std::string &uuid);
	DLLSHUTIL size_t get_uuid_hash(const Uuid &uuid);

	template<typename T>
		constexpr size_t size_of_container(const T &t)
	{
		return t.size() *sizeof(t[0]);
	}

	enum class ThreadPriority : uint32_t
	{
		Lowest = 0,
		Low,
		BelowNormal,
		Normal,
		AboveNormal,
		High,
		Highest
	};
	DLLSHUTIL void set_thread_priority(std::thread &thread,ThreadPriority priority);
	DLLSHUTIL bool set_thread_name(std::thread &thread,const std::string &name);
	DLLSHUTIL bool set_thread_name(const std::string &name);
	DLLSHUTIL std::optional<std::string> get_thread_name(std::thread &thread);
	DLLSHUTIL std::optional<std::string> get_thread_name();

	DLLSHUTIL void flash_window();
	DLLSHUTIL void minimize_window_to_tray();
	DLLSHUTIL void unhide_window();
#ifdef _WIN32
	DLLSHUTIL HWND get_window_handle();
#endif
	DLLSHUTIL bool shutdown_os();

	DLLSHUTIL void flip_item_sequence(void *sequence,size_t sequenceSize,uint32_t numItems,uint32_t itemStride);
	using MurmurHash3 = std::array<uint8_t,sizeof(uint32_t) *4>;
	DLLSHUTIL MurmurHash3 murmur_hash3(const void *data,int32_t len,uint32_t seed);

	template<typename T>
		using unique_ptr_c = std::unique_ptr<T,std::function<void(T*)>>;
	template<typename T>
		constexpr unique_ptr_c<T> unique_ptr_c_null() {return unique_ptr_c<T>{nullptr,[](T*) {}};}
	template<typename T,typename... TARGS>
		unique_ptr_c<T> make_unique_ptr_c(const std::function<void(T&)> &fOnDelete,TARGS&& ...args)
	{
		return util::unique_ptr_c<T>{new T{std::forward<TARGS>(args)...},[fOnDelete](T *v) {fOnDelete(*v); delete v;}};
	}
	template<typename TFrom,typename TTo>
		static std::unique_ptr<TTo> static_unique_pointer_cast(std::unique_ptr<TFrom> &&p)
	{
		return std::unique_ptr<TTo>{static_cast<TTo*>(p.release())};
	}
	template<typename TFrom,typename TTo>
		static std::unique_ptr<TTo> dynamic_unique_pointer_cast(std::unique_ptr<TFrom> &&p)
	{
		auto *ptr = dynamic_cast<TTo*>(p.get());
		if(!ptr)
		{
			auto tmp = std::move(p);
			return nullptr;
		}
		return std::unique_ptr<TTo>{ptr};
	}

	template<typename T>
		uint64_t get_size_in_bytes(const T &container);

	// See https://stackoverflow.com/a/28796458/2482983
	template<typename Test, template<typename...> class Ref>
		struct is_specialization : std::false_type {};
	template<template<typename...> class Ref, typename... Args>
		struct is_specialization<Ref<Args...>, Ref>: std::true_type {};

	template<typename T, std::size_t N>
		constexpr bool is_c_string_p(T(&)[N]) 
	{ 
		return std::is_same_v<const char, T>;
	}
	template<typename T>
		constexpr bool is_c_string_p(T &&) 
	{ 
		return std::is_same_v<const char *, T>;
	}

	template<typename T>
		struct is_c_string
		: public std::disjunction<std::is_same<char *, typename std::decay_t<T>>,std::is_same<const char *, typename std::decay_t<T>>> {};

	template<typename T>
		struct is_string
		: public std::disjunction<std::is_same<char *, typename std::decay_t<T>>,std::is_same<const char *, typename std::decay_t<T>>,std::is_same<std::string, typename std::decay_t<T>>> {};

	template <typename T>
		using base_type = typename std::remove_cv_t<std::remove_pointer_t<std::remove_reference_t<T>>>;
}

uint32_t util::to_uint(const std::string_view &str)
{
	uint32_t result = 0;
	auto res = std::from_chars(str.data(),str.data() +str.size(),result);
	return result;
}

template<class T>
	T util::to_number(const std::string_view &str)
{
	T result = 0;
	auto res = std::from_chars(str.data(),str.data() +str.size(),result);
	return result;
}

template<class T>
	T util::to_float(const std::string_view &str)
{
	return to_number<T>(str);
}

template<class T>
	T util::to_int(const std::string_view &str)
{
	return to_number<T>(str);
}

template<typename T, typename C>
	constexpr T util::declvalue(T C::* ptr)
{
	return C{}.*ptr;
}

template<typename T>
	std::string util::to_hex_string(const T &o)
{
	std::stringstream stream;
	stream<<std::hex<<o;
	return stream.str();
}

template<typename T>
	uint64_t util::to_hex_number(const T &o)
{
	std::stringstream stream;
	stream<<o;
	auto hex = 0ull;
	stream>>std::hex>>hex;
	return hex;
}

template<typename T>
	uint64_t util::get_size_in_bytes(const T &container)
{
	return container.size() *sizeof(container.front());
}

#endif
