#pragma once
#include <string>

namespace telegram {
	void SendMessageT(std::string msg, std::string parsemode = "MarkdownV2");
	void SendDocument(std::string path);
}