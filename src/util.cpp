/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "sharedutils/util.h"
#include <mathutil/umath.h>
#include "sharedutils/util_string.h"
#include "sharedutils/util_file.h"
#include "sharedutils/uuid.h"
#include "sharedutils/scope_guard.h"
#include "sharedutils/util_hash.hpp"
#include "sharedutils/util_string_hash.hpp"
#include "sharedutils/util_log.hpp"
#include "sharedutils/util_path.hpp"
#include <sstream>
#include <iomanip>
#include <cstring>
#include <random>
#ifdef __linux__
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <signal.h>
#include <glob.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdio.h>
#include <dlfcn.h>
#include <unistd.h>
#include <linux/reboot.h>
#include <sys/reboot.h>
#include <sys/prctl.h>
#include <sys/wait.h>
#include <spawn.h>
#include <fstream>
#else
#include "Shlwapi.h"
#include <vector>
#include <Windows.h>
#include <TlHelp32.h>
#include <comutil.h>
#include <direct.h>
#include <Shlobj_core.h>
#endif
#include <thread>

#ifdef _WIN32
std::string util::LOG_NL = "\r\n";
#else
std::string util::LOG_NL = "\n";
#endif

std::string util::CommandInfo::GetArguments() const
{
	std::string argsStr;
	for(const auto &arg : args) {
		if(!argsStr.empty())
			argsStr += " ";
		argsStr += arg;
	}
	return argsStr;
}

std::string util::get_pretty_bytes(unsigned long long bytes)
{
	auto sz = static_cast<double>(bytes);
	const char *suffixes[] = {"Byte", "KiB", "MiB", "GiB", "TiB", "PiB", "EiB", "ZiB", "YiB"};
	char i = 0;
	while(sz >= 1024.0 && i < 8) {
		sz /= 1024.0;
		i++;
	}
	std::stringstream ss;
	ss << util::round_string(sz, 2) << " " << suffixes[i];
	return ss.str();
}

std::string util::get_pretty_duration(unsigned long long ms, int addSegments, bool bNoMs)
{
	if(addSegments == -1)
		addSegments = 100;
	std::stringstream ss;
	if(ms >= 1000 || bNoMs == true) {
		unsigned long long seconds = ms / 1000;
		ms %= 1000;
		if(bNoMs == true && ms > 0)
			seconds++;
		if(seconds >= 60) {
			unsigned long long minutes = seconds / 60;
			seconds %= 60;
			if(minutes >= 60) {
				unsigned long long hours = minutes / 60;
				minutes %= 60;
				if(hours >= 24) {
					unsigned long long days = hours / 24;
					hours %= 60;
					if(days > 7) {
						unsigned long long weeks = days / 7;
						days %= 24;
						ss << weeks << "wk";
						if(addSegments-- <= 0)
							return ss.str();
						ss << " ";
					}
					ss << days << "d";
					if(addSegments-- <= 0)
						return ss.str();
					ss << " ";
				}
				ss << hours << "h";
				if(addSegments-- <= 0)
					return ss.str();
				ss << " ";
			}
			ss << minutes << "m";
			if(addSegments-- <= 0)
				return ss.str();
			ss << " ";
		}
		ss << seconds << "s";
		if(bNoMs == true || addSegments-- <= 0)
			return ss.str();
		ss << " ";
	}
	ss << ms << "ms";
	return ss.str();
}

static std::string program_name(bool bPost = false)
{
	std::string programPath = "";
#ifdef __linux__
	pid_t pid = getpid();
	char buf[20] = {0};
	sprintf(buf, "%d", pid);
	std::string _link = "/proc/";
	_link.append(buf);
	_link.append("/exe");
	char proc[512];
	int ch = readlink(_link.c_str(), proc, 512);
	if(ch != -1) {
		proc[ch] = 0;
		programPath = proc;
		std::string::size_type t = programPath.find_last_of("/");
		programPath = (bPost == false) ? programPath.substr(0, t) : programPath.substr(t + 1, programPath.length());
	}
#else
	char path[MAX_PATH + 1];
	GetModuleFileName(NULL, path, MAX_PATH + 1);

	programPath = path;
	auto br = programPath.rfind("\\");
	programPath = (bPost == false) ? programPath.substr(0, br) : programPath.substr(br + 1, programPath.length());
#endif
	return programPath;
}

static std::string g_programPath = "";
std::string util::get_program_path()
{
	if(g_programPath.empty())
		g_programPath = program_name();
	return g_programPath;
}

void util::set_program_path(const std::string &path) { g_programPath = path; }

std::string util::get_program_name()
{
	static std::string programName = "";
	if(programName.empty())
		programName = program_name(true);
	return programName;
}

std::optional<std::string> util::get_library_file_path(const void *ptrToAnyStaticLibFunc)
{
	// https://stackoverflow.com/a/6924332/2482983
#ifdef _WIN32
	char path[MAX_PATH];
	HMODULE hm = NULL;

	if(GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, (LPCSTR)ptrToAnyStaticLibFunc, &hm) == 0)
		return {};
	if(GetModuleFileName(hm, path, sizeof(path)) == 0)
		return {};
	return path;
#else
	Dl_info dlInfo;
	dladdr(const_cast<void *>(ptrToAnyStaticLibFunc), &dlInfo);
	if(dlInfo.dli_sname == NULL || dlInfo.dli_saddr == NULL)
		return {};
	return dlInfo.dli_fname;
#endif
}

std::optional<std::string> util::get_path_to_library(const void *ptrToAnyStaticLibFunc)
{
	auto path = get_library_file_path(ptrToAnyStaticLibFunc);
	if(path.has_value() == false)
		return {};
	return ufile::get_path_from_filename(*path);
}

unsigned long long util::get_process_id()
{
#ifdef _WIN32
	return GetCurrentProcessId();
#else
	return getpid();
#endif
}

bool util::is_process_running(unsigned long long id)
{
#ifdef _WIN32
	auto process = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, static_cast<DWORD>(id));
	if(process == nullptr)
		return false;
	unsigned long exitCode = 0;
	auto r = GetExitCodeProcess(process, &exitCode);
	if(r == false || exitCode != STILL_ACTIVE)
		return false;
	return true;
#else
	return (kill(id, 0) == 0) ? true : false;
#endif
}

#ifdef __linux__
static pid_t find_process_id(const char *process_name)
{
	pid_t pid = -1;
	glob_t pglob;
	char *procname, *readbuf;
	int buflen = strlen(process_name) + 2;
	unsigned i;

	/* Get a list of all comm files. man 5 proc */
	if(glob("/proc/*/comm", 0, NULL, &pglob) != 0)
		return pid;

	/* The comm files include trailing newlines, so... */
	procname = (char *)malloc(buflen);
	strcpy(procname, process_name);
	procname[buflen - 2] = '\n';
	procname[buflen - 1] = 0;

	/* readbuff will hold the contents of the comm files. */
	readbuf = (char *)malloc(buflen);

	for(i = 0; i < pglob.gl_pathc; ++i) {
		FILE *comm;
		char *ret;

		/* Read the contents of the file. */
		if((comm = fopen(pglob.gl_pathv[i], "r")) == NULL)
			continue;
		ret = fgets(readbuf, buflen, comm);
		fclose(comm);
		if(ret == NULL)
			continue;

		/*
		If comm matches our process name, extract the process ID from the
		path, convert it to a pid_t, and return it.
		*/
		if(strcmp(readbuf, procname) == 0) {
			pid = (pid_t)atoi(pglob.gl_pathv[i] + strlen("/proc/"));
			break;
		}
	}

	/* Clean up. */
	free(procname);
	free(readbuf);
	globfree(&pglob);
	return pid;
}
#endif

bool util::is_process_running(const char *name)
{
#ifdef _WIN32
	HANDLE SnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if(SnapShot == INVALID_HANDLE_VALUE)
		return false;
	PROCESSENTRY32 procEntry;
	procEntry.dwSize = sizeof(PROCESSENTRY32);
	if(!Process32First(SnapShot, &procEntry))
		return false;
	do {
		if(strcmp(procEntry.szExeFile, name) == 0)
			return true;
	} while(Process32Next(SnapShot, &procEntry));
	return false;
#else
	auto id = find_process_id(name);
	if(id == -1)
		return false;
	return is_process_running(id);
#endif
}

bool util::get_current_working_directory(std::string &cwd)
{
	char cCurrentPath[FILENAME_MAX];
#ifdef __linux__
	if(!getcwd(cCurrentPath, sizeof(cCurrentPath)))
		return false;
#else
	if(!_getcwd(cCurrentPath, sizeof(cCurrentPath)))
		return false;
#endif
	cwd = cCurrentPath;
	return true;
}
std::string util::get_current_working_directory()
{
	std::string r;
	get_current_working_directory(r);
	return r;
}
bool util::set_current_working_directory(const std::string &path)
{
#ifdef __linux__
	if(!chdir(path.c_str()))
		return false;
#else
	if(_chdir(path.c_str()) != 0)
		return false;
#endif
	return true;
}
bool util::start_process(const CommandInfo &cmdInfo)
{
	std::string fullCmdPath = cmdInfo.command;
	if(cmdInfo.absoluteCommandPath == false) {
		fullCmdPath = get_program_path();
		fullCmdPath += "/";
		fullCmdPath += cmdInfo.command;
	}
#ifdef __linux__
	if(access(fullCmdPath.c_str(), F_OK) == -1) {
		errno = ENOENT;
		return false;
	}
	std::string cmd = "\"";
	cmd += fullCmdPath.c_str();
	cmd += "\" ";
	cmd += cmdInfo.GetArguments();
	cmd += " &";
	auto r = system(cmd.c_str());
	if(r == -1) {
		errno = ESRCH;
		return false;
	}
	return true;
#else
	auto fp = util::FilePath(fullCmdPath);
	auto path = fp.GetPath();
	auto file = fp.GetFileName();

	std::string pArgs = '\"' + std::string {path} + '\"';
	auto args = cmdInfo.GetArguments();
	if(!args.empty())
		pArgs += " " + args;

	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));
	std::string programPath = std::string {path};
	programPath += "\\";
	programPath += file;
	auto fullPath = programPath + ' ' + args;
	return (CreateProcess(nullptr, const_cast<char *>(fullPath.c_str()), nullptr, nullptr, TRUE, 0, nullptr, nullptr, &si, &pi) != 0) ? true : false;
	//return (CreateProcess(programPath.c_str(),const_cast<char*>(pArgs.c_str()),nullptr,nullptr,TRUE,0,nullptr,nullptr,&si,&pi) != 0) ? true : false; // Does not work in some cases (See ZeroBrane)
#endif
}

bool util::set_env_variable(const std::string &varName, const std::string &value)
{
#ifdef _WIN32
	return SetEnvironmentVariable(varName.c_str(), value.c_str()) != 0;
#else
	return setenv(varName.c_str(), value.c_str(), 1) == 0;
#endif
}
bool util::unset_env_variable(const std::string &varName)
{
#ifdef _WIN32
	return _putenv((varName + "=").c_str()) == 0;
#else
	return unsetenv(varName.c_str()) == 0;
#endif
}
std::optional<std::string> util::get_env_variable(const std::string &varName)
{
	auto *val = getenv(varName.c_str());
	if(!val)
		return {};
	return val;
}

#ifdef _WIN32
util::CommandResult util::start_and_wait_for_command(const CommandInfo &cmdInfo)
{
	std::string path;
	if(!cmdInfo.command.empty()) {
		path = cmdInfo.command;
		if(cmdInfo.absoluteCommandPath == false) {
			path = get_program_path();
			path += "/";
			path += cmdInfo.command;
		}
	}
	STARTUPINFO StartupInfo;
	ZeroMemory(&StartupInfo, sizeof(StartupInfo));

	//
	//util::ScopeGuard sgStdIn {};
	//util::ScopeGuard sgStdOut {};
	HANDLE g_hChildStd_OUT_Rd;
	{
		// See https://stackoverflow.com/a/54689395/2482983
		SECURITY_ATTRIBUTES sa;
		HANDLE g_hChildStd_IN_Rd, g_hChildStd_OUT_Wr, g_hChildStd_IN_Wr; //pipe handles

		sa.nLength = sizeof(SECURITY_ATTRIBUTES);
		sa.bInheritHandle = TRUE;
		sa.lpSecurityDescriptor = NULL;

		if(CreatePipe(&g_hChildStd_IN_Rd, &g_hChildStd_IN_Wr, &sa, 0)) //create stdin pipe
		{
			/*sgStdIn = [&g_hChildStd_IN_Rd,&g_hChildStd_IN_Wr]() {
				CloseHandle(g_hChildStd_IN_Rd);
				CloseHandle(g_hChildStd_IN_Wr);
			};*/

			if(CreatePipe(&g_hChildStd_OUT_Rd, &g_hChildStd_OUT_Wr, &sa, 0)) //create stdout pipe
			{
				/*sgStdOut = [&g_hChildStd_OUT_Wr,&g_hChildStd_OUT_Rd]() {
					CloseHandle(g_hChildStd_OUT_Wr);
					CloseHandle(g_hChildStd_OUT_Rd);
				};*/

				StartupInfo.dwFlags = STARTF_USESTDHANDLES;
				StartupInfo.hStdOutput = g_hChildStd_OUT_Wr;
				// StartupInfo.hStdError = g_hChildStd_OUT_Wr;
				StartupInfo.hStdInput = g_hChildStd_IN_Rd;
			}
		}
	}
	//

	PROCESS_INFORMATION ProcessInfo;
	auto args = cmdInfo.GetArguments();
	auto hProcess = CreateProcess(!path.empty() ? path.c_str() : nullptr, //  pointer to name of executable module
	  (LPSTR)args.c_str(),                                                        //  pointer to command line string
	  nullptr,                                                                   //  pointer to process security attributes
	  nullptr,                                                                   //  pointer to thread security attributes
	  TRUE,                                                                      //  handle inheritance flag
	  0,                                                                         //  creation flags
	  nullptr,                                                                   //  pointer to new environment block
	  cmdInfo.workingDir ? cmdInfo.workingDir->c_str() : nullptr,                //  pointer to current directory name
	  &StartupInfo,                                                              //  pointer to STARTUPINFO
	  &ProcessInfo                                                               //  pointer to PROCESS_INFORMATION
	);
	if(!hProcess) {
		util::CommandResult result {};
		result.executionResult = util::CommandResult::ExecutionResult::Failure;
		result.errorMessage = std::string("CreateProcess failed: ") + util::get_last_system_error_string();
		return result;
	}
	auto r = WaitForSingleObject(ProcessInfo.hProcess,
	  INFINITE // time-out interval in milliseconds
	);
	UNUSED(r);
	DWORD code;
	GetExitCodeProcess(ProcessInfo.hProcess, &code);

	util::CommandResult result {};
	result.exitCode = code;
	result.executionResult = util::CommandResult::ExecutionResult::Success;
	{
		DWORD read;  //bytes read
		DWORD avail; //bytes available
		std::vector<char> buf;

		for(;;) {
			if(!PeekNamedPipe(g_hChildStd_OUT_Rd, nullptr, 0, nullptr, &avail, nullptr))
				break;
			auto offset = buf.size();
			buf.resize(buf.size() + avail);
			auto *data = buf.data() + offset;
			auto toRead = avail;
			PeekNamedPipe(g_hChildStd_OUT_Rd, data, toRead, &read, &avail, nullptr);
			//check to see if there is any data to read from stdout
			if(read != 0) {
				if(ReadFile(g_hChildStd_OUT_Rd, data, toRead, &read, nullptr))
					break;
			}
		}
		result.output = std::string(buf.data(), buf.size());
	}

	// CloseHandle(ProcessInfo.hThread);
	// CloseHandle(ProcessInfo.hProcess);
	return result;
}
#pragma comment(lib, "Dbghelp.lib")
#include <DbgHelp.h>
util::SubSystem util::get_subsystem()
{
	// See https://stackoverflow.com/a/1440163/1879228
	PIMAGE_NT_HEADERS nth = ImageNtHeader((PVOID)GetModuleHandle(NULL));
	switch(nth->OptionalHeader.Subsystem) {
	case IMAGE_SUBSYSTEM_WINDOWS_CUI:
		return SubSystem::Console;
	case IMAGE_SUBSYSTEM_WINDOWS_GUI:
		return SubSystem::GUI;
	}
	return SubSystem::Unknown;
}
#else
util::CommandResult util::start_and_wait_for_command(const CommandInfo &cmdInfo)
{
	int pipefd[2];
	if(pipe(pipefd) == -1) {
		util::CommandResult result {};
		result.executionResult = util::CommandResult::ExecutionResult::Failure;
		result.errorMessage = std::string("pipe failed: ") + std::strerror(errno);
		return result;
	}

	posix_spawn_file_actions_t actions;
	posix_spawn_file_actions_init(&actions);
	posix_spawn_file_actions_addclose(&actions, pipefd[0]);
	posix_spawn_file_actions_adddup2(&actions, pipefd[1], /*STDOUT_FILENO=*/1);

	pid_t pid;
	std::vector<const char *> argv;
	argv.reserve(cmdInfo.args.size() + 2);
	argv.push_back(cmdInfo.command.c_str());
	for(const auto &arg : cmdInfo.args)
		argv.push_back(arg.c_str());
	argv.push_back(nullptr);

	// Spawn the process
	int status = posix_spawnp(&pid, argv[0], &actions,
	  /*attrp=*/nullptr, const_cast<char *const *>(argv.data()), cmdInfo.useParentEnvironment ? environ : nullptr);

	posix_spawn_file_actions_destroy(&actions);
	close(pipefd[1]);

	if(status != 0) {
		close(pipefd[0]);
		util::CommandResult result {};
		result.executionResult = util::CommandResult::ExecutionResult::Failure;
		result.errorMessage = std::string("posix_spawnp: ") + std::strerror(status);
		return result;
	}

	std::string output;
	std::array<char, 256> buf;
	ssize_t n;
	while((n = read(pipefd[0], buf.data(), buf.size())) > 0) {
		output.append(buf.data(), n);
	}
	close(pipefd[0]);

	if(waitpid(pid, &status, 0) == -1) {
		util::CommandResult result {};
		result.executionResult = util::CommandResult::ExecutionResult::Failure;
		result.errorMessage = std::string("waitpid failed: ") + std::strerror(errno);
		return result;
	}
	if(!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
		util::CommandResult result {};
		result.executionResult = util::CommandResult::ExecutionResult::Failure;
		result.errorMessage = "command exited with error";
		result.exitCode = WEXITSTATUS(status);
		return result;
	}

	// Trim trailing newline(s)
	while(!output.empty() && (output.back() == '\n' || output.back() == '\r'))
		output.pop_back();

	util::CommandResult result {};
	result.output = output;
	result.executionResult = util::CommandResult::ExecutionResult::Success;
	result.exitCode = WEXITSTATUS(status);
	return result;
}
#endif

std::string util::get_last_system_error_string()
{
#ifdef _WIN32
	DWORD errorMessageID = ::GetLastError();
	if(errorMessageID == 0)
		return "No error message has been recorded";
	LPSTR messageBuffer = nullptr;
	size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, nullptr);
	std::string message(messageBuffer, size);
	LocalFree(messageBuffer);
	return message;
#else
	return std::strerror(errno);
#endif
}

void util::set_prevent_os_sleep_mode(bool prevent)
{
#ifdef _WIN32
	if(prevent)
		SetThreadExecutionState(ES_CONTINUOUS | ES_SYSTEM_REQUIRED | ES_AWAYMODE_REQUIRED);
	else
		SetThreadExecutionState(ES_CONTINUOUS);
#else
// TODO: Not yet implemented
#endif
}

bool util::is_x64_system()
{
#ifdef __linux__
#if __x86_64__ || __ppc64__
	return true; // 64-bit application can't run on 32-bit system, so it HAS to be 64-bit. Macro is only set for gcc?
#else
	return (access("/lib64/ld-linux-x86-64.so.2", F_OK) != -1) ? true : false;
#endif
#else
#if _WIN64
	return true; // 64-bit application can't run on 32-bit system, so it HAS to be 64-bit. Macro is only set for visual studio?
#else
	SYSTEM_INFO info;
	ZeroMemory(&info, sizeof(info));
	GetNativeSystemInfo(&info);
	return (info.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64) ? true : false;
#endif
#endif
}
bool util::is_x86_system() { return (is_x64_system() == true) ? false : true; }
bool util::is_windows_system()
{
#ifdef _WIN32
	return true;
#else
	return false;
#endif
}
bool util::is_linux_system()
{
#ifdef __linux__
	return true;
#else
	return false;
#endif
}

#ifdef _WIN32
std::unordered_map<std::string, std::string> util::get_launch_parameters() { return get_launch_parameters(__argc, __argv); }
#endif
std::unordered_map<std::string, std::string> util::get_launch_parameters(int argc, char *argv[])
{
	std::unordered_map<std::string, std::string> out;
	for(auto i = 0; i < argc; i++) {
		char *arg = argv[i];
		std::vector<std::string> sub;
		ustring::explode(arg, "=", sub);
		if(!sub.empty()) {
			if(sub.size() > 1)
				out.insert(std::unordered_map<std::string, std::string>::value_type(sub[0], sub[1]));
			else
				out.insert(std::unordered_map<std::string, std::string>::value_type(sub[0], "1"));
		}
	}
	return out;
}

float util::to_float(const std::string_view &str) { return to_float<float>(str); }
int util::to_int(const std::string_view &str) { return to_int<int>(str); }
bool util::to_boolean(const std::string_view &str)
{
	if(str.length() >= 4) {
		if(ustring::match(str, "*true*"))
			return true;
		if(ustring::match(str, "*false*"))
			return false;
	}
	return to_int(str) != 0;
}

std::string util::round_string(double v, int places)
{
	v = umath::round(v, places);
	std::stringstream ss;
	ss << std::fixed << std::setprecision(places) << v;
	return ss.str();
}

util::Version util::string_to_version(const std::string_view &str)
{
	auto v = util::Version();
	auto pos = str.find_first_of('.');
	v.major = util::to_int(str.substr(0, pos));
	v.minor = util::to_int(str.substr(pos + 1, str.length()));
	return v;
}

Float util::get_faded_time_factor(Float cur, Float dur, Float fadeIn, Float fadeOut)
{
	cur = umath::max(cur, 0.f);
	dur = umath::max(dur, 0.f);
	if(cur > dur)
		return 0.f;
	fadeIn = umath::max(fadeIn, 0.f);
	fadeOut = umath::max(fadeOut, 0.f);
	auto scale = 1.f;
	if(cur < fadeIn)
		scale *= cur / fadeIn;
	else if(cur >= (dur - fadeOut))
		scale *= 1.f - ((cur - (dur - fadeOut)) / fadeOut);
	return scale;
}

Float util::get_scale_factor(Float val, Float min, Float max)
{
	if(max < min || val < min)
		return 0.f;
	if(val > max)
		return 1.f;
	return get_scale_factor(max - min, val - min);
}
Float util::get_scale_factor(Float val, Float range)
{
	if(val < 0.f || range == 0.f)
		return 0.f;
	if(val > range)
		return 1.f;
	return val / range;
}

#ifdef _WIN32
bool util::get_registry_key_value(HKey key, const std::string &path, const std::string &strValueName, std::string &strValue)
{
	HKEY hKey;
	std::wstring wpath(path.begin(), path.end());
	LONG lRes = RegOpenKeyExW(reinterpret_cast<HKEY>(key), wpath.c_str(), 0, KEY_READ, &hKey);
	UNUSED(lRes);

	WCHAR szBuffer[512];
	DWORD dwBufferSize = sizeof(szBuffer);
	ULONG nError;
	std::wstring wstrValue;
	std::wstring wstrValueName(strValueName.begin(), strValueName.end());
	nError = RegQueryValueExW(hKey, wstrValueName.c_str(), 0, nullptr, (LPBYTE)szBuffer, &dwBufferSize);
	RegCloseKey(hKey);
	if(nError == ERROR_SUCCESS) {
		wstrValue = szBuffer;
		strValue = std::string(wstrValue.begin(), wstrValue.end());
		return true;
	}
	return false;
}
bool util::get_registry_key_value(HKey key, const std::string &path, const std::string &strValueName, uint64_t &intValue)
{
	HKEY hKey;
	std::wstring wpath(path.begin(), path.end());
	LONG lRes = RegOpenKeyExW(reinterpret_cast<HKEY>(key), wpath.c_str(), 0, KEY_READ, &hKey);
	UNUSED(lRes);

	DWORD dwBufferSize(sizeof(DWORD));
	DWORD nResult(0);
	std::wstring wstrValueName(strValueName.begin(), strValueName.end());
	RegCloseKey(hKey);
	LONG nError = RegQueryValueExW(hKey, wstrValueName.c_str(), 0, nullptr, reinterpret_cast<LPBYTE>(&nResult), &dwBufferSize);
	if(ERROR_SUCCESS == nError) {
		intValue = nResult;
		return true;
	}
	return false;
}
bool util::get_registry_key_value(HKey key, const std::string &path, const std::string &strValueName, bool &val)
{
	uint64_t intVal = 0;
	val = false;
	if(get_registry_key_value(key, path, strValueName, intVal) == false)
		return false;
	val = (intVal != 0) ? true : false;
	return true;
}
#endif

std::string util::get_date_time(const std::string &format)
{
	time_t now = time(0);
	struct tm tstruct;
	char buf[80];
#ifdef _WIN32
	localtime_s(&tstruct, &now);
#else
	tstruct = *localtime(&now);
#endif
	strftime(buf, sizeof(buf), format.c_str(), &tstruct);
	return buf;
}

#ifdef __linux__
// Quote a string so it’s safe to pass to /bin/sh
static std::string shellQuote(const std::string &s) {
    std::string q = "'";
    for (char c : s) {
        if (c == '\'')
            q += "'\\''";
        else
            q += c;
    }
    q += "'";
    return q;
}

// Try running `cmd`; returns true if it exited with status 0.
static bool tryCmd(const std::string &cmd) {
    return std::system((cmd + " > /dev/null 2>&1").c_str()) == 0;
}
#endif

void util::open_path_in_explorer(const std::string &path, const std::optional<std::string> &selectFile)
{
#ifdef _WIN32
	if(selectFile.has_value()) {
		auto absPath = path + *selectFile;
		ustring::replace(absPath, "/", "\\");
		auto *pidl = ILCreateFromPath(absPath.c_str());
		if(pidl) {
			SHOpenFolderAndSelectItems(pidl, 0, 0, 0);
			ILFree(pidl);
			return;
		}
	}
	// Source: https://stackoverflow.com/a/4139684/2482983
	auto npath = path;
	ustring::replace(npath, "/", "\\");
	std::wstring stemp = ustring::string_to_wstring(npath);
	LPCWSTR pszPathToOpen = stemp.c_str();
	PIDLIST_ABSOLUTE pidl;
	auto result = SHParseDisplayName(pszPathToOpen, 0, &pidl, 0, 0);
	if(SUCCEEDED(result)) {
		// we don't want to actually select anything in the folder, so we pass an empty
		// PIDL in the array. if you want to select one or more items in the opened
		// folder you'd need to build the PIDL array appropriately
		ITEMIDLIST idNull = {0};
		LPCITEMIDLIST pidlNull[1] = {&idNull};
		SHOpenFolderAndSelectItems(pidl, 1, pidlNull, 0);
		ILFree(pidl);
	}
#else
	auto fullPath = util::DirPath(path);
	auto shouldSelect = selectFile.has_value();
	if(shouldSelect)
		fullPath = util::FilePath(fullPath, *selectFile);

	auto strFullPath = fullPath.GetString();
	if(shouldSelect && !strFullPath.empty() && strFullPath.back() == '/')
		strFullPath.pop_back();

	std::string arg {};
	if(shouldSelect)
		arg = "--select ";

	const auto p = shellQuote(strFullPath);
	if (tryCmd("nautilus " +arg +p)) return;
	if (tryCmd("dolphin " +arg  +p)) return;
	if (tryCmd("nemo " +p)) return;
	if (tryCmd("thunar " +p)) return;
	if (tryCmd("pcmanfm " +p)) return;

	// Fallback
    const auto &d = shellQuote(strFullPath);
    tryCmd("xdg-open " + d);
#endif
}

void util::open_file_in_default_program(const std::string &filePath)
{
#ifdef _WIN32
	ShellExecute(0, 0, filePath.c_str(), 0, 0, SW_SHOW);
#else
	// TODO: This is untested!
	std::string cmd = "xdg-open " + filePath;
	system(cmd.c_str());
#endif
}

void util::open_url_in_browser(const std::string &url)
{
#ifdef _WIN32
	ShellExecute(nullptr, "open", url.c_str(), nullptr, nullptr, SW_SHOW);
#else
	auto cmd = "open " + url;
	system(cmd.c_str());
#endif
}

void util::set_thread_priority(std::thread &thread, ThreadPriority priority)
{
#ifdef _WIN32
	auto *threadHandle = thread.native_handle();
	switch(priority) {
	case ThreadPriority::Lowest:
		SetThreadPriority(threadHandle, THREAD_PRIORITY_LOWEST);
		break;
	case ThreadPriority::Low:
	case ThreadPriority::BelowNormal:
		SetThreadPriority(threadHandle, THREAD_PRIORITY_BELOW_NORMAL);
		break;
	case ThreadPriority::Normal:
		SetThreadPriority(threadHandle, THREAD_PRIORITY_NORMAL);
		break;
	case ThreadPriority::AboveNormal:
		SetThreadPriority(threadHandle, THREAD_PRIORITY_ABOVE_NORMAL);
		break;
	case ThreadPriority::High:
	case ThreadPriority::Highest:
		SetThreadPriority(threadHandle, THREAD_PRIORITY_HIGHEST);
		break;
	}
#else
	auto threadHandle = thread.native_handle();
	auto minPriority = sched_get_priority_min(SCHED_OTHER);
	auto maxPriority = sched_get_priority_max(SCHED_OTHER);
	switch(priority) {
	case ThreadPriority::Lowest:
		pthread_setschedprio(threadHandle, minPriority);
		break;
	case ThreadPriority::Low:
		pthread_setschedprio(threadHandle, -10);
		break;
	case ThreadPriority::BelowNormal:
		pthread_setschedprio(threadHandle, -5);
		break;
	case ThreadPriority::Normal:
		pthread_setschedprio(threadHandle, 0);
		break;
	case ThreadPriority::AboveNormal:
		pthread_setschedprio(threadHandle, 5);
		break;
	case ThreadPriority::High:
		pthread_setschedprio(threadHandle, 10);
		break;
	case ThreadPriority::Highest:
		pthread_setschedprio(threadHandle, maxPriority);
		break;
	}
#endif
}

#ifdef _WIN32
static bool set_thread_name(HANDLE hThread, const std::string &name)
{
	std::wstring wname(name.begin(), name.end());
	HRESULT hr = SetThreadDescription(hThread, wname.c_str());
	if(FAILED(hr))
		return false;
	return true;
}
static std::string utf8_encode(const std::wstring &wstr)
{
	// See https://stackoverflow.com/a/3999597/2482983
	if(wstr.empty())
		return std::string();
	int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
	std::string strTo(size_needed, 0);
	WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
	return strTo;
}
static std::optional<std::string> get_thread_name(HANDLE hThread)
{
	PWSTR name = nullptr;
	HRESULT hr = GetThreadDescription(hThread, &name);
	if(FAILED(hr))
		return {};
	auto strName = utf8_encode(name);
	LocalFree(name);
	return strName;
}
#endif

bool util::set_thread_name(std::thread &thread, const std::string &name)
{
#ifdef _WIN32
	return ::set_thread_name(thread.native_handle(), name);
#else
	auto handle = thread.native_handle();
	return pthread_setname_np(handle, name.c_str()) == 0;
#endif
}
bool util::set_thread_name(const std::string &name)
{
#ifdef _WIN32
	return ::set_thread_name(GetCurrentThread(), name);
#else
	return prctl(PR_SET_NAME, name.c_str(), 0, 0, 0) == 0;
#endif
}

std::optional<std::string> util::get_thread_name(std::thread &thread)
{
#ifdef _WIN32
	return ::get_thread_name(thread.native_handle());
#else
	auto handle = thread.native_handle();
	std::array<char, 16> name;
	auto res = pthread_getname_np(handle, name.data(), name.size());
	if(res != 0)
		return {};
	return std::string {name.data()};
#endif
}
std::optional<std::string> util::get_thread_name()
{
#ifdef _WIN32
	return ::get_thread_name(GetCurrentThread());
#else
	std::array<char, 16> name;
	auto result = prctl(PR_GET_NAME, name.data());
	if(result != 0)
		return {};
	return std::string {name.data()};
#endif
}

uint64_t util::to_uint64(const std::string_view &str) { return strtoll(str.data(), nullptr, 10); }

#ifdef _WIN32
HWND util::get_window_handle() { return GetActiveWindow(); }
#endif

void util::minimize_window_to_tray()
{
#ifdef _WIN32
	ShowWindow(get_window_handle(), SW_HIDE);
#endif
}

void util::unhide_window()
{
#ifdef _WIN32
	ShowWindow(get_window_handle(), SW_SHOW);
#endif
}

void util::flip_item_sequence(void *sequence, size_t sequenceSize, uint32_t numItems, uint32_t itemStride)
{
	auto *tmp = new uint8_t[itemStride];
	auto *row0 = static_cast<uint8_t *>(sequence);
	auto *row1 = row0 + sequenceSize - itemStride;
	for(auto y = decltype(numItems) {0u}; y < numItems / 2; ++y) {
		memcpy(tmp, row1, itemStride);
		memcpy(row1, row0, itemStride);
		memcpy(row0, tmp, itemStride);

		row0 += itemStride;
		row1 -= itemStride;
	}
	delete[] tmp;
}

void util::flash_window()
{
#ifdef _WIN32
	FLASHWINFO fi;
	fi.cbSize = sizeof(FLASHWINFO);
	fi.hwnd = get_window_handle();
	fi.dwFlags = FLASHW_ALL | FLASHW_TIMERNOFG;
	fi.uCount = 0;
	fi.dwTimeout = 0;
	FlashWindowEx(&fi);
#endif
}

bool util::is_dark_mode()
{
#ifdef _WIN32
	DWORD value = 1; // default to light
	DWORD dataSize = sizeof(value);

	LONG rc = RegGetValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", L"AppsUseLightTheme", RRF_RT_REG_DWORD, nullptr, &value, &dataSize);

	if(rc != ERROR_SUCCESS)
		return false;
	return (value == 0);
#else
	CommandInfo cmdInfo;
	cmdInfo.command = "gsettings";
	cmdInfo.args = {"get", "org.gnome.desktop.interface", "color-scheme"};
	cmdInfo.useParentEnvironment = false;
	auto res = util::start_and_wait_for_command(cmdInfo);
	if(res.executionResult != util::CommandResult::ExecutionResult::Success)
		return false;
	ustring::remove_whitespace(res.output);
	return res.output == "'prefer-dark'";
#endif
}

std::optional<std::string> util::get_system_language()
{
#ifdef _WIN32
	// See https://github.com/boostorg/locale/blob/develop/src/win32/lcid.cpp#L44
	unsigned lcid = 0;
	char iso_639_lang[16];
	char iso_3166_country[16];
	if(GetLocaleInfoA(lcid, LOCALE_SISO639LANGNAME, iso_639_lang, sizeof(iso_639_lang)) == 0)
		return {};
	/*std::string lc_name = iso_639_lang;
	if(GetLocaleInfoA(lcid,LOCALE_SISO3166CTRYNAME,iso_3166_country,sizeof(iso_3166_country))!=0) {
		lc_name += "_";
		lc_name += iso_3166_country;
	}*/
	return iso_639_lang;
#else
	setlocale(LC_ALL, "");
	std::string lan {getenv("LANG")};
	auto uscorePos = lan.find('_');
	auto dotPos = lan.find('.');
	if(uscorePos != std::string::npos)
		lan = lan.substr(0, uscorePos);
	if(dotPos != std::string::npos)
		lan = lan.substr(0, dotPos);
	return lan;
#endif
}

bool util::shutdown_os()
{
#ifdef _WIN32
	// See https://docs.microsoft.com/en-us/windows/win32/shutdown/how-to-shut-down-the-system?redirectedfrom=MSDN
	HANDLE hToken;
	TOKEN_PRIVILEGES tkp;

	// Get a token for this process.
	if(!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
		return (FALSE);

	// Get the LUID for the shutdown privilege.

	LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid);

	tkp.PrivilegeCount = 1; // one privilege to set
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	// Get the shutdown privilege for this process.

	AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);

	if(GetLastError() != ERROR_SUCCESS)
		return FALSE;

	// Shut down the system and force all applications to close.

	if(!ExitWindowsEx(EWX_SHUTDOWN | EWX_FORCE, SHTDN_REASON_MAJOR_OPERATINGSYSTEM | SHTDN_REASON_MINOR_UPGRADE | SHTDN_REASON_FLAG_PLANNED))
		return FALSE;

	//shutdown was successful
	return TRUE;
#else
	reboot(LINUX_REBOOT_CMD_POWER_OFF);
	return true;
#endif
}

// See https://github.com/mariusbancila/stduuid
static util::Uuid uuid_to_bytes(const uuids::uuid &uuid)
{
	auto bytes = uuid.as_bytes();
	util::Uuid result;
	memcpy(result.data(), bytes.data(), util::size_of_container(result));
	return result;
}
util::Uuid util::generate_uuid_v4(const std::optional<uint32_t> seed)
{
	static std::random_device rd;
	static auto seed_data = std::array<int, std::mt19937::state_size> {};
	static auto generated = false;
	if(generated == false) {
		generated = true;
		std::generate(std::begin(seed_data), std::end(seed_data), std::ref(rd));
	}
	static std::seed_seq seq(std::begin(seed_data), std::end(seed_data));
	if(seed.has_value()) {
		std::mt19937 generator(*seed);
		uuids::uuid_random_generator gen {generator};
		return uuid_to_bytes(gen());
	}
	static std::mt19937 generator(seq);
	static uuids::uuid_random_generator gen {generator};
	return uuid_to_bytes(gen());
}
std::string util::uuid_to_string(const util::Uuid &uuid)
{
	std::array<uuids::uuid::value_type, 16> a {};
	static_assert(sizeof(a) == sizeof(uuid));
	memcpy(a.data(), uuid.data(), size_of_container(uuid));
	return uuids::to_string(uuids::uuid {a});
}
util::Uuid util::uuid_string_to_bytes(const std::string &uuid)
{
	auto str = uuids::uuid::from_string(uuid);
	if(str.has_value() == false)
		return {};
	return uuid_to_bytes(*str);
}
bool util::is_uuid(const std::string &uuid) { return uuids::uuid::from_string(uuid).has_value(); }
size_t util::get_uuid_hash(const Uuid &uuid)
{
	size_t hash = 0;
	hash = util::hash_combine<size_t>(hash, uuid[0]);
	hash = util::hash_combine<size_t>(hash, uuid[1]);
	return hash;
}
//
