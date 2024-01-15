#pragma once

#include <ll/api/plugin/NativePlugin.h>
#include <ll/api/event/Event.h>

namespace plugin {

class Plugin {
public:
    explicit Plugin(ll::plugin::NativePlugin& self);

    Plugin(Plugin&&)                 = delete;
    Plugin(const Plugin&)            = delete;
    Plugin& operator=(Plugin&&)      = delete;
    Plugin& operator=(const Plugin&) = delete;

    ~Plugin() = default;

    /// @return True if the plugin is enabled successfully.
    bool enable();

    /// @return True if the plugin is disabled successfully.
    bool disable();

private:
    ll::plugin::NativePlugin& mSelf;
    ll::event::ListenerPtr mPlayerJoinEventListener;
    ll::event::ListenerPtr mPlayerLeaveEventListener;
    ll::event::ListenerPtr mServerStartedEventListener;
};

} // namespace plugin
