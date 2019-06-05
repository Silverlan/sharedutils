/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __UTIL_STRING_H__
#define __UTIL_STRING_H__

#include "utildefinitions.h"
#include <string>
#include <string_view>
#include <optional>
#include <algorithm>
#include <vector>
#include <functional>
#include <mathutil/umath.h>
#define StringToLower(str) std::transform(str.begin(),str.end(),str.begin(),::tolower);
#define StringToUpper(str) std::transform(str.begin(),str.end(),str.begin(),::toupper);

#if defined(_UNICODE)
	#define _T(x) L ##x
    #ifdef __linux__
        #define PTSTR wchar_t*
        #define PCTSTR const wchar_t*
    #endif
#else
	#ifndef _T
		#define _T(x) x
	#endif
    #ifdef __linux__
        #define PTSTR char *
        #define PCTSTR const char*
    #endif
#endif
#ifdef _WIN32
    #include <Windows.h>
#endif

namespace ustring
{
	DLLSHUTIL extern const std::string WHITESPACE;
	DLLSHUTIL extern const UInt NOT_FOUND;

	DLLSHUTIL void remove_whitespace(std::string &s);
	DLLSHUTIL void remove_whitespace(std::vector<std::string> &ss);
	DLLSHUTIL void remove_quotes(std::string &s);
	DLLSHUTIL void remove_quotes(std::vector<std::string> &ss);
	DLLSHUTIL void remove_comment(std::string &s);
	DLLSHUTIL void to_lower(std::string &str);
	DLLSHUTIL void to_upper(std::string &str);
	DLLSHUTIL void split(const std::string &str,std::vector<std::string> &substrings);
	DLLSHUTIL void explode(std::string str,const Char *sep,std::vector<std::string> &substrings);
	DLLSHUTIL UInt get_parameters(const std::string &s,std::string &rname,std::vector<std::string> &args);
	DLLSHUTIL UInt find_first_of_outside_quotes(const std::string &str,std::string tofind,UInt qPrev=0);
	DLLSHUTIL UInt find_first_of(FILE *f,const std::string &tofind,std::string *line=nullptr);
	DLLSHUTIL std::string float_to_string(Float f);
	DLLSHUTIL std::string int_to_string(Int32 i);
	DLLSHUTIL void get_sequence_commands(const std::string &str,const std::function<void(std::string,std::vector<std::string>&)> &f);
	DLLSHUTIL UInt get_args(const std::string &line,std::vector<std::string> &argv);
	DLLSHUTIL std::vector<std::string> get_args(const std::string &line);
	DLLSHUTIL UInt get_command_args(const std::string &line,std::string &cmd,std::vector<std::string> &argv);
	DLLSHUTIL std::vector<std::string> get_command_args(const std::string &line,std::string &cmd);
	DLLSHUTIL std::string fill_zeroes(const std::string &i,Int32 numDigits);
	DLLSHUTIL Bool get_key_value(const std::string &str,std::string &rkey,std::string &rval,const Char *sep="=");

	DLLSHUTIL std::string read_until(const std::string &t,Char c);
	DLLSHUTIL std::string read_until_etx(const std::string &t);
	DLLSHUTIL Bool match(PCTSTR pszText,PCTSTR pszMatch,Bool bMatchCase=false);
	DLLSHUTIL Bool match(const std::string &text,const std::string &match,Bool bMatchCase=false);
	template<class type,class rtype>
		void string_to_array(const std::string &str,type *a,rtype(*atot)(const Char*),UInt32 count);
	template<class type>
		void string_to_array(const std::string &str,type *a,type(*atot)(const Char*),UInt32 count);
#ifdef _WIN32
	DLLSHUTIL std::wstring string_to_wstring(const std::string &str);
	DLLSHUTIL std::string wstring_to_string(const std::wstring &str);
#endif
	DLLSHUTIL void replace(std::string &str,const std::string &from,const std::string &to);
	DLLSHUTIL std::string get_lower(const std::string &str);
	DLLSHUTIL std::string get_upper(const std::string &str);
	DLLSHUTIL int32_t to_int(const std::string &str);
	DLLSHUTIL float to_float(const std::string &str);
	DLLSHUTIL double to_double(const std::string &str);
	DLLSHUTIL void explode_whitespace(const std::string &str,std::vector<std::string> &substrings);
	DLLSHUTIL uint32_t calc_levenshtein_distance(const std::string_view &s1,const std::string_view &s2);
	DLLSHUTIL double calc_levenshtein_similarity(const std::string_view &s1,const std::string_view &s2);
	DLLSHUTIL std::size_t longest_common_substring(const std::string &str1,const std::string &str2,std::size_t &startIdx1,std::size_t &startIdx2);
	DLLSHUTIL std::size_t longest_common_substring(const std::string &str1,const std::string &str2,std::size_t &startIdx1);
	DLLSHUTIL std::size_t longest_common_substring(const std::string &str1,const std::string &str2,std::string &subStr);
	DLLSHUTIL std::size_t longest_common_substring(const std::string &str1,const std::string &str2);
	DLLSHUTIL std::string substr(const std::string &str,std::size_t start,size_t len=std::string::npos);
	DLLSHUTIL std::string_view substr(const std::string_view &str,std::size_t start,size_t len=std::string::npos);
	DLLSHUTIL bool compare(const std::string &a,const std::string &b,bool caseSensitive=true);
	DLLSHUTIL bool compare(const char *a,const char *b,bool caseSensitive=true,size_t len=std::string::npos);
	DLLSHUTIL std::string name_to_identifier(const std::string &name);

	// Calculates the similarity between the two specified strings. The result only makes sense in context,
	// lower values (<0) represent a higher similarity than higher values.
	// This function is VERY expensive (O(n^3)) and should be used sparingly.
	DLLSHUTIL double calc_similarity(const std::string_view &inputWord,const std::string_view &checkWord);

	DLLSHUTIL void gather_similar_elements(const std::string_view &baseElement,const std::function<std::optional<std::string_view>(void)> &f,std::vector<std::string_view> &outElements,uint32_t limit,std::vector<float> *inOutSimilarities=nullptr);
}

template<class type,class rtype>
	void ustring::string_to_array(const std::string &str,type *a,rtype(*atot)(const Char*),UInt32 count)
{
	std::vector<std::string> vdat;
	explode(str,WHITESPACE.c_str(),vdat);
	size_t l = vdat.size();
	if(l > count)
		l = count;
	for(unsigned int i=0;i<l;i++)
		a[i] = static_cast<type>(atot(vdat[i].c_str()));
}

template<class type>
	void ustring::string_to_array(const std::string &str,type *a,type(*atot)(const Char*),UInt32 count)
{return string_to_array<type,type>(str,a,atot,count);}

#endif