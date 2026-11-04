
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

    using superbuffer::PlayMode;
    using superbuffer::InputBuffer;
    using superbuffer::BufferedInput;

    InputBuffer testBuffer;
    BufferedInput event;

    bool normalP1 = testBuffer.enqueue(
        PlayMode::Normal, 100, 1, true, true, 3
    );

    bool normalP2 = testBuffer.enqueue(
        PlayMode::Normal, 100, 1, true, false, 3
    );

    bool practiceP1 = testBuffer.enqueue(
        PlayMode::Practice, 100, 1, false, true, 3
    );

    bool practiceP2 = testBuffer.enqueue(
        PlayMode::Practice, 100, 1, false, false, 3
    );

    bool foundNormalP1 =
        testBuffer.peekNext(
            PlayMode::Normal, true, 100, event
        ) && event.pressed && event.isPlayer1;

    bool foundNormalP2 =
        testBuffer.peekNext(
            PlayMode::Normal, false, 100, event
        ) && event.pressed && !event.isPlayer1;

    bool foundPracticeP1 =
        testBuffer.peekNext(
            PlayMode::Practice, true, 100, event
        ) && !event.pressed && event.isPlayer1;

    bool foundPracticeP2 =
        testBuffer.peekNext(
            PlayMode::Practice, false, 100, event
        ) && !event.pressed && !event.isPlayer1;

    bool isolated =
        testBuffer.size(PlayMode::Normal, true) == 1 &&
        testBuffer.size(PlayMode::Normal, false) == 1 &&
        testBuffer.size(PlayMode::Practice, true) == 1 &&
        testBuffer.size(PlayMode::Practice, false) == 1;

    bool consumed =
        testBuffer.consumeNext(PlayMode::Normal, true) &&
        testBuffer.consumeNext(PlayMode::Normal, false) &&
        testBuffer.consumeNext(PlayMode::Practice, true) &&
        testBuffer.consumeNext(PlayMode::Practice, false);

    bool allEmpty =
        testBuffer.empty(PlayMode::Normal, true) &&
        testBuffer.empty(PlayMode::Normal, false) &&
        testBuffer.empty(PlayMode::Practice, true) &&
        testBuffer.empty(PlayMode::Practice, false);

    log::info(
        "SUPERBUFFER: Four Queue Test "
        "Enqueue={} Detect={} Isolated={} "
        "Consumed={} Empty={}",
        normalP1 && normalP2 && practiceP1 && practiceP2,
        foundNormalP1 && foundNormalP2 &&
        foundPracticeP1 && foundPracticeP2,
        isolated,
        consumed,
        allEmpty
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

        // Cube ground-state diagnostic.
        if (gameMode == superbuffer::GameMode::Cube) {
            log::info(
                "SUPERBUFFER: Cube Ground State "
                "Player={} Ground={} Ground2={} "
                "JumpBuffered={} Dead={}",
                isPlayer1 ? "P1" : "P2",
                player->m_isOnGround,
                player->m_isOnGround2,
                player->m_jumpBuffered,
                player->m_isDead
            );
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

        const auto queuedCount =
            this->m_queuedButtons.size();

        if (queuedCount > 0) {
            const auto& queued =
                this->m_queuedButtons[queuedCount - 1];

            log::info(
                "SUPERBUFFER: Native Input "
                "Count={} Button={} Push={} P2={} "
                "Step={} Timestamp={}",
                queuedCount,
                static_cast<int>(queued.m_button),
                queued.m_isPush,
                queued.m_isPlayer2,
                queued.m_step,
                queued.m_timestamp
            );
        }
    }

    bool shouldUseSubstepForButton(float dt) {
        const bool result =
            GJBaseGameLayer::shouldUseSubstepForButton(dt);

        if (result || !this->m_queuedButtons.empty()) {
            log::info(
                "SUPERBUFFER: Substep Decision "
                "Result={} DT={} Timestamp={} "
                "Queue={} BetweenSteps={} ClickBetweenSteps={}",
                result,
                dt,
                this->m_timestamp,
                this->m_queuedButtons.size(),
                this->m_isBetweenSteps,
                this->m_clickBetweenSteps
            );
        }

        return result;
    }

    void processCommands(
        float dt,
        bool isHalfTick,
        bool isLastTick
    ) {
        const auto before =
            this->m_queuedButtons.size();

        if (before > 0) {
            log::info(
                "SUPERBUFFER: Command Tick BEFORE "
                "Frame={} DT={} Timestamp={} "
                "HalfTick={} LastTick={} "
                "Count={} BetweenSteps={} ClickBetweenSteps={}",
                frameTiming.getCurrentFrame(),
                dt,
                this->m_timestamp,
                isHalfTick,
                isLastTick,
                before,
                this->m_isBetweenSteps,
                this->m_clickBetweenSteps
            );
        }

        GJBaseGameLayer::processCommands(
            dt,
            isHalfTick,
            isLastTick
        );

        const auto after =
            this->m_queuedButtons.size();

        if (before > 0 || after > 0) {
            log::info(
                "SUPERBUFFER: Command Tick AFTER "
                "Frame={} Timestamp={} Count={}",
                frameTiming.getCurrentFrame(),
                this->m_timestamp,
                after
            );
        }
    }

    void processQueuedButtons(
        float dt,
        bool clearInputQueue
    ) {
        const auto before =
            this->m_queuedButtons.size();

        if (before > 0) {
            log::info(
                "SUPERBUFFER: Queue Process "
                "BEFORE Count={} Clear={}",
                before,
                clearInputQueue
            );

            for (std::size_t i = 0; i < before; i++) {
                const auto& command =
                    this->m_queuedButtons[i];

                log::info(
                    "SUPERBUFFER: Queue Command "
                    "Index={} Button={} Push={} P2={} "
                    "Step={} Timestamp={}",
                    i,
                    static_cast<int>(command.m_button),
                    command.m_isPush,
                    command.m_isPlayer2,
                    command.m_step,
                    command.m_timestamp
                );
            }
        }

        GJBaseGameLayer::processQueuedButtons(
            dt,
            clearInputQueue
        );

        const auto after =
            this->m_queuedButtons.size();

        if (before > 0 || after > 0) {
            log::info(
                "SUPERBUFFER: Queue Process "
                "AFTER Count={} Clear={}",
                after,
                clearInputQueue
            );
        }
    }
};
