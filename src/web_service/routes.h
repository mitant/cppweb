#pragma once

#include "common.h"

namespace routes
{
http::response<http::string_body> livecheck_handler(const boost::shared_ptr<boost::asio::io_context> &ioc, QueryParameters queryParameters, const std::string &body);
http::response<http::string_body> api_client_handler(const boost::shared_ptr<boost::asio::io_context> &ioc, QueryParameters queryParameters, const std::string &body);
}