
#pragma once

#include <array>
#include <cstdint>
#include <cstddef>

#include "InputTracker.hpp"

namespace superbuffer {

enum class GameMode : std::size_t {
    Cube = 0,
    Ship,
    Ball,
    UFO,
    Wave,
    Robot,
    Spider,
    Swing,
    Count
};

struct FrameInput {
    uint64_t frame = 0;
    int button = 0;
    bool pressed = false;
};

struct FrameProfile {
    uint64_t totalInputs = 0;
    uint64_t lastInputFrame = 0;
    uint32_t bufferWindow = 3;
};

class FrameTiming {
private:
    uint64_t currentFrame = 0;

    std::array<
        std::array<
            std::array<FrameProfile, 2>,
            static_cast<std::size_t>(GameMode::Count)
        >,
        2
    > profiles{};

public:
    void updateFrame() {
        currentFrame++;
    }

    uint64_t getCurrentFrame() const {
        return currentFrame;
    }

    FrameInput recordInput(
        PlayMode playMode,
        GameMode gameMode,
        bool isPlayer1,
        bool pressed,
        int button
    ) {
        auto& profile = profiles[
            static_cast<std::size_t>(playMode)
        ][
            static_cast<std::size_t>(gameMode)
        ][
            isPlayer1 ? 0 : 1
        ];

        profile.totalInputs++;
        profile.lastInputFrame = currentFrame;

        return {
            currentFrame,
            button,
            pressed
        };
    }

    const FrameProfile& getProfile(
        PlayMode playMode,
        GameMode gameMode,
        bool isPlayer1
    ) const {
        return profiles[
            static_cast<std::size_t>(playMode)
        ][
            static_cast<std::size_t>(gameMode)
        ][
            isPlayer1 ? 0 : 1
        ];
    }
};

}
