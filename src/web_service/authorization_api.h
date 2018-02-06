#pragma once

#include <unordered_set>
#include <memory>

#include "authorization.h"

class authorization_api : public std::enable_shared_from_this<authorization_api>, public authorization {
  std::unordered_set<std::string> allowed_keys_;
public:
  authorization_api(const std::unordered_set<std::string> allowed_keys);
  bool is_authorized(const std::string& authorization_header_val);
};