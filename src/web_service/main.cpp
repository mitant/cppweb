#include "stdafx.h"

#include <boost/make_shared.hpp>

#include <iostream>
#include <memory>
#include <thread>

#include "common.h"
#include "web_service.h"

int main()
{
  // Check command line arguments.
  if (std::getenv("PORT") == NULL || std::getenv("THREADS") == NULL)
  {
    std::cerr << "Usage: web_service\n"
              << "Please define the PORT and THREADS environment variables\n";
    return EXIT_FAILURE;
  }

  unsigned short port = static_cast<unsigned short>(std::atoi(std::getenv("PORT")));
  int threads = std::max<int>(1, std::atoi(std::getenv("THREADS")));

  boost::shared_ptr<boost::asio::io_context> ioc = boost::make_shared<boost::asio::io_context>(threads);

  std::make_shared<web_service>(
      ioc,
      "0.0.0.0",
      port,
      "web_service")
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