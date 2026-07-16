
#ifdef ARDUINO
#include <Arduino.h>
#else
#include <cstdlib>
#endif  // ARDUINO

#include "constants.hpp"
#include "e32c_log.hpp"
#include "utils.hpp"

void die(void) {
    LOG_E(Log::Uni::Main, Log::Err::Died);
#ifdef ARDUINO
    while (true) {
        delay(Constants::medium_delay);
    }
#else
    std::exit(EXIT_FAILURE);
#endif  // ARDUINO
}

const char* b2s(bool val) { return val ? "true" : "false"; }