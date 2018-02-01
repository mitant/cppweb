#include "stdafx.h"

#include <boost/make_shared.hpp>
#include "web_service_context.h"

web_service_context::web_service_context(int threads) : threads_(threads)
{
  ioc_ = boost::make_shared<boost::asio::io_context>(threads);
}

boost::shared_ptr<boost::asio::io_context> web_service_context::get_ioc()
{
  return ioc_;
}