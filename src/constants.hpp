#pragma once

#ifdef ARDUINO
#include <Arduino.h>
#else
#include <cstdint>
#endif  // ARDUINO

namespace Constants {
// delays
static constexpr uint32_t startup_delay = 3000;
static constexpr uint32_t long_delay = 1000;
static constexpr uint32_t medium_delay = 500;
static constexpr uint32_t tiny_delay = 5;
static constexpr uint32_t loop_interval = 250000;

// timing information
static constexpr uint32_t sec_ms = 1000UL;
static constexpr uint32_t min_sec = 60UL;
static constexpr uint32_t send_keep_alive_msg_int = 15 * sec_ms;
static constexpr uint32_t check_internet_int = 1 * min_sec * sec_ms;
static constexpr uint32_t udp_loop_delay = 200;

// sizes
static constexpr size_t chipid_size = 17;
static constexpr size_t timestamp_size = 32;
static constexpr size_t name_size = 32;
static constexpr size_t kind_size = 16;
static constexpr size_t bytes_kb = 1024;
static constexpr uint8_t command_q = 10;
static constexpr size_t command_size = 120;

// ips
static constexpr uint32_t BroadAll = 0xFFFFFFFF; 

// numbers
static constexpr uint32_t million = 1000000;

// message codes
static constexpr const char log = '*';
static constexpr const char brd = '^';
static constexpr const char dat = '@';

// formats for time and messages
static constexpr const char* const time_fmt = "%Y-%m-%d@%H:%M:%S-%Z";
static constexpr const char* const log_fmt = "%c|%s|%u|%s|%s|%s|%s:%d|";
static constexpr const char* const data_fmt = "%c|%s|%u|%s|";

#ifdef ARDUINO_ARCH_ESP32
// namespace for preferences
static constexpr const char* const prefs_name = PREFS_NAME;
#endif

}  // namespace Constants