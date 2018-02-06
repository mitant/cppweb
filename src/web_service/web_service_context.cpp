#include "stdafx.h"

#include <boost/make_shared.hpp>
#include "web_service_context.h"

web_service_context::web_service_context(
    int threads,
    boost::shared_ptr<authorization> authorization) : threads_(threads),
                                                      authorization_(authorization)
{
  ioc_ = boost::make_shared<boost::asio::io_context>(threads);
}

boost::shared_ptr<boost::asio::io_context> web_service_context::get_ioc()
{
  return ioc_;
}

boost::shared_ptr<authorization> web_service_context::get_authorization()
{
  return authorization_;
}