#include "stdafx.h"

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/config.hpp>
#include <algorithm>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/sinks.hpp>
#include <boost/log/sources/logger.hpp>

#include "http_auth_client.h"
#include "common.h"
#include "listener.h"

namespace
{
  void init_logging()
  {
    namespace logging = boost::log;
    namespace src = boost::log::sources;
    namespace sinks = boost::log::sinks;
    namespace keywords = boost::log::keywords;

    logging::add_common_attributes();

    logging::add_file_log(
      keywords::file_name = "web_service_%N.log",
      keywords::rotation_size = 10 * 1024 * 1024,
      keywords::time_based_rotation = sinks::file::rotation_at_time_point(0, 0, 0),
      keywords::auto_flush = true,
      keywords::format = "[%TimeStamp%]: %Message%");

    logging::core::get()->set_filter(
      logging::trivial::severity >= logging::trivial::info);
  }

  void fail(boost::system::error_code ec, char const *what)
  {
    BOOST_LOG_TRIVIAL(fatal) << ec.message();
    std::cerr << what << ": " << ec.message() << "\n";
  }

  http::response<http::string_body> livecheck_handler(const boost::shared_ptr<boost::asio::io_context>& ioc, QueryParameters queryParameters, const std::string &body)
  {
    http::response<http::string_body> res;
    res.version(HTTP_11);
    res.result(http::status::ok);
    res.set(http::field::server, HTTP_SERVER_VERSION);
    res.set(http::field::content_type, "text/html");
    res.body() = "Livecheck OK!";
    res.prepare_payload();
    return res;
  }

  http::response<http::string_body> api_client_handler(const boost::shared_ptr<boost::asio::io_context>& ioc, QueryParameters queryParameters, const std::string &body)
  {
    auto const host = "raw.githubusercontent.com";
    auto const port = "443";
    auto const api_key = "somehash";
    auto const target = "/mitant/cppweb/master/README.md";
    std::string auth_header = "";
    auth_header += api_key;

    http_auth_client client(ioc, host, port, auth_header);

    http::response<http::string_body> res;
    res.version(HTTP_11);
    res.result(http::status::ok);
    res.set(http::field::server, HTTP_SERVER_VERSION);
    res.set(http::field::content_type, "text/plain");
    res.body() = buffers_to_string(client.get(target).body().data());
    res.prepare_payload();
    return res;
  }
}

int main(int argc, char *argv[])
{
  // Check command line arguments.
  if (std::getenv("PORT") == NULL || std::getenv("THREADS") == NULL)
  {
    std::cerr << "Usage: web_service\n"
      << "Please define the PORT and THREADS environment variables\n";
    return EXIT_FAILURE;
  }

  init_logging();

  auto const address = boost::asio::ip::make_address("0.0.0.0");
  auto const port = static_cast<unsigned short>(std::atoi(std::getenv("PORT")));
  auto const threads = std::max<int>(1, std::atoi(std::getenv("THREADS")));

  BOOST_LOG_TRIVIAL(info) << "Starting server at " << address << ":" << port;

  std::cout << "Starting server at " << address << ":" << port;

  // The io_context is required for all I/O
  boost::shared_ptr<boost::asio::io_context> ioc = boost::make_shared<boost::asio::io_context>(threads);
  std::function<void(boost::system::error_code, char const *)> fail = ::fail;

  // Route map
  std::map<std::string, RouteHandler> route_handlers;
  route_handlers.insert(std::pair<std::string, RouteHandler>("/livecheck", livecheck_handler));
  route_handlers.insert(std::pair<std::string, RouteHandler>("/api_client", api_client_handler));

  // Create and launch a listening port
  std::make_shared<listener>(
    ioc,
    tcp::endpoint{ address, port },
    fail,
    route_handlers)
    ->run();

  // Run the I/O service on the requested number of threads
  std::vector<std::thread> v;
  v.reserve(threads - 1);
  for (auto i = threads - 1; i > 0; --i)
    v.emplace_back(
      [&ioc] {
    ioc->run();
  });
  ioc->run();

  return EXIT_SUCCESS;
}