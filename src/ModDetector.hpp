
#pragma once

#include <Geode/Geode.hpp>
#include "FrameTiming.hpp"

namespace superbuffer {

class ModeDetector {
public:
    static GameMode detect(PlayerObject* player) {

        if (player == nullptr) {
            return GameMode::Count;
        }

        if (player->m_isShip) {
            return GameMode::Ship;
        }

        if (player->m_isBall) {
            return GameMode::Ball;
        }

        if (player->m_isBird) {
            return GameMode::UFO;
        }

        if (player->m_isDart) {
            return GameMode::Wave;
        }

        if (player->m_isRobot) {
            return GameMode::Robot;
        }

        if (player->m_isSpider) {
            return GameMode::Spider;
        }

        if (player->m_isSwing) {
            return GameMode::Swing;
        }

        return GameMode::Cube;
    }
};

}
