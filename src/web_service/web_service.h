#pragma once

#include <string>
#include <chrono>

#include "common.h"

class web_service : public std::enable_shared_from_this<web_service>
{
  unsigned short port_;
  std::string ip_address_;
  std::string log_name_;
  boost::shared_ptr<boost::asio::io_context> ioc_;
  std::chrono::system_clock::time_point start_time_;

public:
  web_service(boost::shared_ptr<boost::asio::io_context> ioc, const std::string &ip_address, unsigned short port, const std::string &log_name);

  void run();
};