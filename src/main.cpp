
#include <Geode/Geode.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>

#include "InputTracker.hpp"
#include "FrameTiming.hpp"

using namespace geode::prelude;

static superbuffer::FrameTiming frameTiming;

class $modify(SuperBufferGameLayer, GJBaseGameLayer) {

    void update(float dt) {
        GJBaseGameLayer::update(dt);
        frameTiming.updateFrame();
    }

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

        const auto frameEvent = frameTiming.recordInput(
            mode,
            superbuffer::GameMode::Cube,
            isPlayer1,
            down,
            button
        );

        log::info(
            "SUPERBUFFER FRAME: Frame={} Mode={} Player={} "
            "Button={} State={} Event={} Gap={}us",
            frameEvent.frame,
            this->m_isPracticeMode ? "PRACTICE" : "NORMAL",
            isPlayer1 ? "P1" : "P2",
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
