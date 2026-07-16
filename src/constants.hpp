#pragma once

#ifdef ARDUINO
#include <Arduino.h>
#else
#include <cstdint>
#endif  // ARDUINO

namespace Constants {
static constexpr uint32_t startup_delay = 3000;
static constexpr uint32_t long_delay = 1000;
static constexpr uint32_t medium_delay = 500;

#ifdef ARDUINO_ARCH_ESP32
// namespace for preferences
static constexpr const char* const prefs_name = PREFS_NAME;
#endif

}  // namespace Constants