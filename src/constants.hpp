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
static constexpr uint32_t loop_interval = 250000;

// timing information
static constexpr uint32_t sec_ms = 1000UL;
static constexpr uint32_t min_sec = 60UL;
static constexpr uint32_t send_keep_alive_msg_int = 15 * sec_ms;
static constexpr uint32_t check_internet_int = 1 * min_sec * sec_ms;

// sizes
static constexpr size_t chipid_size = 17;
static constexpr size_t timestamp_size = 32;

// formats for time and messages
static constexpr const char* const time_fmt = "%Y-%m-%d@%H:%M:%S-%Z";
static constexpr const char* const log_fmt = "*|%s|%u|%s|%s|%s|%s:%d|";
static constexpr const char* const data_fmt = "@|%s|%u|%s|";

#ifdef ARDUINO_ARCH_ESP32
// namespace for preferences
static constexpr const char* const prefs_name = PREFS_NAME;
#endif

}  // namespace Constants