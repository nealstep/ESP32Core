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

#ifdef ARDUINO

// scheduler
Scheduler runner;

// task wrappers
void keepAliveMsg() {
    LOG_N(Log::Uni::Main, Log::Sev::Inf, Log::Note::KeepAlive);
}
void checkInternet() { esp32Net.check_internet(); }

// create tasks
Task taskSendKeepAliveMsg(prefs.keep_alive_int, TASK_FOREVER, &keepAliveMsg);
Task taskCheckInternet(prefs.check_internet_int, TASK_FOREVER, &checkInternet);

// array of tasks to be added to the scheduler
Task* tasks[] = {&taskSendKeepAliveMsg, &taskCheckInternet};

#endif  // ARDUINO

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
    get_pref_u32(Prefs::Keys::keep_alive_int, prefs.keep_alive_int,
                 Prefs::BadValues::keep_alive_int);
    taskSendKeepAliveMsg.setInterval(prefs.keep_alive_int);
    get_pref_u32(Prefs::Keys::check_internet_int, prefs.check_internet_int,
                 Prefs::BadValues::check_internet_int);
    taskCheckInternet.setInterval(prefs.check_internet_int);
}

// TODO: #7 add a device name (internal) and a pretty name (prefs)
void setup(void) {
    delay(Constants::startup_delay);

    // get preferences ready
    Log::Err err = prefs.open(Constants::prefs_name, true);
    if (err != Log::Err::NoError) {
        LOG_E(Log::Uni::Main, err);
    }

    // startup serial, if required and issue start message
#ifdef LOG_SERIAL
    serial_setup();
#endif  // LOG_SERIAL
    LOG_N(Log::Uni::Main, Log::Sev::All, Log::Note::Starting);

    // load preferences
    get_main_prefs();
    get_net_prefs();
    prefs.close();

    // display version
    log_version();

    // start network
    err = esp32Net.init();
    if (err != Log::Err::NoError) {
        LOG_E(Log::Uni::Main, err);
        die();
    }

    // enable and start all tasks
    for (auto& task : tasks) {
        runner.addTask(*task);
        task->enable();
    }

    // we are ready
    LOG_N(Log::Uni::Main, Log::Sev::Inf, Log::Note::Started);
}

// handle network events from queue
void events_check(void) {
    ESP32Net::NetMessage netMsg;
    if (xQueueReceive(esp32Net.netQueue, &netMsg, 0) == pdTRUE) {
        switch (netMsg.type) {
            case ESP32Net::NetMessage::Type::Connected:
                LOG_N(Log::Uni::Main, Log::Sev::All, Log::Note::Connected);
                break;
            case ESP32Net::NetMessage::Type::GotIP:
                LOG_N(Log::Uni::Main, Log::Sev::All, Log::Note::GotIP,
                      esp32Net.get_ip().toString().c_str());
                LOG_N(Log::Uni::Main, Log::Sev::All, Log::Note::Broadcast,
                      esp32Net.broadcastIP.toString().c_str());
                esp32Net.check_internet();
                break;
            case ESP32Net::NetMessage::Type::Disconnected:
                LOG_N(Log::Uni::Main, Log::Sev::All, Log::Note::Disconnected);
                break;
            case ESP32Net::NetMessage::Type::TimeSynced:
                LOG_N(Log::Uni::Main, Log::Sev::All, Log::Note::TimeSynced);
                if (HAVE_RTC) {
                    struct tm timeinfo;
                    if (getLocalTime(&timeinfo)) {
#ifdef IS_M5
                        M5.Rtc.setDateTime(&timeinfo);
#endif  // IS_M5
                    }
                }
                break;
            case ESP32Net::NetMessage::Type::InternetConnected:
                LOG_N(Log::Uni::Main, Log::Sev::All,
                      Log::Note::InternetConnected);
                break;
            case ESP32Net::NetMessage::Type::NoInternet:
                LOG_E(Log::Uni::Main, Log::Err::NoInternet);
                break;
            case ESP32Net::NetMessage::Type::TimeSyncFailed:
                LOG_E(Log::Uni::Main, Log::Err::TimeSyncFailed);
                break;
            default:
                LOG_E(Log::Uni::Main, Log::Err::UnknownMessage);
                break;
        }
    }
    ESP32Net::UDPMessage udpMsg;
    if (xQueueReceive(esp32Net.udpQueue, &udpMsg, 0) == pdTRUE) {
        LOG_N(Log::Uni::Main, Log::Sev::All, Log::Note::ReceivedUDPPacket,
              udpMsg.remoteIP.toString().c_str(), b2s(udpMsg.broadcast),
              udpMsg.data);
        // TODO: #5 handle_message(udpMsg.data, udpMsg.remoteIP);
    }
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

    // check if anything needs handling
    events_check();
    ota_check();
#ifdef IS_M5
    updateM5();
#endif

    // check if jobs need running
    runner.execute();

    Global::loop_counter++;
}

#else  // !ARDUINO

#include <iostream>

#ifndef PIO_UNIT_TESTING

void test(void) {
    uint16_t times = 0;
    LOG_N(Log::Uni::Unnamed, Log::Sev::All, Log::Note::Starting);
    log_version();
    std::cout << std::hex
              << "Unit: " << static_cast<uint32_t>(lg.get_unit_mask())
              << std::endl;
    std::cout << "Sev: " << static_cast<uint16_t>(lg.get_severity())
              << std::endl;
    std::cout << std::dec << "loops: " << Global::loop_counter << std::endl;
    std::cout << "times: " << times << std::endl;
    while (times < 3) {
        if (Global::loop_counter > Constants::loop_interval) {
            LOG_N(Log::Uni::Main, Log::Sev::Inf, Log::Note::LoopedN,
                  Global::loop_counter);
            Global::loop_counter = 0;
            times++;
        }
        Global::loop_counter++;
    }
    LOG_E(Log::Uni::Main, Log::Err::UnexpectedError);
    LOG(Log::Uni::Main, Log::Sev::Wrn, "A test %d", 45);
    std::cout << "loops: " << Global::loop_counter << std::endl;
    std::cout << "times: " << times << std::endl;
    LOG_N(Log::Uni::Main, Log::Sev::Inf, Log::Note::Done);
}

int main(int argc, char* argv[]) {
    std::cout << "Starting" << std::endl;
    log_version();

    std::cout << std::endl << "Everything" << std::endl;
    lg.set_unit_mask(Log::Uni::Last);
    lg.set_severity(Log::Sev::Dbg);
    test();
    std::cout << std::endl << "Only Errors+" << std::endl;
    lg.set_severity(Log::Sev::Err);
    test();

    std::cout << std::endl << "Warnings+ from Main" << std::endl;
    lg.set_unit_mask(Log::Uni::Main);
    lg.set_severity(Log::Sev::Wrn);
    test();

    // restore to full
    lg.set_unit_mask(Log::Uni::Last);
    lg.set_severity(Log::Sev::Dbg);

    // basic circular queue test

#if USE_QUEUE
    std::cout << std::endl << "Circual Queue" << std::endl;
    static constexpr size_t test_buffer_size = 10;
    static constexpr uint16_t test_entries = 3;
    CircularQueue circularQueue(test_buffer_size, test_entries);
    LOG_N(Log::Uni::Main, Log::Sev::Inf, Log::Note::Starting);
    LOG_N(Log::Uni::Main, Log::Sev::Inf, Log::Note::FreeCapacity,
          circularQueue.get_free_capacity());
    LOG_N(Log::Uni::Main, Log::Sev::Inf, Log::Note::FreeEntries,
          circularQueue.get_free_entries());
#endif  // USE_QUEUE

    LOG_N(Log::Uni::Main, Log::Sev::Inf, Log::Note::Done);
    std::cout << std::endl << "Done" << std::endl;
}
#endif  // PIO_UNIT_TESTING

#endif  // ARDUINO !ARDUINO
