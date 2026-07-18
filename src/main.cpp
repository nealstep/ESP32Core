#ifdef ARDUINO
#include <Preferences.h>
#include <TaskScheduler.h>
#endif  // ARDUINO

#ifdef IS_M5
#include <M5Unified.h>
#endif  // IS_M5

#include "constants.hpp"
#include "log/e32c_log.hpp"
#include "log/messages.hpp"
#include "modules/modules.hpp"
#include "net/e32c_net.hpp"
#include "prefs/prefs.hpp"
#include "utils/utils.hpp"
#include "version.hpp"

#ifdef M_S_DUMMY
#include "modules/m_s_dummy.hpp"
M_S_Dummy m_s_dummy_1_1(1, 1);
constexpr uint32_t m_s_dummy_1_1_int =
    1 * Constants::min_sec * Constants::sec_ms;

void m_s_dummy_1_1_chk(void) {
    DATA(Constants::brd, Log::Data::SU, m_s_dummy_1_1.get_name(),
         m_s_dummy_1_1.s1, m_s_dummy_1_1.get_s1());
}
#endif  // M_S_DUMMY

uint32_t loop_counter = 0;

#ifdef ARDUINO

// scheduler
Scheduler runner;

// task wrappers
void keepAliveMsg(void) {
    LOG_N(Log::Uni::Main, Log::Sev::Inf, Log::Note::KeepAlive);
}

void checkInternet(void) { esp32Net.check_internet(); }

// create tasks
Task taskSendKeepAliveMsg(prefs.keep_alive_int, TASK_FOREVER, &keepAliveMsg);
Task taskCheckInternet(prefs.check_internet_int, TASK_FOREVER, &checkInternet);

#ifdef M_S_DUMMY
Task taskMSDummy_1_1(m_s_dummy_1_1_int, TASK_FOREVER, &m_s_dummy_1_1_chk);
#endif  // M_S_DUMMY

// array of tasks to be added to the scheduler
Task* tasks[] = {&taskSendKeepAliveMsg,
#ifdef M_S_DUMMY
                 &taskMSDummy_1_1,
#endif  // M_S_DUMMY
                 &taskCheckInternet};

#endif  // ARDUINO

void log_version(void) {
    LOG_N(Log::Uni::Main, Log::Sev::Inf, Log::Note::SimpleValueStr,
          "E32C git version", Version::get_git_version());
    LOG_N(Log::Uni::Main, Log::Sev::Inf, Log::Note::SimpleValueStr,
          "E32C firmware version", Version::get_firmware_version());
    LOG_N(Log::Uni::Main, Log::Sev::Inf, Log::Note::SimpleValueStr,
          "E32C build time", Version::get_build_time());
    LOG_N(Log::Uni::Main, Log::Sev::Inf, Log::Note::SimpleValueStr,
          "E32C build id", Version::get_build_id());
}

void log_diagnostics(void) {
    LOG_N(Log::Uni::Main, Log::Sev::All, Log::Note::SimpleValueUInt,
          "ESP32 CPU Frequency (Mhz)", ESP.getCpuFreqMHz());
    LOG_N(Log::Uni::Main, Log::Sev::All, Log::Note::SimpleValueUInt,
          "ESP32 Flash Speed (Mhz)",
          ESP.getFlashChipSpeed() / Constants::million);
    LOG_N(Log::Uni::Main, Log::Sev::All, Log::Note::SimpleValueUInt,
          "ESP32 Free Heap (bytes)", ESP.getFreeHeap());
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
    get_pref_str(Prefs::Keys::chip_name, prefs.chip_name,
                 Prefs::Sizes::chip_name, Prefs::BadValues::chip_name, true,
                 true, "MyChip");
    // TODO: #9 Remove workaround chip name
    get_pref_u32(Prefs::Keys::keep_alive_int, prefs.keep_alive_int,
                 Prefs::BadValues::keep_alive_int);
    taskSendKeepAliveMsg.setInterval(prefs.keep_alive_int);
    get_pref_u32(Prefs::Keys::check_internet_int, prefs.check_internet_int,
                 Prefs::BadValues::check_internet_int);
    taskCheckInternet.setInterval(prefs.check_internet_int);
}

void setup(void) {
    delay(Constants::startup_delay);

#ifdef IS_M5
    auto cfg = M5.config();
    M5.begin(cfg);
#endif  // IS_M5

    // get preferences ready
    Log::Err err = prefs.open(Constants::prefs_name, true);
    if (err != Log::Err::NoError) {
        LOG_E(Log::Uni::Main, err);
    }

    // startup serial, if required and issue start message
#ifdef LOG_SERIAL
    serial_setup();
#endif  // LOG_SERIAL
    esp32Net.early_init();
    LOG_N(Log::Uni::Main, Log::Sev::All, Log::Note::Starting);
    DATA(Constants::dat, Log::Data::U, Log::Name::MsgID, lg.get_msgid());

    // load preferences
    get_main_prefs();
    DATA(Constants::dat, Log::Data::Str, Log::Name::ChipName, prefs.chip_name);
    get_net_prefs();
    prefs.close();

    // display version
    log_version();
    log_diagnostics();

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
                DATA(Constants::dat, Log::Data::Str, Log::Name::IP,
                     esp32Net.get_ip().toString().c_str());
                LOG_N(Log::Uni::Main, Log::Sev::All, Log::Note::Broadcast,
                      esp32Net.broadcastIP.toString().c_str());
                esp32Net.set_ip_ready();
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

#ifdef LOG_SERIAL
void serial_in_check() {
    // TODO: #13 Handle serial input
}
#endif  // LOG_SERIAL

void command_handler(void) {
    // TODO: #14 Check if we have commands to handle and handle them
    // amongst these will be updating and saving to prefs
}

void loop(void) {
    if (loop_counter > Constants::loop_interval) {
        LOG_N(Log::Uni::Main, Log::Sev::Inf, Log::Note::LoopedN, loop_counter);
        loop_counter = 0;
    }

#ifdef LOG_SERIAL
    serial_in_check();
#endif  // LOG_SERIAL
    command_handler();

    // check if anything needs handling
    events_check();
    ota_check();
#ifdef IS_M5
    updateM5();
#endif

    // check if jobs need running
    runner.execute();

    loop_counter++;
}

#endif  // ARDUINO
