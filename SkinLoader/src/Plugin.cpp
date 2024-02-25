#include "Plugin.h"
#include "lodepng.h"

#include <stdexcept>
#include <string_view>

#include <ll/api/memory/Hook.h>
#include <ll/api/memory/Memory.h>
#include <mc/world/actor/player/Player.h>
#include <mc/world/actor/player/SerializedSkin.h>

#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>

#define CONF_PATH "./plugins/SkinLoader/skins/"

namespace plugin {

Plugin::Plugin(ll::plugin::NativePlugin& self) : mSelf(self) {
    try
    {
        if (!std::filesystem::exists(CONF_PATH))
        {
            std::filesystem::create_directories(CONF_PATH);
        }
    }
    catch(...)  { }
}

bool Plugin::enable() {
    mSelf.getLogger().info("Loaded!");
    return true;
}

bool Plugin::disable() {
    return true;
}

} // namespace plugin


LL_AUTO_STATIC_HOOK(
    UpdateSkinHook,
    ll::memory::HookPriority::Normal,
    "?updateSkin@Player@@QEAAXAEBVSerializedSkin@@H@Z",
    void*,
    Player* pl,
    SerializedSkin* skin,
    int hz
) {
    
    std::string skinData(reinterpret_cast<char*>(skin->mSkinImage.mImageBytes.mBlob.get()), skin->mSkinImage.mImageBytes.mSize);
    std::vector<unsigned char> image(skinData.begin(), skinData.end());
    std::vector<unsigned char> png;
    unsigned error = lodepng::encode(png, image, skin->mSkinImage.mWidth, skin->mSkinImage.mHeight);
    if (!error)
    {
      std::string fileName = CONF_PATH + pl->getRealName() + ".png";
      lodepng::save_file(png, fileName);
    }
    return origin(pl, skin, hz);
}