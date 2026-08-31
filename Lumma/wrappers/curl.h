#pragma once
#include <string>

namespace curl {
	struct HttpResponse {
		std::string body;
		long statusCode = 0;
	};

	HttpResponse Post(const std::string& url, const std::string& data);
	std::string PostDocument(const std::string& url, const std::string& path);
	HttpResponse Get(const std::string& url, const std::string& data = "");
}