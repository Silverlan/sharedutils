/*
   base64.cpp and base64.h

   base64 encoding and decoding with C++.
   More information at
     https://renenyffenegger.ch/notes/development/Base64/Encoding-and-decoding-base-64-with-cpp

   Version: 2.rc.08 (release candidate)

   Copyright (C) 2004-2017, 2020, 2021 Ren� Nyffenegger

   This source code is provided 'as-is', without any express or implied
   warranty. In no event will the author be held liable for any damages
   arising from the use of this software.

   Permission is granted to anyone to use this software for any purpose,
   including commercial applications, and to alter it and redistribute it
   freely, subject to the following restrictions:

   1. The origin of this source code must not be misrepresented; you must not
      claim that you wrote the original source code. If you use this source code
      in a product, an acknowledgment in the product documentation would be
      appreciated but is not required.

   2. Altered source versions must be plainly marked as such, and must not be
      misrepresented as being the original source code.

   3. This notice may not be removed or altered from any source distribution.

   Ren� Nyffenegger rene.nyffenegger@adp-gmbh.ch

*/

//
//  base64 encoding and decoding with C++.
//  Version: 2.rc.08 (release candidate)
//

export module pragma.util:base64;

export import std.compat;

export namespace pragma::util {
	DLLSHUTIL std::string base64_encode(std::string const &s, bool url = false);
	DLLSHUTIL std::string base64_encode_pem(std::string const &s);
	DLLSHUTIL std::string base64_encode_mime(std::string const &s);

	DLLSHUTIL std::string base64_decode(std::string const &s, bool remove_linebreaks = false);
	DLLSHUTIL std::string base64_encode(unsigned char const *, size_t len, bool url = false);
};

#if __cplusplus >= 201703L
//
// Interface with std::string_view rather than const std::string&
// Requires C++17
// Provided by Yannic Bonenberger (https://github.com/Yannic)
//
export namespace pragma::util {
	DLLSHUTIL std::string base64_encode(std::string_view s, bool url = false);
	DLLSHUTIL std::string base64_encode_pem(std::string_view s);
	DLLSHUTIL std::string base64_encode_mime(std::string_view s);

	DLLSHUTIL std::string base64_decode(std::string_view s, bool remove_linebreaks = false);
};
#endif // __cplusplus >= 201703L
