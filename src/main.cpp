
#include <Geode/Geode.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>

#include "InputTracker.hpp"
#include "FrameTiming.hpp"
#include "ModeDetector.hpp"
#include "ModeSettings.hpp"
#include "InputBuffer.hpp"

using namespace geode::prelude;

static superbuffer::FrameTiming frameTiming;
static bool bufferSelfTestDone = false;

static const char* getGameModeName(
    superbuffer::GameMode mode
) {
    using superbuffer::GameMode;

    switch (mode) {
        case GameMode::Cube:   return "CUBE";
        case GameMode::Ship:   return "SHIP";
        case GameMode::Ball:   return "BALL";
        case GameMode::UFO:    return "UFO";
        case GameMode::Wave:   return "WAVE";
        case GameMode::Robot:  return "ROBOT";
        case GameMode::Spider: return "SPIDER";
        case GameMode::Swing:  return "SWING";
        default:               return "UNKNOWN";
    }
}

static void runBufferSelfTest() {
    if (bufferSelfTestDone) {
        return;
    }

    superbuffer::InputBuffer testBuffer;
    superbuffer::BufferedInput next;

    bool pressQueued = testBuffer.enqueue(
        100, 1, true, true, 3
    );

    bool releaseQueued = testBuffer.enqueue(
        101, 1, false, true, 3
    );

    bool firstFound =
        testBuffer.peekNext(100, next);

    bool firstIsPress =
        firstFound && next.pressed;

    bool firstConsumed =
        testBuffer.consumeNext();

    bool secondFound =
        testBuffer.peekNext(101, next);

    bool secondIsRelease =
        secondFound && !next.pressed;

    bool secondConsumed =
        testBuffer.consumeNext();

    log::info(
        "SUPERBUFFER: Queue Test "
        "Press={} Release={} Order={} "
        "Consumed={} Empty={}",
        pressQueued,
        releaseQueued,
        firstIsPress && secondIsRelease,
        firstConsumed && secondConsumed,
        testBuffer.empty()
    );

    bufferSelfTestDone = true;
}

class $modify(SuperBufferGameLayer, GJBaseGameLayer) {

    void update(float dt) {
        GJBaseGameLayer::update(dt);

        frameTiming.updateFrame();
        runBufferSelfTest();
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

        PlayerObject* player = isPlayer1
            ? this->m_player1
            : this->m_player2;

        const auto gameMode =
            superbuffer::ModeDetector::detect(player);

        if (gameMode == superbuffer::GameMode::Count) {
            log::warn(
                "SUPERBUFFER: Player object unavailable!"
            );

            GJBaseGameLayer::handleButton(
                down, button, isPlayer1
            );
            return;
        }

        const auto event = tracker.record(
            mode, isPlayer1, down, button
        );

        const auto bufferWindow =
            superbuffer::ModeSettings::getBufferWindow(gameMode);

        const auto frameEvent = frameTiming.recordInput(
            mode,
            gameMode,
            isPlayer1,
            down,
            button,
            bufferWindow
        );

        log::info(
            "SUPERBUFFER: Frame={} GameMode={} "
            "Buffer={} Mode={} Player={} Button={} State={} "
            "Event={} Gap={}us",
            frameEvent.frame,
            getGameModeName(gameMode),
            bufferWindow,
            this->m_isPracticeMode ? "PRACTICE" : "NORMAL",
            isPlayer1 ? "P1" : "P2",
            button,
            down ? "PRESSED" : "RELEASED",
            event.sequence,
            event.intervalMicros
        );

        GJBaseGameLayer::handleButton(
            down, button, isPlayer1
        );
    }
};
