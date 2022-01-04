/* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "sharedutils/util_path.hpp"
#include "sharedutils/util_file.h"
#include "sharedutils/util_string.h"
#include <filesystem>
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

#undef CreateFile

std::string util::get_normalized_path(const std::string &path)
{
	auto newPath = path;
	canonicalize_path(newPath);
	if(newPath.empty() == false && newPath.back() != '/')
		newPath += '/';
	return newPath;
}
void util::canonicalize_path(std::string &inOutPath)
{
	if(inOutPath.empty())
		return;
	std::replace(inOutPath.begin(),inOutPath.end(),'\\','/');
	std::vector<std::string> components;
	ustring::explode(inOutPath,"/",components);

	auto first = true;
	auto startedWithSlash = false;
	auto endedWithSlash = false;

	for(auto it=components.begin();it!=components.end();)
	{
		auto &c = *it;
		ustring::remove_whitespace(c);
		if(first)
		{
			first = false;
			if(c.empty())
				startedWithSlash = true;
		}
		if(it == components.end() -1 && c.empty())
			endedWithSlash = true;
		if(c.empty() || c == ".")
		{
			it = components.erase(it);
			continue;
		}
		if(c == "..")
		{
			if(it == components.begin())
			{
				it = components.erase(it);
				continue;
			}
			auto idx = it -components.begin();
			components.erase(components.begin() +idx -1,components.begin() +idx +1);
			it = components.begin() +(idx -1);
			continue;
		}
		++it;
	}

	std::string newPath;
	newPath.reserve(inOutPath.size());
	first = true;
	for(auto &c : components)
	{
		if(first)
			first = false;
		else
			newPath += '/';
		newPath += c;
	}
	if(startedWithSlash) // If our original path started with a simple slash, we want the new path to also start with it
		newPath = '/' +newPath;
	if(endedWithSlash) // If our original path ended with a simple slash, we want the new path to also end with it
		newPath += '/';
	inOutPath = std::move(newPath);
}

util::Path util::Path::CreatePath(const std::string &path)
{
	auto npath = path;
	if(npath.empty())
		npath = '/';
	else if(npath.back() != '/' && npath.back() != '\\')
		npath += '/';
	return util::Path{npath};
}
util::Path util::Path::CreateFile(const std::string &path)
{
	return util::Path{path};
}

util::Path::Path(const std::string &path)
{
	SetPath(path);
}
util::Path::Path(std::string &&path)
{
	SetPath(std::move(path));
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
}
util::Path &util::Path::operator=(const std::string &path)
{
	SetPath(path);
	return *this;
}
util::Path &util::Path::operator=(std::string &&path)
{
	SetPath(std::move(path));
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

util::Path util::Path::operator+(const Path &other) const
{
	auto cpy = *this;
	cpy += other;
	return cpy;
}
util::Path &util::Path::operator+=(const Path &other)
{
	if(IsFile())
	{
		if(other.GetString().find('/') != std::string::npos)
			return *this; // Appending a path to a file makes no sense, we'll just ignore it...
		m_path += other.m_path;
		return *this;
	}
	m_path += other.m_path;
	return *this;
}
util::Path util::Path::operator+(const char *other) const {return operator+(std::string{other});}
util::Path &util::Path::operator+=(const char *other) {return operator+=(std::string{other});}

std::string_view util::Path::GetPath() const
{
	if(IsFile() == false)
		return m_path;
	std::string_view path = m_path;
	auto br = path.rfind('/');
	return (br != std::string::npos) ? path.substr(0,br +1) : std::string_view{};
}
std::string_view util::Path::GetFileName() const
{
	if(IsFile() == false)
		return "";
	std::string_view file = m_path;
	auto br = file.rfind('/');
	if(br != std::string::npos)
		file = file.substr(br +1);
	return file;
}

std::string_view util::Path::GetFront() const {return GetComponent(0);}
std::string_view util::Path::GetBack() const
{
	auto br = m_path.rfind('/');
	if(IsFile() == false)
	{
		br = m_path.rfind('/',br -1);
		if(br == std::string::npos)
			return m_path;
	}
	if(br == std::string::npos)
		return std::string_view{};
	return std::string_view{m_path}.substr(br +1);
}
std::string_view util::Path::GetComponent(size_t offset,size_t *outOptNextOffset) const
{
	if(offset >= m_path.size())
		return {};
	std::string_view path {m_path};
	path = path.substr(offset);
	auto br = path.find('/');
	if(br == std::string::npos)
	{
		if(outOptNextOffset)
			*outOptNextOffset = std::numeric_limits<size_t>::max();
		return IsFile() ? path : std::string_view{};
	}
	if(outOptNextOffset)
		*outOptNextOffset = offset +br +1;
	return path.substr(0,br);
}

void util::Path::PopFront()
{
	auto br = m_path.find('/');
	if(br == std::string::npos)
	{
		if(IsFile())
			m_path = "";
		return;
	}
	m_path = m_path.substr(br +1);
}
void util::Path::PopBack()
{
	auto br = m_path.rfind('/');
	if(IsFile() == false)
	{
		br = m_path.rfind('/',br -1);
		if(br == std::string::npos)
		{
			m_path = "";
			return;
		}
	}
	if(br == std::string::npos)
		return;
	m_path = m_path.substr(0,br +1);
}
bool util::Path::MakeRelative(const Path &relativeTo)
{
	auto itThis = begin();
	auto itOther = relativeTo.begin();
	uint32_t numMatch = 0;
	while(itThis != end() && itOther != relativeTo.end() && *itThis == *itOther)
	{
		++numMatch;
		++itThis;
		++itOther;
	}
	if(itOther != relativeTo.end())
		return false;
	for(auto i=decltype(numMatch){0u};i<numMatch;++i)
		PopFront();
	return true;
}

std::string util::Path::Move() {return std::move(m_path);}

uint32_t util::Path::GetComponentCount() const
{
	auto n = std::count(m_path.begin(),m_path.end(),'/');
	if(IsFile())
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
bool util::Path::IsFile() const {return !IsDirectory();}
bool util::Path::IsDirectory() const {return m_path.empty() || m_path.back() == '/' || m_path.back() == '\\';}
std::optional<std::string> util::Path::GetFileExtension() const
{
	std::string ext;
	return ufile::get_extension(m_path,&ext) ? ext : std::optional<std::string>{};
}
void util::Path::RemoveFileExtension() {ufile::remove_extension_from_filename(m_path);}

void util::Path::UpdatePath()
{
	std::replace(m_path.begin(),m_path.end(),'\\','/');
	if(m_path.empty() == false && m_path.front() == '/')
		m_path.erase(m_path.begin());
}
void util::Path::SetPath(std::string &&path)
{
	m_path = std::move(path);
	UpdatePath();
}
void util::Path::SetPath(const std::string &path)
{
	m_path = path;
	UpdatePath();
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
	bool util::PathIterator<TPath>::operator==(const PathIterator &other) const
{
	return m_path == other.m_path && m_pos == other.m_pos;
}
template<class TPath>
	bool util::PathIterator<TPath>::operator!=(const PathIterator &other) const
{
	return !operator==(other);
}

template class util::PathIterator<util::Path>;
template class util::PathIterator<const util::Path>;

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
