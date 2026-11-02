
#pragma once

#include <Geode/Geode.hpp>
#include <cstdint>

#include "FrameTiming.hpp"

namespace superbuffer {

class ModeSettings {
public:

    static uint32_t getBufferWindow(GameMode mode) {
        using geode::Mod;

        switch (mode) {
            case GameMode::Cube:
                return static_cast<uint32_t>(
                    Mod::get()->getSettingValue<int64_t>("cube-buffer")
                );

            case GameMode::Ship:
                return static_cast<uint32_t>(
                    Mod::get()->getSettingValue<int64_t>("ship-buffer")
                );

            case GameMode::Ball:
                return static_cast<uint32_t>(
                    Mod::get()->getSettingValue<int64_t>("ball-buffer")
                );

            case GameMode::UFO:
                return static_cast<uint32_t>(
                    Mod::get()->getSettingValue<int64_t>("ufo-buffer")
                );

            case GameMode::Wave:
                return static_cast<uint32_t>(
                    Mod::get()->getSettingValue<int64_t>("wave-buffer")
                );

            case GameMode::Robot:
                return static_cast<uint32_t>(
                    Mod::get()->getSettingValue<int64_t>("robot-buffer")
                );

            case GameMode::Spider:
                return static_cast<uint32_t>(
                    Mod::get()->getSettingValue<int64_t>("spider-buffer")
                );

            case GameMode::Swing:
                return static_cast<uint32_t>(
                    Mod::get()->getSettingValue<int64_t>("swing-buffer")
                );

            default:
                return 3;
        }
    }
};

}
