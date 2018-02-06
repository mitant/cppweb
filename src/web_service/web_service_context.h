#pragma once

#include "authorization.h"

#include <boost/asio/io_context.hpp>

// This is a wrapper class around io_context.  This gets passed around to the route handlers.
class web_service_context : public std::enable_shared_from_this<web_service_context>
{
  int threads_;
  boost::shared_ptr<boost::asio::io_context> ioc_;
  boost::shared_ptr<authorization> authorization_;

public:
  web_service_context(int threads, boost::shared_ptr<authorization> authorization);

  boost::shared_ptr<boost::asio::io_context> get_ioc();
  boost::shared_ptr<authorization> get_authorization();
};