#pragma once

#include <boost/beast/http.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl.hpp>
#include "version.h"

#include <functional>

using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>
namespace http = boost::beast::http;    // from <boost/beast/http.hpp>
namespace ssl = boost::asio::ssl;       // from <boost/asio/ssl.hpp>

typedef std::map<std::string, std::string> QueryParameters;
typedef std::function<http::response<http::string_body>(const boost::shared_ptr<boost::asio::io_context>&, QueryParameters, const std::string&)> RouteHandler;

#define HTTP_11 11
#define HTTP_USER_AGENT "CppWeb"