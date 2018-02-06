#include "stdafx.h"

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/config.hpp>

#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/sinks.hpp>
#include <boost/log/sources/logger.hpp>

#include <algorithm>
#include <string>

#include "common.h"
#include "listener.h"

#include "routes.h"
#include "web_service.h"

namespace
{

void fail(boost::system::error_code ec, char const *what)
{
  BOOST_LOG_TRIVIAL(fatal) << ec.message();
  std::cerr << what << ": " << ec.message() << "\n";
}

void init_logging(const std::string& log_name)
{
  namespace logging = boost::log;
  namespace src = boost::log::sources;
  namespace sinks = boost::log::sinks;
  namespace keywords = boost::log::keywords;

  logging::add_common_attributes();

  logging::add_file_log(
      keywords::file_name = log_name + "_%N.log",
      keywords::rotation_size = 10 * 1024 * 1024,
      keywords::time_based_rotation = sinks::file::rotation_at_time_point(0, 0, 0),
      keywords::auto_flush = true,
      keywords::format = "[%TimeStamp%]: %Message%");

  logging::core::get()->set_filter(
      logging::trivial::severity >= logging::trivial::info);
}
}

web_service::web_service(
  boost::shared_ptr<web_service_context> ctx,
  const std::string &ip_address, 
  unsigned short port,
  const std::string& log_name)
    : port_(port),
      ip_address_(ip_address),
      log_name_(log_name),
      ctx_(ctx)
{
}

void
web_service::run()
{
  start_time_ = std::chrono::system_clock::now();
  init_logging(log_name_);

  auto const address = boost::asio::ip::make_address(ip_address_);
  
  BOOST_LOG_TRIVIAL(info) << "Starting server at " << ip_address_ << ":" << port_;

  std::cout << "Starting server at " << ip_address_ << ":" << port_;

  std::function<void(boost::system::error_code, char const *)> fail = ::fail;

  // Route map
  // TODO: Add any new routes that you create, into this map of routes and handlers!
  std::map<std::string, RouteHandler> route_handlers;
  route_handlers.insert(std::pair<std::string, RouteHandler>("/livecheck", routes::livecheck_handler));
  route_handlers.insert(std::pair<std::string, RouteHandler>("/api_client", routes::api_client_handler));

  // Create and launch a listening port
  std::make_shared<listener>(
      ctx_,
      tcp::endpoint{address, port_},
      fail,
      route_handlers)
      ->run();
}
