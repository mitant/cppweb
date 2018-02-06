#include "stdafx.h"

#include "authorization_dummy.h"

bool
authorization_dummy::is_authorized(const std::string& authorization_header_val)
{
  return true;
}