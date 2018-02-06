#include "common.h"

#include "http_error_handlers.h"

namespace http_error_handlers {
	http::response<http::string_body> bad_request(boost::beast::string_view why) {
		http::response<http::string_body> res{ http::status::bad_request, HTTP_11 };
		res.set(http::field::server, HTTP_SERVER_VERSION);
		res.set(http::field::content_type, "text/html");
		res.body() = why.to_string();
		res.prepare_payload();
		return res;
	}

	http::response<http::string_body> not_found(boost::beast::string_view target) {
		http::response<http::string_body> res{ http::status::not_found, HTTP_11 };
		res.set(http::field::server, HTTP_SERVER_VERSION);
		res.set(http::field::content_type, "text/html");
		res.body() = "The resource '" + target.to_string() + "' was not found.";
		res.prepare_payload();
		return res;
	}

	http::response<http::string_body> server_error(boost::beast::string_view what) {
		http::response<http::string_body> res{ http::status::internal_server_error, HTTP_11 };
		res.set(http::field::server, HTTP_SERVER_VERSION);
		res.set(http::field::content_type, "text/html");
		res.body() = "An error occurred: '" + what.to_string() + "'";
		res.prepare_payload();
		return res;
	}

  http::response<http::string_body> unauthorized() {
		http::response<http::string_body> res{ http::status::unauthorized, HTTP_11 };
		res.set(http::field::server, HTTP_SERVER_VERSION);
		res.set(http::field::content_type, "text/html");
		res.body() = "Unauthorized.";
		res.prepare_payload();
		return res;
	}
}