/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "sharedutils/util_markup_file.hpp"
#include "sharedutils/util_string.h"
#include "sharedutils/datastream.h"

using namespace util;

MarkupFile::MarkupFile(const DataStream &ds)
	: m_dataStream{ds}
{}
MarkupFile::ResultCode MarkupFile::ReadUntil(const std::string &str,std::string &readString,bool bExclude,bool bIncludeLast)
{
	auto &ds = m_dataStream;
	for(;;)
	{
		if(ds->GetOffset() == ds->GetDataSize())
			return ResultCode::Eof;
		auto c = ds->Read<char>();
		if(c == '/')
		{
			auto peek = ds->Read<char>();
			if(peek != '/')
				ds->SetOffset(ds->GetOffset() -1);
			else
			{
				std::string tmp {};
				auto r = ReadUntil("\n",tmp);
				if(r != ResultCode::Ok)
					return r;
				ds->SetOffset(ds->GetOffset() +1);
				continue;
			}
		}
		readString += c;
		auto r = str.find(readString.back());
		if((bExclude == false && r != std::string::npos) || (bExclude == true && r == std::string::npos))
		{
			if(bIncludeLast == false)
				readString.erase(readString.end() -1);
			break;
		}
	}
	ds->SetOffset(ds->GetOffset() -1u);
	return ResultCode::Ok;
}

MarkupFile::ResultCode MarkupFile::ReadNextToken(char &token)
{
	auto &ds = m_dataStream;
	auto str = std::string{};
	auto r = ReadUntil(ustring::WHITESPACE,str,true);
	if(r != ResultCode::Ok)
		return r;
	token = str.back();
	if(ds->GetOffset() == ds->GetDataSize())
		return ResultCode::Eof;
	return ResultCode::Ok;
}

MarkupFile::ResultCode MarkupFile::ReadNextParameterToken(char &token)
{
	auto filter = ustring::WHITESPACE;
	filter.erase(std::remove(filter.begin(),filter.end(),'\n'),filter.end());
	auto str = std::string{};
	auto r = ReadUntil(filter,str,true);
	if(r != ResultCode::Ok)
		return r;
	token = str.back();
	if(m_dataStream->GetOffset() == m_dataStream->GetDataSize())
		return ResultCode::Eof;
	return ResultCode::Ok;
}

MarkupFile::ResultCode MarkupFile::ReadNextString(std::string &readStr,const std::string &endOfStringTokens)
{
	auto &str = readStr;
	auto r = ReadUntil(ustring::WHITESPACE,str,true);
	if(r != ResultCode::Ok)
		return r;
	if(endOfStringTokens.find(str.back()) != std::string::npos)
	{
		readStr = {};
		return r;
	}
	if(str.back() == '\"')
	{
		str.clear();
		m_dataStream->SetOffset(m_dataStream->GetOffset() +1);
		r = ReadUntil("\"",str);
		if(r != ResultCode::Ok)
			return r;
		str.erase(str.end() -1);
		m_dataStream->SetOffset(m_dataStream->GetOffset() +1);
	}
	else
	{
		str.clear();
		r = ReadUntil(endOfStringTokens,str,false,false);
		if(r != ResultCode::Ok)
			return r;
	}
	return ResultCode::Ok;
}

void MarkupFile::IncrementFilePos(uint32_t inc)
{
	m_dataStream->SetOffset(m_dataStream->GetOffset() +inc);
}
void MarkupFile::DecrementFilePos(uint32_t dec)
{
	m_dataStream->SetOffset(m_dataStream->GetOffset() -dec);
}

const DataStream &MarkupFile::GetDataStream() const {return const_cast<MarkupFile*>(this)->GetDataStream();}
DataStream &MarkupFile::GetDataStream() {return m_dataStream;}
