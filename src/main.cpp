
#include <Geode/Geode.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>

#include "InputTracker.hpp"

using namespace geode::prelude;

class $modify(GJBaseGameLayer) {
    void handleButton(
        bool down,
        int button,
        bool isPlayer1
    ) {
        static superbuffer::InputTracker tracker;

        const auto mode = this->m_isPracticeMode
            ? superbuffer::PlayMode::Practice
            : superbuffer::PlayMode::Normal;

        const auto event = tracker.record(
            mode,
            isPlayer1,
            down,
            button
        );

        const char* modeName =
            this->m_isPracticeMode
            ? "PRACTICE"
            : "NORMAL";

        const char* playerName =
            isPlayer1 ? "P1" : "P2";

        log::info(
            "SUPERBUFFER TRACKER: Mode={} Player={} "
            "Button={} State={} Event={} Gap={}us",
            modeName,
            playerName,
            button,
            down ? "PRESSED" : "RELEASED",
            event.sequence,
            event.intervalMicros
        );

        GJBaseGameLayer::handleButton(
            down,
            button,
            isPlayer1
        );
    }
};
