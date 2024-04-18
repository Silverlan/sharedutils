/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __UTIL_UTF8_HPP__
#define __UTIL_UTF8_HPP__

#include "utildefinitions.h"
#include <sharedutils/tinyutf8/tinyutf8.h>
#include <unordered_map>
#include <string>

namespace util {
	using Utf8StringIterator = tiny_utf8::string::iterator;
	using Utf8String = tiny_utf8::string;

	// This is currently a temporary wrapper over Utf8String until
	// tiny_utf8 implements a string_view. This means that write-operations on this
	// class currently won't work as expected and should not be used!
	struct DLLSHUTIL Utf8StringView {
	  public:
		friend Utf8String;
		static constexpr inline size_t npos = std::string::npos;
		Utf8StringView() = default;
		Utf8StringView(const Utf8String &str);
		Utf8StringView(const std::string &str);
		Utf8StringView(const char *str);
		Utf8StringView(const char *str, size_t count);
		Utf8StringView &operator=(const Utf8String &str);
		Utf8StringView &operator=(const Utf8StringView &str);
		Utf8StringView &operator=(const std::string &str);
		Utf8StringView &operator=(const char *str);
		bool operator==(const Utf8StringView &other) const;
		bool operator!=(const Utf8StringView &other) const;
		std::string cpp_str() const;
		Utf8String &to_str() { return m_string; }
		const Utf8String &to_str() const { return const_cast<Utf8StringView *>(this)->to_str(); }
		size_t find(const char *str, size_t startPos = 0) const;
		size_t find(const std::string &str, size_t startPos = 0) const;
		size_t find(const Utf8String &str, size_t startPos = 0) const;
		Utf8StringIterator begin() const;
		Utf8StringIterator end() const;
		const char *c_str() const;
		const char *data() const;
		size_t length() const;
		size_t size() const;
		bool empty() const { return length() == 0; }
		uint16_t get(size_t idx) const;
		Utf8StringView substr(size_t start, size_t count = std::numeric_limits<size_t>::max()) const;

		operator std::string() const;
	  private:
		Utf8String m_string;
	};
	DLLSHUTIL bool utf8_strncmp(const char *t0, const char *t1, size_t num);
	DLLSHUTIL int utf8_char_to_lower(int c);
	DLLSHUTIL int utf8_char_to_upper(int c);
	DLLSHUTIL void utf8_string_to_upper(std::string &str);
	DLLSHUTIL void utf8_string_to_lower(std::string &str);
};
DLLSHUTIL util::Utf8String &operator+=(util::Utf8String &str, const util::Utf8StringView &view);
DLLSHUTIL std::ostream &operator<<(std::ostream &out, const util::Utf8String &str);
DLLSHUTIL std::ostream &operator<<(std::ostream &out, const util::Utf8StringView &str);

#endif
