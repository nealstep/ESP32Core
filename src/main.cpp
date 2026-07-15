#include "main.hpp"

#ifdef ARDUINO
#include <Preferences.h>
#include <TaskScheduler.h>
#endif  // ARDUINO

#include "e32c_log.hpp"
#include "messages.hpp"
#include "prefs.hpp"
#include "vars.hpp"

#ifdef ARDUINO
Preferences preferences;
#endif  // ARDUINO

void die(void) {
    LOG_E(Log::Uni::Main, Log::Err::Died);
#ifdef ARDUINO
    while (true) {
        delay(Config::medium_delay);
    }
#endif  // ARDUINO
}

void log_version(void) {
    LOG_N(Log::Uni::Main, Log::Sev::Inf, Log::Note::SimpleValueStr,
          "E32C git version", Version::get_git_version());
    LOG_N(Log::Uni::Main, Log::Sev::Inf, Log::Note::SimpleValueStr,
          "E32C firmware version", Version::get_firmware_version());
    LOG_N(Log::Uni::Main, Log::Sev::Inf, Log::Note::SimpleValueStr,
          "E32C build time", Version::get_build_time());
}

#ifdef LOG_SERIAL
void serial_setup() {
    bool no_use_serial;
    bool no_serial_speed;
    if (preferences.isKey(k_use_serial)) {
        use_serial = preferences.getBool(k_use_serial);
        no_use_serial = false;
    } else {
        no_use_serial = true;
        use_serial = true;
    }
    serial_speed = preferences.getUInt(k_serial_speed, b_serial_speed);
    if (serial_speed == b_serial_speed) {
        serial_speed = SERIAL_SPEED;
        no_serial_speed = true;
    } else {
        no_serial_speed = false;
    }
    if (use_serial) {
        LOG_SERIAL.begin(serial_speed);
        if (no_use_serial) {
            // we could do 'use_serial ? "true" : "false"' but why
            LOG_E(Log::Uni::Main, Log::Err::NoPrefS, "true");
        }
        if (no_serial_speed) {
            LOG_E(Log::Uni::Main, Log::Err::NoPrefU, serial_speed);
        }
    }
}
#endif  // LOG_SERIAL

#ifdef ARDUINO

void setup() {
    delay(Config::startup_delay);
#if defined(ARDUINO_ARCH_ESP32)
    bool succ = preferences.begin(namespace_name, true);
    if (!succ) {
#ifdef LOG_SERIAL
        LOG_SERIAL.begin(SERIAL_SPEED);
        LOG_SERIAL.println(
            "No Preferences: Have you run ESP32Core-Preferences");
#endif  // LOG_SERIAL
        die();
    }
#ifdef LOG_SERIAL
    serial_setup();
#endif  // LOG_SERIAL
#endif  // ARDUINO_ARCH_ESP32
    LOG_N(Log::Uni::Main, Log::Sev::All, Log::Note::Starting);
    log_version();
    LOG_N(Log::Uni::Main, Log::Sev::Inf, Log::Note::Started);
}

void loop() { delay(Config::medium_delay); }

#else  // !ARDUINO

#include <iostream>

int main(int argc, char* argv[]) {
    std::cout << "Starting" << std::endl;
    LOG_N(Log::Uni::Unnamed, Log::Sev::All, Log::Note::Starting);
    log_version();
    LOG_N(Log::Uni::Main, Log::Sev::Err, Log::Note::Started);
    LOG_N(Log::Uni::Main, Log::Sev::Err, Log::Note::Done);
    std::cout << "Done" << std::endl;
}

#endif  // ARDUINO !ARDUINO
