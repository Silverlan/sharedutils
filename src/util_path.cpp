/* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "sharedutils/util_path.hpp"
#include "sharedutils/util_file.h"
#include "sharedutils/util_string.h"
#include <algorithm>
#ifdef __linux__
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <unistd.h>
	#include <dirent.h>
#else
	#include "Shlwapi.h"
	#pragma comment(lib,"shlwapi.lib")
#endif

#ifdef _WIN32
static auto USE_CASE_SENSITIVE_PATHS = false;
#else
static auto USE_CASE_SENSITIVE_PATHS = true;
#endif

#pragma optimize("",off)
void util::canonicalize_path(std::string &inOutPath)
{
	if(inOutPath.empty())
		return;
	std::replace(inOutPath.begin(),inOutPath.end(),'/','\\'); // TODO: Do we need to do this?
	ustring::replace(inOutPath,"\\\\","");
#ifdef __linux__
	std::string spathReal = normalizePath(path);
#else
	const char *buf = inOutPath.c_str();
	char pathReal[MAX_PATH];
	PathCanonicalizeA(pathReal,buf);
	std::string spathReal = pathReal;
	if(!spathReal.empty() && spathReal[0] == '\\')
		spathReal = spathReal.substr(1,spathReal.size());
#endif
	inOutPath = std::move(spathReal);
	std::replace(inOutPath.begin(),inOutPath.end(),'\\','/');
}

util::Path::Path(const std::string &path,std::optional<bool> optIsFile)
{
	if(optIsFile.has_value())
		m_bFile = *optIsFile;
	else
		UpdateFileState(path);
	SetPath(path);
}
util::Path::Path(const std::vector<std::string> &fromComponents)
{
	auto first = true;
	for(auto &c : fromComponents)
	{
		if(first)
			first = false;
		else
			m_path += '/';
		m_path += c;
	}
	UpdateFileState(m_path);
}
util::Path &util::Path::operator=(const std::string &path)
{
	UpdateFileState(path);
	SetPath(path);
	return *this;
}

bool util::Path::operator==(const Path &other) const
{
	auto tmpThis = *this;
	tmpThis.Canonicalize();

	auto tmpOther = other;
	tmpOther.Canonicalize();
	return ustring::compare(tmpThis.m_path,tmpOther.m_path,USE_CASE_SENSITIVE_PATHS);
}
bool util::Path::operator==(const std::string &other) const
{
	Path pathOther {other};
	return *this == pathOther;
}
bool util::Path::operator!=(const Path &other) const {return !operator==(other);}
bool util::Path::operator!=(const std::string &other) const {return !operator==(other);}

util::Path util::Path::operator+(const Path &other)
{
	auto cpy = *this;
	cpy += other;
	return cpy;
}
util::Path &util::Path::operator+=(const Path &other)
{
	if(m_bFile)
	{
		if(other.GetString().find('/') != std::string::npos)
			return *this; // Appending a path to a file makes no sense, we'll just ignore it...
		m_path += other.m_path;
		return *this;
	}
	m_path += other.m_path;
	m_bFile = other.m_bFile;
	return *this;
}
util::Path util::Path::operator+(const char *other) {return operator+(std::string{other});}
util::Path &util::Path::operator+=(const char *other) {return operator+=(std::string{other});}

std::string util::Path::GetPath() const
{
	if(m_bFile == false)
		return m_path;
	auto path = m_path;
	auto br = path.rfind('/');
	if(br != std::string::npos)
		path = path.substr(0,br +1);
	return path;
}
std::string util::Path::GetFileName() const
{
	if(m_bFile == false)
		return "";
	auto file = m_path;
	auto br = file.rfind('/');
	if(br != std::string::npos)
		file = file.substr(br +1);
	return file;
}

std::string util::Path::GetFront() const
{
	auto br = m_path.find('/');
	if(br == std::string::npos)
		return "";
	return m_path.substr(0,br);
}
std::string util::Path::GetBack() const
{
	auto br = m_path.rfind('/');
	if(m_bFile == false)
		br = m_path.rfind('/',br -1);
	if(br == std::string::npos)
		return "";
	return m_path.substr(br +1);
}

void util::Path::PopFront()
{
	auto br = m_path.find('/');
	if(br == std::string::npos)
		return;
	m_path = m_path.substr(br +1);
}
void util::Path::PopBack()
{
	auto br = m_path.rfind('/');
	if(m_bFile == false)
		br = m_path.rfind('/',br -1);
	if(br == std::string::npos)
		return;
	m_path = m_path.substr(0,br);
	m_bFile = false;
}
void util::Path::MakeRelative(const Path &relativeTo)
{
	auto itThis = begin();
	auto itOther = relativeTo.begin();
	while(itThis != end() && itOther != relativeTo.end() && *itThis == *itOther)
	{
		PopFront();
		itThis = begin();
		++itOther;
	}
}

uint32_t util::Path::GetComponentCount() const
{
	auto n = std::count(m_path.begin(),m_path.end(),'/');
	if(m_bFile)
		++n;
	return n;
}
bool util::Path::IsEmpty() const {return m_path.empty();}
std::vector<std::string> util::Path::ToComponents() const
{
	std::vector<std::string> components;
	ustring::explode(m_path,"/",components);
	return components;
}
const std::string &util::Path::GetString() const {return m_path;}
void util::Path::MoveUp() {PopBack();}
void util::Path::Canonicalize() {canonicalize_path(m_path);}
bool util::Path::IsFile() const {return m_bFile;}
std::optional<std::string> util::Path::GetFileExtension() const
{
	std::string ext;
	return ufile::get_extension(m_path,&ext) ? ext : std::optional<std::string>{};
}
void util::Path::RemoveFileExtension() {ufile::remove_extension_from_filename(m_path);}

void util::Path::SetPath(const std::string &path)
{
	m_path = path;
	std::replace(m_path.begin(),m_path.end(),'\\','/');
	if(m_path.empty() == false && m_path.front() == '/')
		m_path.erase(m_path.begin());
	if(m_bFile == false && (m_path.empty() || m_path.back() != '/'))
		m_path += '/';
}

void util::Path::UpdateFileState(const std::string &path)
{
	std::string ext;
	m_bFile = ufile::get_extension(path,&ext);
}

util::PathIterator<util::Path> util::Path::begin()
{
	return PathIterator{*this,true};
}
util::PathIterator<util::Path> util::Path::end()
{
	return PathIterator{*this,false};
}
util::PathIterator<const util::Path> util::Path::begin() const
{
	return PathIterator{*this,true};
}
util::PathIterator<const util::Path> util::Path::end() const
{
	return PathIterator{*this,false};
}

/*util::PathIterator<util::Path> util::Path::rbegin()
{
	return PathIterator{*this,false};
}
util::PathIterator<util::Path> util::Path::rend()
{
	return PathIterator{*this,true};
}
util::PathIterator<const util::Path> util::Path::rbegin() const
{
	return PathIterator{*this,false};
}
util::PathIterator<const util::Path> util::Path::rend() const
{
	return PathIterator{*this,true};
}*/

////////////

template<class TPath>
	util::PathIterator<TPath>::PathIterator(TPath &path,bool begin)
	: m_path{&path},m_pos{begin ? 0 : std::string::npos}
{
	if(m_path->GetString().empty())
	{
		m_pos = std::string::npos;
		return;
	}
	if(begin)
		operator++();
}

template<class TPath>
	const typename util::PathIterator<TPath>::value_type &util::PathIterator<TPath>::operator++()
{
	auto curPos = m_pos;
	if(curPos >= m_path->GetString().length())
	{
		m_pos = std::string::npos;
		m_cur = {};
		return m_cur;
	}
	auto brPos = m_path->GetString().find('/',curPos);
	auto newPos = brPos;
	if(newPos == std::string::npos)
		newPos = m_path->GetString().length() -1;
	if(curPos != std::string::npos)
	{
		if(newPos != std::string::npos)
			++newPos;
		if(newPos == curPos)
			m_cur = {};
		else
			m_cur = std::string_view{m_path->GetString()}.substr(curPos,(brPos != std::string::npos) ? (brPos -curPos) : brPos);
	}
	m_pos = (newPos <= m_path->GetString().length()) ? newPos : std::string::npos;
	return m_cur;
}

template<class TPath>
	const typename util::PathIterator<TPath>::value_type &util::PathIterator<TPath>::operator++(int)
{
	return operator++();
}

template<class TPath>
	const typename util::PathIterator<TPath>::value_type &util::PathIterator<TPath>::operator--()
{
	auto curPos = m_pos;
	if(curPos == 0)
		return m_cur;
	auto newPos = m_path->GetString().rfind('/',curPos);
	if(newPos == std::string::npos)
		newPos = 0;
	m_cur = std::string_view{m_path->GetString()}.substr(newPos +1,(curPos != std::string::npos) ? (curPos -newPos -1) : curPos);
	if(newPos != 0)
		--newPos; // Move to before the slash
	m_pos = newPos;
	return m_cur;
}

template<class TPath>
	const typename util::PathIterator<TPath>::value_type &util::PathIterator<TPath>::operator--(int)
{
	return operator--();
}

template<class TPath>
	const typename util::PathIterator<TPath>::value_type &util::PathIterator<TPath>::operator*() const
{
	return m_cur;
}

template<class TPath>
	bool typename util::PathIterator<TPath>::operator==(const PathIterator &other) const
{
	return m_path == other.m_path && m_pos == other.m_pos;
}
template<class TPath>
	bool typename util::PathIterator<TPath>::operator!=(const PathIterator &other) const
{
	return !operator==(other);
}

////////////

util::Path operator+(const std::string &path,const util::Path &other)
{
	util::Path ppath {path};
	return ppath +other;
}

std::ostream &operator<<(std::ostream &out,const util::Path &path)
{
	return out<<path.GetString();
}
#pragma optimize("",on)
