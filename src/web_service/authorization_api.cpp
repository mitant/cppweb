#include "stdafx.h"

#include <iostream>

#include "authorization_api.h"

bool
authorization_api::is_authorized(const std::string& authorization_header_val)
{
  // The authorization header value should be of the form "Bearer <api_key>"
  std::string bearer_token = "Bearer ";
  std::string::size_type idx = authorization_header_val.find(bearer_token);
  std::string api_key;

  if (idx != std::string::npos) {
    api_key = authorization_header_val.substr(idx + bearer_token.size(), authorization_header_val.size() - bearer_token.size());
  }

  return allowed_keys_.find(api_key) != allowed_keys_.end();
}

authorization_api::authorization_api(const std::unordered_set<std::string> allowed_keys)
  : allowed_keys_(allowed_keys)
{
}