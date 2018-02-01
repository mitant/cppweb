#pragma once

#include <boost/beast/core.hpp>
#include <boost/asio/bind_executor.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/steady_timer.hpp>

#include <memory>
#include <vector>

#include "common.h"

class http_session : public std::enable_shared_from_this<http_session>
{
  // This queue is used for HTTP pipelining.
  class queue
  {
    enum
    {
      // Maximum number of responses we will queue
      limit = 8
    };

    // The type-erased, saved work item
    struct work
    {
      virtual ~work() = default;
      virtual void operator()() = 0;
    };

    http_session &self_;
    std::vector<std::unique_ptr<work>> items_;

  public:
    explicit queue(http_session &self)
        : self_(self)
    {
      static_assert(limit > 0, "queue limit must be positive");
      items_.reserve(limit);
    }

    // Returns `true` if we have reached the queue limit
    bool
    is_full() const
    {
      return items_.size() >= limit;
    }

    // Called when a message finishes sending
    // Returns `true` if the caller should initiate a read
    bool
    on_write()
    {
      BOOST_ASSERT(!items_.empty());
      auto const was_full = is_full();
      items_.erase(items_.begin());
      if (!items_.empty())
        (*items_.front())();
      return was_full;
    }

    // Called by the HTTP handler to send a response.
    template <bool isRequest, class Body, class Fields>
    void
    operator()(http::message<isRequest, Body, Fields> &&msg)
    {
      // This holds a work item
      struct work_impl : work
      {
        http_session &self_;
        http::message<isRequest, Body, Fields> msg_;

        work_impl(
            http_session &self,
            http::message<isRequest, Body, Fields> &&msg)
            : self_(self), msg_(std::move(msg))
        {
        }

        void
        operator()()
        {
          http::async_write(
              self_.socket_,
              msg_,
              boost::asio::bind_executor(
                  self_.strand_,
                  std::bind(
                      &http_session::on_write,
                      self_.shared_from_this(),
                      std::placeholders::_1,
                      msg_.need_eof())));
        }
      };

      // Allocate and store the work
      items_.emplace_back(new work_impl(self_, std::move(msg)));

      // If there was no previous work, start this one
      if (items_.size() == 1)
        (*items_.front())();
    }
  };

  tcp::socket socket_;
  boost::beast::flat_buffer buffer_;
  http::request<http::string_body> req_;

  std::function<void(boost::system::error_code, char const *)> error_handler_;
  std::map<std::string, RouteHandler> route_handlers_;
  boost::asio::strand<boost::asio::io_context::executor_type> strand_;
  boost::asio::steady_timer timer_;
  queue queue_;
  boost::shared_ptr<web_service_context> ctx_;

  template <
      class Body, class Allocator,
      class Send>
  void
  handle_request(
      http::request<Body, http::basic_fields<Allocator>> &&req,
      Send &&send);

public:
  // Take ownership of the socket
  explicit http_session(
      const boost::shared_ptr<web_service_context>& ctx,
      tcp::socket socket,
      std::function<void(boost::system::error_code, char const *)> error_handler,
      const std::map<std::string, RouteHandler> &route_handlers);

  // Start the asynchronous operation
  void run();
  void do_read();
  void on_timer(boost::system::error_code ec);
  void on_read(boost::system::error_code ec);
  void on_write(boost::system::error_code ec, bool close);
  void do_close();
};