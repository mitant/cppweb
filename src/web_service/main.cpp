#include "stdafx.h"

#include <boost/make_shared.hpp>

#include <iostream>
#include <memory>
#include <thread>

#include "common.h"
#include "authorization_api.h"
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

  std::unordered_set<std::string> allowed_keys;
  allowed_keys.insert("SampleKey");
  boost::shared_ptr<authorization_api> auth = boost::make_shared<authorization_api>(allowed_keys);
  boost::shared_ptr<web_service_context> ctx = boost::make_shared<web_service_context>(threads, auth);

  std::make_shared<web_service>(
      ctx,
      "0.0.0.0",
      port,
      "web_service")
      ->run();

  // Run the I/O service on the requested number of threads
  std::vector<std::thread> v;
  v.reserve(threads - 1);
  for (auto i = threads - 1; i > 0; --i)
    v.emplace_back(
        [&ctx] {
          ctx->get_ioc()->run();
        });
  ctx->get_ioc()->run();

  return EXIT_SUCCESS;
}