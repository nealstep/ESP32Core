#ifdef ARDUINO
#include <Preferences.h>
#include <TaskScheduler.h>
#endif  // ARDUINO

#ifdef IS_M5
#include <M5Unified.h>
#endif  // IS_M5

#include "constants.hpp"
#include "e32c_log.hpp"
#include "e32c_net.hpp"
#include "messages.hpp"
#include "prefs.hpp"
#include "utils.hpp"
#include "version.hpp"

namespace Global {
uint32_t loop_counter = 0;
}

// scheduler
Scheduler runner;

// task wrappers
void keepAliveMsg() {
    LOG_N(Log::Uni::Main, Log::Sev::Inf, Log::Note::KeepAlive);
}

// create tasks
Task taskSendKeepAliveMsg(prefs.keep_alive_int, TASK_FOREVER, &keepAliveMsg);

// array of tasks to be added to the scheduler
Task* tasks[] = {&taskSendKeepAliveMsg};



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
        prefs.use_serial = Prefs::Defaults::use_serial;
    }
    err = prefs.get_u32(Prefs::Keys::serial_speed, prefs.serial_speed,
                        Prefs::BadValues::serial_speed);
    if (err == Log::Err::NoError) {
        no_serial_speed = false;
    } else {
        no_serial_speed = true;
        prefs.serial_speed = Prefs::Defaults::serial_speed;
    }
    if (prefs.use_serial) {
        LOG_SERIAL.begin(prefs.serial_speed);
        if (no_use_serial) {
            LOG_E(Log::Uni::Main, Log::Err::NoPrefS, Prefs::Keys::use_serial);
        } else {
            LOG_N(Log::Uni::Main, Log::Sev::Inf, Log::Note::PrefReadS,
                  Prefs::Keys::use_serial, b2s(prefs.use_serial));
        }
        if (no_serial_speed) {
            LOG_E(Log::Uni::Main, Log::Err::NoPrefU, Prefs::Keys::serial_speed);
        } else {
            LOG_N(Log::Uni::Main, Log::Sev::Inf, Log::Note::PrefReadU,
                  Prefs::Keys::serial_speed, prefs.serial_speed);
        }
    }
}
#endif  // LOG_SERIAL

void get_main_prefs() {
    get_pref_u32(Prefs::Keys::keep_alive_int, prefs.keep_alive_int, Prefs::BadValues::keep_alive_int);
    taskSendKeepAliveMsg.setInterval(prefs.keep_alive_int);
}

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
    get_main_prefs();
    get_net_prefs();
    log_version();
    // enable and start all tasks
    for (auto& task : tasks) {
        runner.addTask(*task);
        task->enable();
    }
    LOG_N(Log::Uni::Main, Log::Sev::Inf, Log::Note::Started);
    prefs.close();
}

#ifdef IS_M5
// M5 updates, button handling etc may also go here
void updateM5(void) { M5.update(); }
#endif

void loop(void) {
    if (Global::loop_counter > Constants::loop_interval) {
        LOG_N(Log::Uni::Main, Log::Sev::Inf, Log::Note::LoopedN,
              Global::loop_counter);
        Global::loop_counter = 0;
    }
#ifdef IS_M5
    updateM5();
#endif
    runner.execute();
    Global::loop_counter++;
}

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
