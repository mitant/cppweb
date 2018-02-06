#pragma once

#include <memory>

#include "authorization.h"

class authorization_dummy : public std::enable_shared_from_this<authorization_dummy>, public authorization {
public:
  bool is_authorized(const std::string& authorization_header_val);
};