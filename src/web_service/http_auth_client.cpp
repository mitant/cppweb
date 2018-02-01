#include "stdafx.h"

#include <boost/beast/core/flat_buffer.hpp>
#include <boost/asio/connect.hpp>

#include "common.h"
#include "http_auth_client.h"
#include "root_certificates.h"

http_auth_client::http_auth_client(
  const boost::shared_ptr<web_service_context>& ctx,
  const std::string& host,
  const std::string& port,
  const std::string& auth_header)
: ctx_(ctx),
  host_(host),
  port_(port),
  auth_header_(auth_header)
{
}

http::response<http::dynamic_body>
http_auth_client::get(char const *target)
{
  ssl::context ctx{ ssl::context::sslv23_client };
  load_root_certificates(ctx);

  tcp::resolver resolver{ *ctx_->get_ioc().get() };
  ssl::stream<tcp::socket> stream{ *ctx_->get_ioc().get(), ctx };

  if (!SSL_set_tlsext_host_name(stream.native_handle(), host_.c_str()))
  {
    boost::system::error_code ec{ static_cast<int>(::ERR_get_error()), boost::asio::error::get_ssl_category() };
    throw boost::system::system_error{ ec };
  }

  auto const results = resolver.resolve(host_, port_);
  boost::asio::connect(stream.next_layer(), results.begin(), results.end());
  stream.handshake(ssl::stream_base::client);

  // Set up an HTTP GET request message
  http::request<http::string_body> req{ http::verb::get, target, HTTP_11 };
  req.set(http::field::host, host_);
  req.set(http::field::user_agent, HTTP_USER_AGENT);
  req.set(http::field::authorization, auth_header_);

  http::write(stream, req);
  boost::beast::flat_buffer buffer;
  http::response<http::dynamic_body> res;
  http::read(stream, buffer, res);

  boost::system::error_code ec;
  stream.shutdown(ec);
  if (ec == boost::asio::error::eof)
  {
    // Rationale:
    // http://stackoverflow.com/questions/25587403/boost-asio-ssl-async-shutdown-always-finishes-with-an-error
    ec.assign(0, ec.category());
  }
  if (ec)
    throw boost::system::system_error{ ec };

  return res;
}