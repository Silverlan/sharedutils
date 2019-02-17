/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __UTIL_MARKUP_FILE_HPP__
#define __UTIL_MARKUP_FILE_HPP__

#include "utildefinitions.h"
#include "datastream.h"
#include "util_string.h"
#include <string>
#include <cinttypes>

namespace util
{
	class DLLSHUTIL MarkupFile
	{
	public:
		enum class ResultCode : uint32_t
		{
			Ok = 0,
			Eof,
			EndOfBlock,
			Error,

			NoPhonemeData
		};
		MarkupFile(const DataStream &ds);
		ResultCode ReadUntil(const std::string &str,std::string &readString,bool bExclude=false,bool bIncludeLast=true);
		ResultCode ReadNextToken(char &token);
		ResultCode ReadNextParameterToken(char &token);
		ResultCode ReadNextString(std::string &readStr,const std::string &endOfStringTokens=ustring::WHITESPACE);
		void IncrementFilePos(uint32_t inc=1u);
		void DecrementFilePos(uint32_t dec=1u);
		const DataStream &GetDataStream() const;
		DataStream &GetDataStream();
	private:
		DataStream m_dataStream;
	};
};

#endif
