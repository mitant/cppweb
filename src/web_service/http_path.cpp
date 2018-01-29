#include <regex>

#include "http_path.h"

namespace util {
	namespace http {
		std::string getRoute(const std::string& path)
		{
			size_t idx = path.find("?");

			if (idx != std::string::npos)
				return path.substr(0, idx);

			return path;
		}

		std::map<std::string, std::string> getQueryParameters(const std::string& query)
		{
			std::map<std::string, std::string> data;
			std::regex pattern("([\\w+%]+)=([^&]*)");
			auto words_begin = std::sregex_iterator(query.begin(), query.end(), pattern);
			auto words_end = std::sregex_iterator();

			for (std::sregex_iterator i = words_begin; i != words_end; i++)
			{
				std::string key = (*i)[1].str();
				std::string value = (*i)[2].str();
				data[key] = value;
			}

			return data;
		}

	}
}