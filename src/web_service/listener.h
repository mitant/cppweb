#pragma once

#include <boost/beast/core.hpp>
#include "common.h"

class listener : public std::enable_shared_from_this<listener>
{
	tcp::acceptor acceptor_;
	tcp::socket socket_;
	std::function<void(boost::system::error_code, char const*)> error_handler_;
	std::map<std::string, RouteHandler> route_handlers_;
  boost::shared_ptr<web_service_context> ctx_;
public:
	listener(
    boost::shared_ptr<web_service_context> ctx,
		tcp::endpoint endpoint,
		std::function<void(boost::system::error_code, char const*)> error_handler,
		const std::map<std::string, RouteHandler>& route_handlers);

	void run();
	void do_accept();
	void on_accept(boost::system::error_code ec);
};