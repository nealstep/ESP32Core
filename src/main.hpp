#pragma once

#ifdef ARDUINO
#include <Arduino.h>
#else
#include <cstdint>
#endif  // ARDUINO

#include "version.h"

namespace Config {
static constexpr uint32_t startup_delay = 3000;
static constexpr uint32_t medium_delay = 500;
}  // namespace Config

class Version {
   protected:
    static constexpr const char* const module = "ESP32Core";
    static constexpr const char* const git_version = E32C_GIT_VERSION;
    static constexpr const char* const firmware_version = E32C_FIRMWARE_VERSION;
    static constexpr const char* const build_time = E32C_DATETIME_VERSION;

   public:
    static constexpr const char* get_module(void) { return module; }
    static constexpr const char* get_git_version(void) { return git_version; }
    static constexpr const char* get_firmware_version(void) {
        return firmware_version;
    }
    static constexpr const char* get_build_time(void) { return build_time; }
};

void die(void);
