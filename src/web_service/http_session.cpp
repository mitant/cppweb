#include "http_session.h"

#include <boost/log/trivial.hpp>
#include <boost/asio/bind_executor.hpp>
#include "http_error_handlers.h"
#include "http_path.h"

#include <iostream>

#include "http_session.h"

http_session::http_session(
  const boost::shared_ptr<web_service_context>& ctx,
  tcp::socket socket,
  std::function<void(boost::system::error_code, char const *)> error_handler,
  const std::map<std::string, RouteHandler> &route_handlers)
  : socket_(std::move(socket)), error_handler_(error_handler), route_handlers_(route_handlers), strand_(socket_.get_executor()), timer_(socket_.get_executor().context(),
  (std::chrono::steady_clock::time_point::max)()),
  queue_(*this),
  ctx_(ctx)
{
}

void http_session::run()
{
  // Run the timer. The timer is operated
  // continuously, this simplifies the code.
  on_timer({});

  do_read();
}

void http_session::do_read()
{
  // Set the timer
  timer_.expires_after(std::chrono::seconds(15));

  // Read a request
  http::async_read(socket_, buffer_, req_,
    boost::asio::bind_executor(
      strand_,
      std::bind(
        &http_session::on_read,
        shared_from_this(),
        std::placeholders::_1)));
}

void http_session::on_timer(boost::system::error_code ec)
{
  if (ec && ec != boost::asio::error::operation_aborted)
    return error_handler_(ec, "timer");

  // Verify that the timer really expired since the deadline may have moved.
  if (timer_.expiry() <= std::chrono::steady_clock::now())
  {
    // Closing the socket cancels all outstanding operations. They
    // will complete with boost::asio::error::operation_aborted
    socket_.shutdown(tcp::socket::shutdown_both, ec);
    socket_.close(ec);
    return;
  }

  // Wait on the timer
  timer_.async_wait(
    boost::asio::bind_executor(
      strand_,
      std::bind(
        &http_session::on_timer,
        shared_from_this(),
        std::placeholders::_1)));
}

void http_session::on_read(boost::system::error_code ec)
{
  // Happens when the timer closes the socket
  if (ec == boost::asio::error::operation_aborted)
    return;

  // This means they closed the connection
  if (ec == http::error::end_of_stream)
    return do_close();

  if (ec)
    return error_handler_(ec, "read");

  // Send the response
  handle_request(std::move(req_), queue_);

  // If we aren't at the queue limit, try to pipeline another request
  if (!queue_.is_full())
    do_read();
}

void http_session::on_write(boost::system::error_code ec, bool close)
{
  // Happens when the timer closes the socket
  if (ec == boost::asio::error::operation_aborted)
    return;

  if (ec)
    return error_handler_(ec, "write");

  if (close)
  {
    // This means we should close the connection, usually because
    // the response indicated the "Connection: close" semantic.
    return do_close();
  }

  // Inform the queue that a write completed
  if (queue_.on_write())
  {
    // Read another request
    do_read();
  }
}

void http_session::do_close()
{
  // Send a TCP shutdown
  boost::system::error_code ec;
  socket_.shutdown(tcp::socket::shutdown_send, ec);

  // At this point the connection is closed gracefully
}

template <
  class Body, class Allocator,
  class Send>
  void http_session::handle_request(
    http::request<Body, http::basic_fields<Allocator>> &&req,
    Send &&send)
{
  BOOST_LOG_TRIVIAL(info) << req.method() << "\t" << req.target().to_string();

  auto auth_it = req.base().find(http::field::authorization);
  std::string auth_header;

  if (auth_it != req.base().end()) {
    auth_header = auth_it->value().to_string();
  }

  if (!ctx_->get_authorization()->is_authorized(auth_header)) {
    return send(http_error_handlers::unauthorized());
  }

  // Make sure we can handle the method
  if (req.method() != http::verb::get &&
    req.method() != http::verb::head &&
    req.method() != http::verb::post)
    return send(http_error_handlers::bad_request("Unknown HTTP-method"));

  // Find any routes that can be handled.
  std::string path = req.target().to_string();
  std::map<std::string, std::string> queryParams = util::http::getQueryParameters(path);
  std::string route = util::http::getRoute(path);
  std::map<std::string, RouteHandler>::iterator iter = route_handlers_.find(route);

  if (iter != route_handlers_.end())
  {
    std::string s = req.body();
    return send(iter->second(ctx_, queryParams, s));
  }

  return send(http_error_handlers::not_found(req.target()));
}