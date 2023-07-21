#include <llapi/EventAPI.h>
#include <llapi/mc/Player.hpp>
#include <llapi/mc/Level.hpp>
#include "SkinPro.hpp"
#include <fstream>
#include "lodepng.h"

#pragma comment(lib, "../SDK/lib/bedrock_server_api.lib")
#pragma comment(lib, "../SDK/lib/bedrock_server_var.lib")
#pragma comment(lib, "../SDK/lib/SymDBHelper.lib")
#pragma comment(lib, "../SDK/lib/LiteLoader.lib")

void PluginInit()
{
    Event::PlayerJoinEvent::subscribe_ref([](const Event::PlayerJoinEvent& ev)
        {
            for (auto player : Level::getAllPlayers())
            {
                if (player->getRealName() != ev.mPlayer->getRealName())
                {
                    continue;
                }

                auto& skin = player->getSkin();
                if (skin.mIsPersona)
                {
                    continue;
                }

                std::string skinData(reinterpret_cast<char*>(skin.skin.blob.bytes), skin.skin.blob.size);
                std::vector<unsigned char> image(skinData.begin(), skinData.end());
                std::vector<unsigned char> png;
                unsigned error = lodepng::encode(png, image, skin.skin.width, skin.skin.height);
                if (!error)
                {
                    std::string fileName = "plugins/Skins/" + player->getRealName() + ".png";
                    lodepng::save_file(png, fileName);
                }
            }
            return true;
        });
}
