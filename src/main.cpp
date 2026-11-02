
#include <Geode/Geode.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>

using namespace geode::prelude;

class $modify(GJBaseGameLayer) {
    void handleButton(bool down, int button, bool isPlayer1) {

        log::info(
            "SUPERBUFFER INPUT DETECTED: {} | Button: {} | Player: {}",
            down ? "PRESSED" : "RELEASED",
            button,
            isPlayer1 ? "P1" : "P2"
        );

        GJBaseGameLayer::handleButton(
            down,
            button,
            isPlayer1
        );
    }
};
