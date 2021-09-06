#include "sharedutils/util_uri.hpp"
#include <uriparser/Uri.h>

uriparser::Uri::Uri(std::string uri)
	: uri_(uri)
{
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
