#pragma once

#ifdef ARDUINO
#include "Arduino.h"
#else
#include <cstdint>
#endif  // ARDUINO

#define MODULE_LIST(X) \
    X(Dummy, "Dummy")  \
    X(Last, "Last")

class Modules {
   public:
    enum class Types : uint8_t { sensor = 0, control };

    enum class Mod : uint32_t {
        Unnamed = 0,
#define AS_ENUM(name, string) name = 1 << __COUNTER__,
        MODULE_LIST(AS_ENUM)
#undef AS_ENUM
    };

#define GENERATE_STRING(id, msg) msg,
    static constexpr const char* const Units[] = {"Unamed",
                                                  MODULE_LIST(GENERATE_STRING)};
#undef GENERATE_STRING

    // lazy singleton
    static Modules& getInstance(void) {
        static Modules instance;
        return instance;
    }
    Modules(const Modules&) = delete;
    Modules& operator=(const Modules&) = delete;

    constexpr const char* get_message(Mod code) {
        uint32_t un = static_cast<uint32_t>(code);
        if (un == 0) return Units[0];
        uint8_t bit = __builtin_ctz(un);
        if (bit >= max_unit) {
            return "Unknown";
        }
        return Units[bit + 1];
    }

    bool inMod(Mod item, Mod code) {
        return (static_cast<uint32_t>(item) & static_cast<uint32_t>(code)) != 0;
    }

   protected:
    uint8_t max_unit;

    // hidden creator
    Modules(void) {
        max_unit = __builtin_ctz(static_cast<uint32_t>(Mod::Last));
    }
};

static Modules& mods = Modules::getInstance();

// TODO: #10 Work on how to implement modules
class Module {
   public:
    const char *get_name(void);
    uint8_t get_value(void);
    bool set_value(uint8_t val);
   protected:
    Modules::Types type;
    const char* name;
    uint8_t id;
};
