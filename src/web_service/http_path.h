#pragma once

#include <map>
#include <string>

namespace util {
	namespace http {
		std::string getRoute(const std::string& path);
		std::map<std::string, std::string> getQueryParameters(const std::string& query);
	}
}