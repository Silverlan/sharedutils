/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __UTIL_STRING_HPP__
#define __UTIL_STRING_HPP__

#include "utildefinitions.h"
#include <string>
#include <string_view>
#include <optional>
#include <algorithm>
#include <vector>
#include <functional>
#include <algorithm>
#include <mathutil/umath.h>
#define StringToLower(str) std::transform(str.begin(), str.end(), str.begin(), ::tolower);
#define StringToUpper(str) std::transform(str.begin(), str.end(), str.begin(), ::toupper);

#if defined(_UNICODE)
#define _T(x) L##x
#ifdef __linux__
#define PTSTR wchar_t *
#define PCTSTR const wchar_t *
#endif
#else
#ifndef _T
#define _T(x) x
#endif
#ifdef __linux__
#define PTSTR char *
#define PCTSTR const char *
#endif
#endif
#ifdef _WIN32
#include <Windows.h>
#endif

namespace ustring {
	DLLSHUTIL extern const std::string WHITESPACE;
	DLLSHUTIL extern const UInt NOT_FOUND;

	DLLSHUTIL void remove_whitespace(std::string_view &s);
	DLLSHUTIL void remove_whitespace(std::string &s);
	DLLSHUTIL void remove_whitespace(std::vector<std::string> &ss);
	DLLSHUTIL void remove_quotes(std::string &s);
	DLLSHUTIL void remove_quotes(std::vector<std::string> &ss);
	DLLSHUTIL void remove_comment(std::string &s);
	DLLSHUTIL void to_lower(std::string &str);
	DLLSHUTIL void to_upper(std::string &str);
	DLLSHUTIL void split(const std::string &str, std::vector<std::string> &substrings);
	DLLSHUTIL void explode(std::string str, const Char *sep, std::vector<std::string> &substrings);
	DLLSHUTIL std::string implode(const std::vector<std::string> &strs, const std::string &separator = " ");
	DLLSHUTIL UInt get_parameters(const std::string &s, std::string &rname, std::vector<std::string> &args);
	DLLSHUTIL UInt find_first_of_outside_quotes(const std::string &str, std::string tofind, UInt qPrev = 0);
	DLLSHUTIL UInt find_first_of(FILE *f, const std::string &tofind, std::string *line = nullptr);
	DLLSHUTIL std::string float_to_string(Float f);
	DLLSHUTIL std::string int_to_string(Int32 i);
	DLLSHUTIL bool is_integer(const std::string &str);
	DLLSHUTIL bool is_number(const std::string &str);
	DLLSHUTIL void get_sequence_commands(const std::string &str, const std::function<void(std::string, std::vector<std::string> &)> &f);
	DLLSHUTIL UInt get_args(const std::string &line, std::vector<std::string> &argv);
	DLLSHUTIL std::vector<std::string> get_args(const std::string &line);
	DLLSHUTIL UInt get_command_args(const std::string &line, std::string &cmd, std::vector<std::string> &argv);
	DLLSHUTIL std::vector<std::string> get_command_args(const std::string &line, std::string &cmd);
	DLLSHUTIL std::string fill_zeroes(const std::string &i, Int32 numDigits);
	DLLSHUTIL Bool get_key_value(const std::string &str, std::string &rkey, std::string &rval, const Char *sep = "=");

	DLLSHUTIL std::string read_until(const std::string &t, Char c);
	DLLSHUTIL std::string read_until_etx(const std::string &t);
	DLLSHUTIL Bool match(PCTSTR pszText, PCTSTR pszMatch, Bool bMatchCase = false);
	DLLSHUTIL Bool match(const std::string_view &text, const std::string_view &match, Bool bMatchCase = false);
	template<class type, class rtype>
	size_t string_to_array(const std::string &str, type *a, rtype (*atot)(const Char *), UInt32 count);
	template<class type>
	size_t string_to_array(const std::string &str, type *a, type (*atot)(const Char *), UInt32 count);
	DLLSHUTIL std::wstring string_to_wstring(const std::string &str);
	DLLSHUTIL std::string wstring_to_string(const std::wstring &str);
	DLLSHUTIL void replace(std::string &str, const std::string &from, const std::string &to);
	template<typename TString>
	void replace(TString &str, const TString &from, const TString &to)
	{
		if(from.empty())
			return;
		size_t start_pos = 0;
		while((start_pos = str.find(from, start_pos)) != TString::npos) {
			str.replace(start_pos, from.length(), to);
			start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
		}
	}
	DLLSHUTIL std::string get_lower(const std::string &str);
	DLLSHUTIL std::string get_upper(const std::string &str);
	DLLSHUTIL int32_t to_int(const std::string &str);
	DLLSHUTIL float to_float(const std::string &str);
	DLLSHUTIL double to_double(const std::string &str);
	DLLSHUTIL void explode_whitespace(const std::string &str, std::vector<std::string> &substrings);
	DLLSHUTIL uint32_t calc_levenshtein_distance(const std::string_view &s1, const std::string_view &s2);
	DLLSHUTIL double calc_levenshtein_similarity(const std::string_view &s1, const std::string_view &s2);
	DLLSHUTIL std::size_t longest_common_substring(const std::string &str1, const std::string &str2, std::size_t &startIdx1, std::size_t &startIdx2);
	DLLSHUTIL std::size_t longest_common_substring(const std::string &str1, const std::string &str2, std::size_t &startIdx1);
	DLLSHUTIL std::size_t longest_common_substring(const std::string &str1, const std::string &str2, std::string &subStr);
	DLLSHUTIL std::size_t longest_common_substring(const std::string &str1, const std::string &str2);
	DLLSHUTIL std::string substr(const std::string &str, std::size_t start, size_t len = std::string::npos);
	DLLSHUTIL std::string_view substr(const std::string_view &str, std::size_t start, size_t len = std::string::npos);
	template<typename T>
	    requires(std::is_same_v<T, std::string> || std::is_same_v<T, std::string_view>) bool
	compare(const T &a, const T &b, bool caseSensitive = true);
	DLLSHUTIL bool compare(const char *a, const char *b, bool caseSensitive = true, size_t len = std::string::npos);
	DLLSHUTIL std::string name_to_identifier(const std::string &name);

	// Calculates the similarity between the two specified strings. The result only makes sense in context,
	// lower values (<0) represent a higher similarity than higher values.
	// This function is VERY expensive (O(n^3)) and should be used sparingly.
	DLLSHUTIL double calc_similarity(const std::string_view &inputWord, const std::string_view &checkWord);

	DLLSHUTIL void gather_similar_elements(const std::string_view &baseElement, const std::vector<std::string> &elements, std::vector<size_t> &outElements, uint32_t limit, std::vector<float> *inOutSimilarities = nullptr);
	DLLSHUTIL void gather_similar_elements(const std::string_view &baseElement, const std::function<std::optional<std::string_view>(void)> &f, std::vector<std::string_view> &outElements, uint32_t limit, std::vector<float> *inOutSimilarities = nullptr);

	DLLSHUTIL uint32_t ip_to_int(const std::string_view &ip);
	DLLSHUTIL std::string int_to_ip(uint32_t ip);

	constexpr char char_to_lower(const char c) { return (c >= 'A' && c <= 'Z') ? c + ('a' - 'A') : c; }

	constexpr bool iequals(const std::string_view &a, const std::string_view &b)
	{
		auto l0 = a.length();
		auto l1 = b.length();
		if(l0 != l1)
			return false;
		for(auto i = decltype(l0) {0u}; i < l0; ++i) {
			if(char_to_lower(a[i]) != char_to_lower(b[i]))
				return false;
		}
		return true;
		// TODO: Use this once C++20 is available
		/*return std::equal(a.begin(), a.end(),
			b.begin(), b.end(),
			[](char a, char b) {
				return char_to_lower(a) == char_to_lower(b);
		});*/
	}

	// See https://stackoverflow.com/a/26082447/2482983
	template<size_t N>
	consteval size_t length(char const (&)[N])
	{
		return N - 1;
	}

	namespace string_switch {
		// See https://stackoverflow.com/a/46711735/2482983
		constexpr uint32_t hash(const char *data, size_t const size) noexcept
		{
			uint32_t hash = 5381;

			for(const char *c = data; c < data + size; ++c)
				hash = ((hash << 5) + hash) + (unsigned char)*c;

			return hash;
		}

		constexpr uint32_t hash(const std::string_view &str) noexcept { return hash(str.data(), str.length()); }

		constexpr inline long long int operator"" _(char const *p, size_t) { return ustring::string_switch::hash(p, std::char_traits<char>::length(p)); }
	}
}

template<class type, class rtype>
size_t ustring::string_to_array(const std::string &str, type *a, rtype (*atot)(const Char *), UInt32 count)
{
	std::vector<std::string> vdat;
	explode(str, WHITESPACE.c_str(), vdat);
	size_t l = vdat.size();
	if(l > count)
		l = count;
	for(unsigned int i = 0; i < l; i++)
		a[i] = static_cast<type>(atot(vdat[i].c_str()));
	return l;
}

template<class type>
size_t ustring::string_to_array(const std::string &str, type *a, type (*atot)(const Char *), UInt32 count)
{
	return string_to_array<type, type>(str, a, atot, count);
}

#endif
