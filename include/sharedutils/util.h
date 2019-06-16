/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __UTIL_H__
#define __UTIL_H__

#include "utildefinitions.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>
#include "util_version.h"
#include <mathutil/umath.h>
#ifdef _WIN32
#include <winreg.h>
#else
#include <ios>
#include <sstream>
#endif

namespace std {class thread;};
namespace util
{
	DLLSHUTIL std::string get_pretty_bytes(unsigned long long bytes);
	DLLSHUTIL std::string get_pretty_duration(unsigned long long ms,int addSegments=0,bool bNoMs=true);
	DLLSHUTIL std::string get_program_path();
	DLLSHUTIL std::string get_program_name();
	DLLSHUTIL unsigned long long get_process_id();
	DLLSHUTIL bool is_process_running(unsigned long long id);
	DLLSHUTIL bool is_process_running(const char *name);
	DLLSHUTIL bool start_process(const char *program,bool bGlobalPath=false);
	DLLSHUTIL bool start_process(const char *program,const std::string &args,bool bGlobalPath=false);
	DLLSHUTIL bool start_process(const char *program,const std::vector<std::string> &args,bool bGlobalPath=false);
#ifdef _WIN32
	DLLSHUTIL bool start_and_wait_for_process(const char *program,unsigned int *exitCode=nullptr,bool bGlobalPath=false);
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
		T to_float(const char *str);
	template<class T>
		T to_float(const std::string &str);
	DLLSHUTIL float to_float(const char *str);
	DLLSHUTIL float to_float(const std::string &str);
	template<class T>
		T to_int(const char *str);
	template<class T>
		T to_int(const std::string &str);
	DLLSHUTIL int to_int(const char *str);
	DLLSHUTIL int to_int(const std::string &str);
	inline uint32_t to_uint(const std::string &str);
	DLLSHUTIL std::string round_string(double v,int places=0);
	DLLSHUTIL Version string_to_version(const std::string &str);
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
	DLLSHUTIL std::string get_date_time(const std::string &format="%Y-%m-%d %X");
	template<typename T, typename C>
		constexpr T declvalue(T C::* ptr);
	constexpr uint64_t get_max_unsigned_integer(uint32_t bitCount) {return (1<<bitCount) -1;}
	template<typename T>
		std::string to_hex_string(const T &o);
	template<typename T>
		uint64_t to_hex_number(const T &o);
	DLLSHUTIL bool to_boolean(const std::string &str);
	DLLSHUTIL bool get_current_working_directory(std::string &cwd);
	DLLSHUTIL std::string get_current_working_directory();
	DLLSHUTIL bool set_current_working_directory(const std::string &path);
#ifdef _WIN32
	DLLSHUTIL bool start_and_wait_for_command(const char *cmd,const char *cwd=nullptr,unsigned int *exitCode=nullptr);
#endif
	DLLSHUTIL void open_url_in_browser(const std::string &url);

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
}

uint32_t util::to_uint(const std::string &str)
{
	return std::stoul(str);
}

template<class T>
	T util::to_float(const char *str) {return static_cast<T>(atof(str));}
template<class T>
	T util::to_float(const std::string &str) {return to_float(str.c_str());}

template<class T>
	T util::to_int(const char *str) {return static_cast<T>(atoi(str));}
template<class T>
	T util::to_int(const std::string &str) {return to_int(str.c_str());}

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

#endif