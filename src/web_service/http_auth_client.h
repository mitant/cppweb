#pragma once

#include <string>
#include "common.h"

class http_auth_client {
  boost::shared_ptr<web_service_context> ctx_;
  std::string host_;
  std::string port_;
  std::string auth_header_;
public:
  http_auth_client(const boost::shared_ptr<web_service_context>& ctx, const std::string& host, const std::string& port, const std::string& auth_header);
  http::response<http::dynamic_body> get(char const *target);
};