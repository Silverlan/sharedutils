#include "utildefinitions.h"
#include <string>
#include <array>

// See https://stackoverflow.com/a/11040947/2482983
struct UriTextRangeStructA;
struct UriPathSegmentStructA;
struct UriUriStructA;
namespace uriparser
{
	class DLLSHUTIL Uri
	{
		public:
			Uri(std::string uri);

			~Uri();

			bool isValid() const;

			std::string scheme() const;
			std::string host() const;
			std::string port() const;
			std::string path() const;
			std::string query() const;
			std::string fragment() const;

		private:
			std::string uri_;
			std::array<uint8_t,160>	 uriParse_;
			UriUriStructA &GetUriParse();
			const UriUriStructA &GetUriParse() const {return const_cast<Uri*>(this)->GetUriParse();}
			bool		isValid_;

			std::string fromRange(const UriTextRangeStructA & rng) const;

			std::string fromList(UriPathSegmentStructA * xs, const std::string & delim) const;
	};
}