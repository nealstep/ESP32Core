#include "main.hpp"

#ifdef ARDUINO
#include <Preferences.h>
#include <TaskScheduler.h>
#endif  // ARDUINO

#include "constants.hpp"
#include "e32c_log.hpp"
#include "e32c_net.hpp"
#include "messages.hpp"
#include "prefs.hpp"

void die(void) {
    LOG_E(Log::Uni::Main, Log::Err::Died);
#ifdef ARDUINO
    while (true) {
        delay(Constants::medium_delay);
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

#ifdef ARDUINO
#ifdef LOG_SERIAL
void serial_setup(void) {
    bool no_use_serial;
    bool no_serial_speed;

    Log::Err err = prefs.get_bool(Prefs::Keys::use_serial, prefs.use_serial);
    if (err == Log::Err::NoError) {
        no_use_serial = false;
    } else {
        no_use_serial = true;
        prefs.use_serial = true;
    }
    err = prefs.get_u32(Prefs::Keys::serial_speed, prefs.serial_speed,
                                 Prefs::BadValues::serial_speed);
    if (err == Log::Err::NoError) {
        no_serial_speed = false;
    } else {
        no_serial_speed = true;
        prefs.serial_speed = SERIAL_SPEED;
    }
    if (prefs.use_serial) {
        LOG_SERIAL.begin(prefs.serial_speed);
        if (no_use_serial) {
            // we could do 'use_serial ? "true" : "false"' but why
            LOG_E(Log::Uni::Main, Log::Err::NoPrefS, "true");
        }
        if (no_serial_speed) {
            LOG_E(Log::Uni::Main, Log::Err::NoPrefU, prefs.serial_speed);
        }
    }
}
#endif  // LOG_SERIAL

void setup(void) {
    delay(Constants::startup_delay);
    Log::Err err = prefs.open(Constants::prefs_name, true);
    if (err != Log::Err::NoError) {
        LOG_E(Log::Uni::Main, err);
    }
#ifdef LOG_SERIAL
    serial_setup();
#endif  // LOG_SERIAL
    LOG_N(Log::Uni::Main, Log::Sev::All, Log::Note::Starting);
    get_net_prefs();
    log_version();
    LOG_N(Log::Uni::Main, Log::Sev::Inf, Log::Note::Started);
    prefs.close();
}

void loop(void) { delay(Constants::medium_delay); }

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
