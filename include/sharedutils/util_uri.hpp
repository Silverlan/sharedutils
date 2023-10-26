#include "utildefinitions.h"
#include <string>
#include <array>
#include <cinttypes>
#include <unordered_map>

// See https://stackoverflow.com/a/11040947/2482983
struct UriTextRangeStructA;
struct UriPathSegmentStructA;
struct UriUriStructA;
namespace uriparser {
	DLLSHUTIL std::string escape(const std::string_view &str);
	DLLSHUTIL std::string unescape(const std::string_view &str);
	class DLLSHUTIL Uri {
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
		std::array<uint8_t, 160> uriParse_;
		UriUriStructA &GetUriParse();
		const UriUriStructA &GetUriParse() const { return const_cast<Uri *>(this)->GetUriParse(); }
		bool isValid_;

		std::string fromRange(const UriTextRangeStructA &rng) const;

		std::string fromList(UriPathSegmentStructA *xs, const std::string &delim) const;
	};
	template<typename T>
	    requires(std::is_same_v<T, std::string> || std::is_same_v<T, std::string_view>)
	void parse_uri_query(const std::string_view &query, std::unordered_map<T, T> &outParams);
}
