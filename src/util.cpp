/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "sharedutils/util.h"
#include <mathutil/umath.h>
#include "sharedutils/util_string.h"
#include "sharedutils/util_file.h"
#include <sstream>
#include <iomanip>
#include <cstring>
#ifdef __linux__
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <unistd.h>
	#include <dirent.h>
	#include <signal.h>
	#include <glob.h>
	#include <stdlib.h>
	#include <string.h>
#else
	#include "Shlwapi.h"
	#include <vector>
	#include <Windows.h>
	#include <TlHelp32.h>
	#include <comutil.h>
	#include <direct.h>
#endif

#pragma comment(lib,"mathutil.lib")

std::string util::get_pretty_bytes(unsigned long long bytes)
{
	auto sz = static_cast<double>(bytes);
    const char *suffixes[] = {
        "Byte",
        "KiB",
        "MiB",
        "GiB",
        "TiB",
        "PiB",
        "EiB",
        "ZiB",
        "YiB"
    };
    char i = 0;
    while(sz >= 1024.0 && i < 8)
    {
        sz /= 1024.0;
        i++;
    }
	std::stringstream ss;
	ss<<util::round_string(sz,2)<<" "<<suffixes[i];
    return ss.str();
}

std::string util::get_pretty_duration(unsigned long long ms,int addSegments,bool bNoMs)
{
	if(addSegments == -1)
		addSegments = 100;
	std::stringstream ss;
	if(ms >= 1000 || bNoMs == true)
	{
		unsigned long long seconds = ms /1000;
		ms %= 1000;
		if(bNoMs == true && ms > 0)
			seconds++;
		if(seconds >= 60)
		{
			unsigned long long minutes = seconds /60;
			seconds %= 60;
			if(minutes >= 60)
			{
				unsigned long long hours = minutes /60;
				minutes %= 60;
				if(hours >= 24)
				{
					unsigned long long days = hours /24;
					hours %= 60;
					if(days > 7)
					{
						unsigned long long weeks = days /7;
						days %= 24;
						ss<<weeks<<"wk";
						if(addSegments-- <= 0)
							return ss.str();
						ss<<" ";
					}
					ss<<days<<"d";
					if(addSegments-- <= 0)
						return ss.str();
					ss<<" ";
				}
				ss<<hours<<"h";
				if(addSegments-- <= 0)
					return ss.str();
				ss<<" ";
			}
			ss<<minutes<<"m";
			if(addSegments-- <= 0)
				return ss.str();
			ss<<" ";
		}
		ss<<seconds<<"s";
		if(bNoMs == true || addSegments-- <= 0)
			return ss.str();
		ss<<" ";
	}
	ss<<ms<<"ms";
	return ss.str();
}

static std::string program_name(bool bPost=false)
{
	std::string programPath = "";
#ifdef __linux__
	pid_t pid = getpid();
	char buf[20] = {0};
	sprintf(buf,"%d",pid);
	std::string _link = "/proc/";
	_link.append(buf);
	_link.append("/exe");
	char proc[512];
	int ch = readlink(_link.c_str(),proc,512);
	if(ch != -1)
	{
		proc[ch] = 0;
		programPath = proc;
		std::string::size_type t = programPath.find_last_of("/");
		programPath = (bPost == false) ? programPath.substr(0,t) : programPath.substr(t +1,programPath.length());
	}
#else
	char path[MAX_PATH +1];
	GetModuleFileName(NULL,path,MAX_PATH +1);

	programPath = path;
	auto br = programPath.rfind("\\");
	programPath = (bPost == false) ? programPath.substr(0,br) : programPath.substr(br +1,programPath.length());
#endif
	return programPath;
}

std::string util::get_program_path()
{
	static std::string programPath = "";
	if(programPath.empty())
		programPath = program_name();
	return programPath;
}

std::string util::get_program_name()
{
	static std::string programName = "";
	if(programName.empty())
		programName = program_name(true);
	return programName;
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
	auto process = OpenProcess(PROCESS_QUERY_INFORMATION,FALSE,static_cast<DWORD>(id));
	if(process == nullptr)
		return false;
	unsigned long exitCode = 0;
	auto r = GetExitCodeProcess(process,&exitCode);
	if(r == false || exitCode != STILL_ACTIVE)
		return false;
	return true;
#else
	return (kill(id,0) == 0) ? true : false;
#endif
}

#ifdef __linux__
static pid_t find_process_id(const char *process_name)
{
	pid_t pid = -1;
	glob_t pglob;
	char *procname,*readbuf;
	int buflen = strlen(process_name) +2;
	unsigned i;

	/* Get a list of all comm files. man 5 proc */
	if(glob("/proc/*/comm",0,NULL,&pglob) != 0)
		return pid;

	/* The comm files include trailing newlines, so... */
	procname = (char*)malloc(buflen);
	strcpy(procname, process_name);
	procname[buflen -2] = '\n';
	procname[buflen -1] = 0;

	/* readbuff will hold the contents of the comm files. */
	readbuf = (char*)malloc(buflen);

	for(i=0;i<pglob.gl_pathc;++i)
	{
		FILE *comm;
		char *ret;

		/* Read the contents of the file. */
		if((comm = fopen(pglob.gl_pathv[i],"r")) == NULL)
			continue;
		ret = fgets(readbuf,buflen,comm);
		fclose(comm);
		if(ret == NULL)
			continue;

		/*
		If comm matches our process name, extract the process ID from the
		path, convert it to a pid_t, and return it.
		*/
		if(strcmp(readbuf, procname) == 0) {
			pid = (pid_t)atoi(pglob.gl_pathv[i] +strlen("/proc/"));
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
	HANDLE SnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	if(SnapShot == INVALID_HANDLE_VALUE)
	return false;
	PROCESSENTRY32 procEntry;
	procEntry.dwSize = sizeof(PROCESSENTRY32);
	if(!Process32First(SnapShot,&procEntry))
		return false;
	do
	{
		if(strcmp(procEntry.szExeFile,name) == 0)
		return true;
	}
	while(Process32Next(SnapShot,&procEntry));
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
	if(!getcwd(cCurrentPath,sizeof(cCurrentPath)))
		return false;
#else
	if(!_getcwd(cCurrentPath,sizeof(cCurrentPath)))
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
	if(!_chdir(path.c_str()))
		return false;
#endif
	return true;
}
bool util::start_process(const char *program,bool bGlobalPath) {return start_process(program,"",bGlobalPath);}
bool util::start_process(const char *program,const std::string &args,bool bGlobalPath)
{
#ifdef __linux__
	std::string path;
	if(bGlobalPath == false)
	{
		path = get_program_path();
		path += "/";
	}
	path += program;
	if(access(path.c_str(),F_OK) == -1)
	{
		errno = ENOENT;
		return false;
	}
	std::string cmd = "\"";
	cmd += path.c_str();
	cmd += "\" ";
	cmd += args;
	cmd += " &";
	auto r = system(cmd.c_str());
	if(r == -1)
	{
		errno = ESRCH;
		return false;
	}
	return true;
#else
	std::string path;
	std::string file;
	if(bGlobalPath == false)
	{
		path = get_program_path();
		file = program;
	}
	else
	{
		path = ufile::get_path_from_filename(program);
		if(path.empty() == false && (path.back() == '/' || path.back() == '\\'))
			path = path.substr(0ull,path.length() -1ull);
		file = ufile::get_file_from_filename(program);
	}
	std::string pArgs = '\"' +path +'\"';
	if(!args.empty())
		pArgs += " " +args;

    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si,sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi,sizeof(pi));
	std::string programPath = path;
	programPath += "\\";
	programPath += file;
	auto fullPath = programPath +' ' +args;
	return (CreateProcess(nullptr,const_cast<char*>(fullPath.c_str()),nullptr,nullptr,TRUE,0,nullptr,nullptr,&si,&pi) != 0) ? true : false;
    //return (CreateProcess(programPath.c_str(),const_cast<char*>(pArgs.c_str()),nullptr,nullptr,TRUE,0,nullptr,nullptr,&si,&pi) != 0) ? true : false; // Does not work in some cases (See ZeroBrane)
#endif
}
bool util::start_process(const char *program,const std::vector<std::string> &args,bool bGlobalPath)
{
	std::stringstream ss;
	bool bFirst = true;
	for(auto i=0;i<args.size();i++)
	{
		if(!args.empty())
		{
			if(bFirst == false)
				ss<<" ";
			else
				bFirst = false;
			std::string arg = args[i];
			ustring::remove_whitespace(arg);
			if(arg.size() > 1 && arg.front() == '"' && arg.back() == '"')
				ss<<arg;
			else
				ss<<'"'<<arg<<'"';
		}
	}
	return start_process(program,ss.str(),bGlobalPath);
}
#ifdef _WIN32
static bool start_and_wait_for_command(const char *program,const char *cmd,const char *cwd,unsigned int *exitCode,bool bGlobalPath)
{
	std::string path;
	if(program != nullptr)
	{
		if(bGlobalPath == true)
			path = program;
		else
		{
			path = util::get_program_path();
			path += "\\";
			path += program;
		}
	}
	STARTUPINFO StartupInfo;
	ZeroMemory(&StartupInfo,sizeof(StartupInfo));
	PROCESS_INFORMATION ProcessInfo;
	auto hProcess = CreateProcess(
		(program != nullptr) ? path.c_str() : nullptr,	//  pointer to name of executable module  
		(LPSTR)cmd,		//  pointer to command line string  
		nullptr,		//  pointer to process security attributes  
		nullptr,		//  pointer to thread security attributes  
		TRUE,			//  handle inheritance flag  
		0,				//  creation flags  
		nullptr,		//  pointer to new environment block  
		cwd,		//  pointer to current directory name  
		&StartupInfo,	//  pointer to STARTUPINFO  
		&ProcessInfo	//  pointer to PROCESS_INFORMATION  
	);
	if(!hProcess)
		return false;
	auto r = WaitForSingleObject(
		ProcessInfo.hProcess,  
		INFINITE	// time-out interval in milliseconds  
	);
	UNUSED(r);
	DWORD code;
	GetExitCodeProcess(ProcessInfo.hProcess,&code);
	if(exitCode != nullptr)
		*exitCode = code;
	return true;
}
bool util::start_and_wait_for_command(const char *cmd,const char *cwd,unsigned int *exitCode) {return ::start_and_wait_for_command(nullptr,cmd,cwd,exitCode,false);}
bool util::start_and_wait_for_process(const char *program,unsigned int *exitCode,bool bGlobalPath) {return ::start_and_wait_for_command(program,nullptr,nullptr,exitCode,bGlobalPath);}
#endif

std::string util::get_last_system_error_string()
{
#ifdef _WIN32
    DWORD errorMessageID = ::GetLastError();
    if(errorMessageID == 0)
        return "No error message has been recorded";
    LPSTR messageBuffer = nullptr;
    size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr,errorMessageID,MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),(LPSTR)&messageBuffer,0,nullptr);
    std::string message(messageBuffer,size);
    LocalFree(messageBuffer);
    return message;
#else
    return std::strerror(errno);
#endif
}

bool util::is_x64_system()
{
#ifdef __linux__
    return (access("/lib64/ld-linux-x86-64.so.2",F_OK) != -1) ? true : false;
#else
    SYSTEM_INFO info;
    ZeroMemory(&info,sizeof(info));
    GetNativeSystemInfo(&info);
    return (info.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64) ? true : false;
#endif
}
bool util::is_x86_system() {return (is_x64_system() == true) ? false : true;}
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
	std::unordered_map<std::string,std::string> util::get_launch_parameters() {return get_launch_parameters(__argc,__argv);}
#endif
std::unordered_map<std::string,std::string> util::get_launch_parameters(int argc,char *argv[])
{
	std::unordered_map<std::string,std::string> out;
	for(auto i=0;i<argc;i++)
	{
		char *arg = argv[i];
		std::vector<std::string> sub;
		ustring::explode(arg,"=",sub);
		if(!sub.empty())
		{
			if(sub.size() > 1)
				out.insert(std::unordered_map<std::string,std::string>::value_type(sub[0],sub[1]));
			else
				out.insert(std::unordered_map<std::string,std::string>::value_type(sub[0],"1"));
		}
	}
	return out;
}

float util::to_float(const char *str) {return to_float<float>(str);}
float util::to_float(const std::string &str) {return to_float<float>(str);}
int util::to_int(const char *str) {return to_int<int>(str);}
int util::to_int(const std::string &str) {return to_int<int>(str);}
bool util::to_boolean(const std::string &str)
{
	if(ustring::match(str,"*true*"))
		return true;
	if(ustring::match(str,"*false*"))
		return false;
	return to_int(str) != 0;
}

std::string util::round_string(double v,int places)
{
	v = umath::round(v,places);
	std::stringstream ss;
	ss<<std::fixed<<std::setprecision(places)<<v;
	return ss.str();
}

util::Version util::string_to_version(const std::string &str)
{
	auto v = util::Version();
	auto pos = str.find_first_of('.');
	v.major = util::to_int(str.substr(0,pos));
	v.minor = util::to_int(str.substr(pos +1,str.length()));
	return v;
}

Float util::get_faded_time_factor(Float cur,Float dur,Float fadeIn,Float fadeOut)
{
	cur = umath::max(cur,0.f);
	dur = umath::max(dur,0.f);
	if(cur > dur)
		return 0.f;
	fadeIn = umath::max(fadeIn,0.f);
	fadeOut = umath::max(fadeOut,0.f);
	auto scale = 1.f;
	if(cur < fadeIn)
		scale *= cur /fadeIn;
	else if(cur >= (dur -fadeOut))
		scale *= 1.f -((cur -(dur -fadeOut)) /fadeOut);
	return scale;
}

Float util::get_scale_factor(Float val,Float min,Float max)
{
	if(max < min || val < min)
		return 0.f;
	if(val > max)
		return 1.f;
	return get_scale_factor(max -min,val -min);
}
Float util::get_scale_factor(Float val,Float range)
{
	if(val < 0.f || range == 0.f)
		return 0.f;
	if(val > range)
		return 1.f;
	return val /range;
}

#ifdef _WIN32
bool util::get_registry_key_value(HKey key,const std::string &path,const std::string &strValueName,std::string &strValue)
{
	HKEY hKey;
	std::wstring wpath(path.begin(),path.end());
	LONG lRes = RegOpenKeyExW(reinterpret_cast<HKEY>(key),wpath.c_str(),0,KEY_READ,&hKey);
	UNUSED(lRes);

	WCHAR szBuffer[512];
	DWORD dwBufferSize = sizeof(szBuffer);
	ULONG nError;
	std::wstring wstrValue;
	std::wstring wstrValueName(strValueName.begin(),strValueName.end());
	nError = RegQueryValueExW(hKey,wstrValueName.c_str(),0,nullptr,(LPBYTE)szBuffer,&dwBufferSize);
	RegCloseKey(hKey);
	if(nError == ERROR_SUCCESS)
	{
		wstrValue = szBuffer;
		strValue = std::string(wstrValue.begin(),wstrValue.end());
		return true;
	}
	return false;
}
bool util::get_registry_key_value(HKey key,const std::string &path,const std::string &strValueName,uint64_t &intValue)
{
	HKEY hKey;
	std::wstring wpath(path.begin(),path.end());
	LONG lRes = RegOpenKeyExW(reinterpret_cast<HKEY>(key),wpath.c_str(),0,KEY_READ,&hKey);
	UNUSED(lRes);

	DWORD dwBufferSize(sizeof(DWORD));
	DWORD nResult(0);
	std::wstring wstrValueName(strValueName.begin(),strValueName.end());
	RegCloseKey(hKey);
	LONG nError = RegQueryValueExW(
		hKey,
		wstrValueName.c_str(),
		0,
		nullptr,
		reinterpret_cast<LPBYTE>(&nResult),
		&dwBufferSize
	);
	if(ERROR_SUCCESS == nError)
	{
		intValue = nResult;
		return true;
	}
	return false;
}
bool util::get_registry_key_value(HKey key,const std::string &path,const std::string &strValueName,bool &val)
{
	uint64_t intVal = 0;
	val = false;
	if(get_registry_key_value(key,path,strValueName,intVal) == false)
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
	localtime_s(&tstruct,&now);
#else
	tstruct = *localtime(&now);
#endif
	strftime(buf,sizeof(buf),format.c_str(),&tstruct);
	return buf;
}