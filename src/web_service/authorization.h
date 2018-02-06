#pragma once

class authorization {
public:
  virtual bool is_authorized(const std::string& authorization_header_val) = 0;
};