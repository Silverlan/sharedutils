/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "sharedutils/util_string.h"
#include <mathutil/umath.h>
#include <sstream>
#include <cstring>
#include <cctype>
#ifdef _WIN32
	#include <winsock.h>
	#pragma comment(lib,"Ws2_32.lib") // Required for inet_addr / inet_ntoa
#else
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
#endif

const std::string ustring::WHITESPACE = " \t\f\v\n\r";
const UInt ustring::NOT_FOUND = std::string::npos;

#if defined(_UNICODE)
	#define _T(x) L ##x
#else
	#define _T(x) x
#endif
/*
#ifdef __linux__
    typedef long LONG;
    typedef unsigned short WORD;
    typedef unsigned long DWORD;
    #define MAKELONG(a, b) \
        ((LONG) (((WORD) (a)) | ((DWORD) ((WORD) (b))) << 16))
    PCTSTR CharUpper(PCTSTR text)
    {
        std::string t(text);
        std::transform(t.begin(),t.end(),t.begin(),::toupper);
        return t.c_str();
    }
#endif
*/
Bool ustring::match(PCTSTR pszText, PCTSTR pszMatch, Bool bMatchCase)
{ 
    // Loop over the string 
    while (*pszText && *pszMatch!=_T('*')) 
    { 
        // Found a match on a next normal character 
        if ((bMatchCase ? (*pszText!=*pszMatch) : (toupper(*pszText)!=toupper(*pszMatch))) && 
            *pszMatch!=_T('?')) 
            return false; // TODO: toupper doesn't account for unicode
  
        // still a match 
        pszMatch++; 
        pszText++; 
    } 
  
    // Either we have a wildcard or or we are at end of the string to test 
    if (!*pszText) 
    { 
        // There is a special case were there is only a wildcard char left 
        // on the match string, so we just skip it 
        while (*pszMatch==_T('*')) 
            ++pszMatch; 
        // it would be a match if both strings reached the end of the string 
        return !*pszText && !*pszMatch; 
    } 
    else 
    { 
        // We found a wildcard '*'. We have some chances now: 
        // 1. we just ignore the wildcard and continue 
        // 2. we just skip 1 character in the source and reuse the wildcard 
        if (match(pszText,pszMatch+1,bMatchCase)) 
            return true; 
        else 
            return match(pszText+1,pszMatch,bMatchCase); 
    } 
}
Bool ustring::match(const std::string &text,const std::string &strMatch,Bool bMatchCase) {return match(text.c_str(),strMatch.c_str(),bMatchCase);}
void ustring::remove_whitespace(std::string &s)
{
	if(s.empty())
		return;
	if(s.back() == '\0')
		s = std::string(s.c_str());
	UInt begin = s.find_first_not_of(WHITESPACE);
	UInt end = s.find_last_not_of(WHITESPACE);
	if(begin == UInt(-1) || end == UInt(-1))
	{
		s = "";
		return;
	}
	s = s.substr(begin,(end -begin) +1);
}

void ustring::remove_whitespace(std::vector<std::string> &ss)
{
	for(int i=0;i<ss.size();i++)
		remove_whitespace(ss[i]);
}

void ustring::remove_quotes(std::string &s)
{
	if(s.empty() || s.front() != '\"' || s.back() != '\"') return;
	s = s.substr(1,s.length() -2);
}

void ustring::remove_quotes(std::vector<std::string> &ss)
{
	for(int i=0;i<ss.size();i++)
		remove_quotes(ss[i]);
}

void ustring::remove_comment(std::string &s)
{
	UInt cSt = s.find("//");
	if(cSt == NOT_FOUND) return;
	s = s.substr(0,cSt -1);
}

void ustring::split(const std::string &str,std::vector<std::string> &substrings)
{
	std::string buf;
	std::stringstream ss(str);
	while(ss >> buf)
		substrings.push_back(buf);
}

void ustring::explode(std::string str,const Char *sep,std::vector<std::string> &substrings)
{
	UInt st = find_first_of_outside_quotes(str,sep);
	while(st != NOT_FOUND)
	{
		std::string sub = str.substr(0,st);
		str = str.substr(st +1);
		remove_whitespace(sub);
		substrings.push_back(sub);
		st = find_first_of_outside_quotes(str,sep);
	}
	remove_whitespace(str);
	if(str.empty() == true)
		return;
	substrings.push_back(str);
}

void ustring::explode_whitespace(const std::string &str,std::vector<std::string> &substrings)
{
	auto estr = str;
	auto st = estr.find_first_not_of(WHITESPACE);
	st = find_first_of_outside_quotes(estr,WHITESPACE,st);
	while(st != NOT_FOUND)
	{
		auto sub = estr.substr(0,st);
		remove_whitespace(sub);
		substrings.push_back(sub);

		st = estr.find_first_not_of(WHITESPACE,st);
		if(st == NOT_FOUND)
			return;
		estr = estr.substr(st);
		st = find_first_of_outside_quotes(estr,WHITESPACE);
	}
	remove_whitespace(estr);
	if(estr.empty() == true)
		return;
	substrings.push_back(estr);
}

// See https://en.wikibooks.org/wiki/Algorithm_Implementation/Strings/Levenshtein_distance
uint32_t ustring::calc_levenshtein_distance(const std::string_view &s1,const std::string_view &s2)
{
	const auto len1 = s1.size();
	const auto len2 = s2.size();
	std::vector<std::vector<uint32_t>> d(len1 +1,std::vector<uint32_t>(len2 +1));

	d.at(0).at(0) = 0;
	for(auto i=decltype(len1){1};i<=len1;++i)
		d.at(i).at(0) = i;
	for(auto i=decltype(len2){1};i<=len2;++i)
		d.at(0).at(i) = i;

	for(auto i=decltype(len1){1};i<=len1;++i)
	{
		for(auto j=decltype(len2){1};j<=len2;++j)
			d.at(i).at(j) = std::min({d.at(i -1).at(j) +1,d.at(i).at(j -1) +1,d.at(i -1).at(j -1) +(s1.at(i -1) == s2.at(j -1) ? 0 : 1)});
	}
	return d.at(len1).at(len2);
}

// See http://stackoverflow.com/a/16018452/2482983
double ustring::calc_levenshtein_similarity(const std::string_view &s1,const std::string_view &s2)
{
	auto *longer = &s1;
	auto *shorter = &s2;
	if(s1.length() < s2.length())
	{
		longer = &s2;
		shorter = &s1;
	}

	auto longerLength = longer->length();
	if(longerLength == 0)
		return 1.0;
	return (longerLength -calc_levenshtein_distance(*longer,*shorter)) /static_cast<double>(longerLength);
}

// See https://en.wikibooks.org/wiki/Algorithm_Implementation/Strings/Longest_common_substring#C.2B.2B_2
std::size_t ustring::longest_common_substring(const std::string &str1,const std::string &str2,std::size_t &startIdx1,std::size_t &startIdx2)
{
	if(str1.empty() == true || str2.empty() == true)
		return 0;

	std::vector<decltype(str2.size())> vcurr(str2.size());
	std::vector<decltype(str2.size())> vprev(str2.size());

	decltype(vcurr) *curr = &vcurr;
	decltype(vprev) *prev = &vprev;
	decltype(vcurr) *swap = nullptr;
	auto maxSubstr = decltype(str1.size()){0};

	for(auto i=decltype(str1.size()){0};i<str1.size();++i)
	{
		for(auto j=decltype(str2.size()){0};j<str2.size();++j)
		{
			if(str1.at(i) != str2.at(j))
				curr->at(j) = 0;
			else
			{
				if(i == 0 || j == 0)
					curr->at(j) = 1;
				else
					curr->at(j) = 1 + prev->at(j-1);
				if(maxSubstr < curr->at(j))
				{
					maxSubstr = curr->at(j);
					startIdx1 = i -maxSubstr +1;
					startIdx2 = j -maxSubstr +1;
				}
			}
		}
		swap = curr;
		curr = prev;
		prev = swap;
	}
	return maxSubstr;
}

std::size_t ustring::longest_common_substring(const std::string &str1,const std::string &str2,std::size_t &startIdx1)
{
	std::size_t startIdx2 = 0;
	return longest_common_substring(str1,str2,startIdx1,startIdx2);
}
std::size_t ustring::longest_common_substring(const std::string &str1,const std::string &str2,std::string &subStr)
{
	std::size_t startIdx1 = 0;
	auto len = longest_common_substring(str1,str2,startIdx1);
	if(len == 0)
	{
		subStr.clear();
		return len;
	}
	subStr = str1.substr(startIdx1,len);
	return len;
}
std::size_t ustring::longest_common_substring(const std::string &str1,const std::string &str2)
{
	std::size_t startIdx = 0;
	return longest_common_substring(str1,str2,startIdx);
}
std::string ustring::substr(const std::string &str,std::size_t start,size_t len) {return (start < str.length()) ? str.substr(start,len) : "";}
std::string_view ustring::substr(const std::string_view &str,std::size_t start,size_t len)
{
	return (start < str.length()) ? str.substr(start,len) : std::string_view{};
}
bool ustring::compare(const std::string &a,const std::string &b,bool caseSensitive)
{
	if(caseSensitive == true)
		return a == b;
	if(a.length() != b.length())
		return false;
	return std::equal(b.begin(),b.end(),a.begin(),[](unsigned char a,unsigned char b) {
		return std::tolower(a) == std::tolower(b);
	});
}
bool ustring::compare(const char *a,const char *b,bool caseSensitive,size_t len)
{
	if(caseSensitive == true)
	{
		if(len == std::string::npos)
			return strcmp(a,b) == 0;
		return strncmp(a,b,len) == 0;
	}
	for(;len > 0;++a,++b,--len)
	{
		auto d = tolower(*a) -tolower(*b);
		if(d != 0 || !*a)
			return false;
	}
	return true;
}

UInt ustring::get_parameters(const std::string &s,std::string &rname,std::vector<std::string> &args)
{
	UInt arSt = s.find('(');
	UInt arEn = s.find(')');
	if(arSt == NOT_FOUND || arEn == NOT_FOUND) return arEn;
	std::string name = s.substr(0,arSt);
	rname = name;
	UInt arNext = 0;
	int it = 0;
	do
	{
		arNext = s.find_first_of(",)",arNext +1);
		std::string arg = s.substr(arSt +1,arNext -arSt -1);
		remove_whitespace(arg);
		if(arg.length() == 0 && it == 0)
			return arEn;
		args.push_back(arg);
		arSt = arNext;
		it++;
	}
	while(s[arNext] != ')');
	return arEn;
}

UInt ustring::find_first_of_outside_quotes(const std::string &str,std::string tofind,UInt qPrev)
{
	UInt qStart = 0;
	UInt qEnd;
	qPrev--;
	for(;;)
	{
		qStart = str.find_first_of('\"',qPrev +1);
		qEnd = str.find_first_of('\"',qStart +1);
		UInt f = str.find_first_of(tofind,qPrev +1);
		if(qStart == NOT_FOUND || qEnd == NOT_FOUND || f < qStart)
			return f;
		qPrev = qEnd;
	}
	//return NOT_FOUND;
}

UInt ustring::find_first_of(FILE *f,const std::string &tofind,std::string *line)
{
	Char buf[4096];
	while(fgets(buf,4096,f))
	{
		std::string sbuf(buf);
		remove_comment(sbuf);
		UInt brSt = sbuf.find_first_of(tofind);
		if(brSt != NOT_FOUND)
		{
			if(line != NULL)
				*line = sbuf;
			return brSt;
		}
	}
	return NOT_FOUND;
}

std::string ustring::float_to_string(float f) {return std::to_string(f);}
std::string ustring::int_to_string(int i) {return std::to_string(i);}

UInt ustring::get_args(const std::string &line,std::vector<std::string> &argv)
{
	UInt i = 0;
	while(i != UInt(-1))
	{
		UInt n = line.find_first_not_of(WHITESPACE,i);
		if(n == UInt(-1)) break;
		bool isstr = false;
		if(line[n] == '\"')
		{
			i = line.find_first_of('\"',n +1);
			if(i == UInt(-1))
				i = line.find_first_of(WHITESPACE,n);
			else
			{
				n = n +1;
				isstr = true;
			}
		}
		else
			i = line.find_first_of(WHITESPACE,n);
		argv.push_back(line.substr(n,i -n));
		if(isstr)
			i++;
	}
	return argv.size();
}
std::vector<std::string> ustring::get_args(const std::string &line)
{
	std::vector<std::string> argv;
	get_args(line,argv);
	return argv;
}

UInt ustring::get_command_args(const std::string &line,std::string &cmd,std::vector<std::string> &argv)
{
	argv = get_args(line);
	if(argv.empty())
		cmd = "";
	else
	{
		cmd = argv.front();
		to_lower(cmd);
		argv.erase(argv.begin());
	}
	return argv.size();
}
std::vector<std::string> ustring::get_command_args(const std::string &line,std::string &cmd)
{
	std::vector<std::string> argv;
	get_command_args(line,cmd,argv);
	return argv;
}

void ustring::get_sequence_commands(const std::string &line,const std::function<void(std::string,std::vector<std::string>&)> &f)
{
	std::string substr(line);
	UInt st = substr.find_first_not_of(WHITESPACE);
	if(st == NOT_FOUND)
		return;
	substr = substr.substr(st,substr.length());
	UInt s = find_first_of_outside_quotes(substr,";");
	std::string next;
	bool bNext = false;
	if(s != NOT_FOUND)
	{
		bNext = true;
		next = substr.substr(s +1,substr.length());
		substr = substr.substr(0,s);
	}
	std::string cmd;
	std::vector<std::string> argv = get_command_args(substr,cmd);
	f(cmd,argv);
	if(bNext)
		get_sequence_commands(next.c_str(),f);
}

std::string ustring::read_until(const std::string &t,Char c)
{
	std::string str;
	unsigned int i = 0;
	while(t[i] != c)
	{
		str += t[i];
		i++;
	}
	return str;
}

std::string ustring::read_until_etx(const std::string &t) {return read_until(t,3);}

std::string ustring::fill_zeroes(const std::string &i,int numDigits)
{
	auto l = CUInt(numDigits) -i.length();
	std::stringstream si;
	for(int j=0;j<l;j++)
		si<<"0";
	si<<i;
	return si.str();
}

void ustring::to_lower(std::string &str) {StringToLower(str);}
void ustring::to_upper(std::string &str) {StringToUpper(str);}

Bool ustring::get_key_value(const std::string &str,std::string &rkey,std::string &rval,const Char *sep)
{
	std::vector<std::string> sub;
	ustring::explode(str,sep,sub);
	if(sub.size() < 2)
		return false;
	rkey = sub[0];
	remove_whitespace(rkey);
	remove_quotes(rkey);
	to_lower(rkey);
	rval = sub[1];
	remove_whitespace(rval);
	remove_quotes(rval);
	return true;
}

#ifdef _WIN32
std::wstring ustring::string_to_wstring(const std::string &str)
{
	return std::wstring(str.begin(),str.end());
}
std::string ustring::wstring_to_string(const std::wstring &str)
{
	return std::string(str.begin(),str.end());
}
#endif

DLLSHUTIL void ustring::replace(std::string &str,const std::string &from,const std::string &to)
{
	size_t start_pos = 0;
	while((start_pos=str.find(from,start_pos)) != std::string::npos)
	{
		str.replace(start_pos, from.length(), to);
		start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
	}
}

std::string ustring::get_lower(const std::string &str)
{
	auto out = str;
	to_lower(out);
	return out;
}
std::string ustring::get_upper(const std::string &str)
{
	auto out = str;
	to_upper(out);
	return out;
}
int32_t ustring::to_int(const std::string &str)
{
	return atoi(str.c_str());
}
float ustring::to_float(const std::string &str)
{
	return static_cast<float>(to_double(str));
}
double ustring::to_double(const std::string &str)
{
	return atof(str.c_str());
}
std::string ustring::name_to_identifier(const std::string &name)
{
	auto r = name;
	to_lower(r);
	for(auto &c : r)
	{
		if(c <= 47 || (c >= 58 && c <= 64) || (c >= 91 && c <= 96) || c >= 123)
			c = '_';
	}
	return r;
}

uint32_t ustring::ip_to_int(const std::string_view &ip)
{
	return inet_addr(ip.data());
}
std::string ustring::int_to_ip(uint32_t ip)
{
	in_addr paddr;
	paddr.S_un.S_addr = ip;
	return inet_ntoa(paddr);
}
