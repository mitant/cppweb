#pragma once

#include <boost/asio/ssl.hpp>
#include <string>

namespace ssl = boost::asio::ssl; // from <boost/asio/ssl.hpp>

namespace detail {
  void load_root_certificates(ssl::context& ctx, boost::system::error_code& ec);
}

void
load_root_certificates(ssl::context& ctx, boost::system::error_code& ec);

void
load_root_certificates(ssl::context& ctx);