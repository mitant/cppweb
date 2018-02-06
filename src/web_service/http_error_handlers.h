#pragma once

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>

namespace http = boost::beast::http;    // from <boost/beast/http.hpp>

namespace http_error_handlers {
	http::response<http::string_body> bad_request(boost::beast::string_view why);
	http::response<http::string_body> not_found(boost::beast::string_view target);
	http::response<http::string_body> server_error(boost::beast::string_view what);
	http::response<http::string_body> unauthorized();
}