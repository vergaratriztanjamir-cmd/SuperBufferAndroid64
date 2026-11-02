
#include <Geode/Geode.hpp>

using namespace geode::prelude;

$on_mod(Loaded) {
    log::info("==============================");
    log::info("SUPERBUFFER INITIALIZED!");
    log::info("Version: 0.1.0");
    log::info("Platform target: Android64");
    log::info("Initialization successful.");
    log::info("==============================");
}
