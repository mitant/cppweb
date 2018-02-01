#include "stdafx.h"

#include <boost/beast/http.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/strand.hpp>
#include <memory>
#include <iostream>

#include "http_session.h"
#include "listener.h"
#include "common.h"

// Accepts incoming connections and launches the sessions
listener::listener(
  boost::shared_ptr<web_service_context> ctx,
  tcp::endpoint endpoint,
  std::function<void(boost::system::error_code, char const*)> error_handler,
  const std::map<std::string, RouteHandler>& route_handlers)
  : acceptor_(*ctx->get_ioc().get())
  , socket_(*ctx->get_ioc().get())
  , error_handler_(error_handler)
  , route_handlers_(route_handlers)
  , ctx_(ctx)
{
  boost::system::error_code ec;

  // Open the acceptor
  acceptor_.open(endpoint.protocol(), ec);
  if (ec)
  {
    error_handler_(ec, "open");
    return;
  }

  // Bind to the server address
  acceptor_.bind(endpoint, ec);
  if (ec)
  {
    error_handler_(ec, "bind");
    return;
  }

  // Start listening for connections
  acceptor_.listen(
    boost::asio::socket_base::max_listen_connections, ec);
  if (ec)
  {
    error_handler_(ec, "listen");
    return;
  }
}

// Start accepting incoming connections
void
listener::run()
{
  if (!acceptor_.is_open())
    return;
  do_accept();
}

void
listener::do_accept()
{
  acceptor_.async_accept(
    socket_,
    std::bind(
      &listener::on_accept,
      shared_from_this(),
      std::placeholders::_1));
}

void
listener::on_accept(boost::system::error_code ec)
{
  if (ec)
  {
    error_handler_(ec, "accept");
  }
  else
  {
    // Create the session and run it
    std::make_shared<http_session>(
      ctx_,
      std::move(socket_),
      error_handler_,
      route_handlers_)->run();
  }

  // Accept another connection
  do_accept();
}