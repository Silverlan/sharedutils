#include "sharedutils/util_uri.hpp"
#include "sharedutils/util_string.h"
#include <uriparser/Uri.h>
#include <limits>

std::string uriparser::escape(const std::string_view &str)
{
	std::string escaped;
	escaped.resize(str.length() *3);
	auto *p = uriEscapeA(str.data(),escaped.data(),false,false);
	escaped.resize(p -escaped.data());
	return escaped;
}
std::string uriparser::unescape(const std::string_view &str)
{
	std::string escaped {str};
	auto *p = uriUnescapeInPlaceA(escaped.data());
	escaped.resize(p -escaped.data());
	return escaped;
}

uriparser::Uri::Uri(std::string uri)
	: uri_{std::move(uri)}
{
	ustring::replace(uri_," ","%20");
	UriParserStateA state_;
	state_.uri = &GetUriParse();
	isValid_   = uriParseUriA(&state_, uri_.c_str()) == URI_SUCCESS;
}

UriUriStructA &uriparser::Uri::GetUriParse()
{
	static_assert(sizeof(UriUriStructA) == decltype(uriParse_){}.size());
	return *reinterpret_cast<UriUriStructA*>(uriParse_.data());
}

uriparser::Uri::~Uri() { uriFreeUriMembersA(&GetUriParse()); }

bool uriparser::Uri::isValid() const { return isValid_; }

std::string uriparser::Uri::scheme()   const { return fromRange(GetUriParse().scheme); }
std::string uriparser::Uri::host()	 const { return fromRange(GetUriParse().hostText); }
std::string uriparser::Uri::port()	 const { return fromRange(GetUriParse().portText); }
std::string uriparser::Uri::path()	 const { return fromList(GetUriParse().pathHead, "/"); }
std::string uriparser::Uri::query()	const { return fromRange(GetUriParse().query); }
std::string uriparser::Uri::fragment() const { return fromRange(GetUriParse().fragment); }

std::string uriparser::Uri::fromRange(const UriTextRangeA & rng) const
{
	return std::string(rng.first, rng.afterLast);
}

std::string uriparser::Uri::fromList(UriPathSegmentA * xs, const std::string & delim) const
{
	UriPathSegmentStructA * head(xs);
	std::string accum;

	while (head)
	{
		accum += delim + fromRange(head->text);
		head = head->next;
	}

	return accum;
}

template<typename T> requires(std::is_same_v<T,std::string> || std::is_same_v<T,std::string_view>)
void uriparser::parse_uri_query(const std::string_view &query,std::unordered_map<T,T> &outParams)
{
	if(query.empty())
		return;
	size_t offset = 0;
	do
	{
		auto posKvSep = query.find('=',offset);
		auto posQuerySep = query.find('&',offset);
		if(posQuerySep <= posKvSep)
		{
			auto key = query.substr(offset,posQuerySep);
			outParams[T{std::move(key)}] = T{};
		}
		else
		{
			auto key = query.substr(offset,posKvSep);
			auto val = query.substr(posKvSep +1,(posQuerySep != std::string::npos) ? (posQuerySep -(posKvSep +1)) : std::numeric_limits<size_t>::max());
			outParams[T{std::move(key)}] = T{std::move(val)};
		}
		offset = (posQuerySep != std::string::npos) ? (posQuerySep +1) : std::string::npos;
	}
	while(offset != std::string::npos);

}
template DLLSHUTIL void uriparser::parse_uri_query(const std::string_view &query,std::unordered_map<std::string,std::string> &outParams);
template DLLSHUTIL void uriparser::parse_uri_query(const std::string_view &query,std::unordered_map<std::string_view,std::string_view> &outParams);
