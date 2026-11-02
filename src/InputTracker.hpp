
#pragma once

#include <array>
#include <chrono>
#include <cstdint>

namespace superbuffer {

enum class PlayMode : uint8_t {
    Normal = 0,
    Practice = 1
};

struct InputEvent {
    uint64_t sequence = 0;
    int button = 0;
    bool pressed = false;
    uint64_t intervalMicros = 0;
};

struct InputProfile {
    uint64_t totalEvents = 0;
    uint64_t pressCount = 0;
    uint64_t releaseCount = 0;

    std::array<InputEvent, 128> history{};

    size_t historyIndex = 0;

    bool hasPreviousEvent = false;

    std::chrono::steady_clock::time_point lastEvent{};
};

class InputTracker {
private:
    // [0] = Normal
    // [1] = Practice
    // Each mode has separate P1 and P2 profiles.
    std::array<std::array<InputProfile, 2>, 2> profiles{};

public:
    InputEvent record(
        PlayMode mode,
        bool isPlayer1,
        bool pressed,
        int button
    ) {
        auto& profile = profiles[
            static_cast<size_t>(mode)
        ][isPlayer1 ? 0 : 1];

        const auto now =
            std::chrono::steady_clock::now();

        uint64_t interval = 0;

        if (profile.hasPreviousEvent) {
            interval = static_cast<uint64_t>(
                std::chrono::duration_cast<
                    std::chrono::microseconds
                >(now - profile.lastEvent).count()
            );
        }

        profile.lastEvent = now;
        profile.hasPreviousEvent = true;

        profile.totalEvents++;

        if (pressed) {
            profile.pressCount++;
        } else {
            profile.releaseCount++;
        }

        InputEvent event;

        event.sequence = profile.totalEvents;
        event.button = button;
        event.pressed = pressed;
        event.intervalMicros = interval;

        profile.history[
            profile.historyIndex
        ] = event;

        profile.historyIndex =
            (profile.historyIndex + 1) %
            profile.history.size();

        return event;
    }

    const InputProfile& getProfile(
        PlayMode mode,
        bool isPlayer1
    ) const {
        return profiles[
            static_cast<size_t>(mode)
        ][isPlayer1 ? 0 : 1];
    }
};

}
