#pragma once

#ifdef ARDUINO
#include "Arduino.h"
#else
#include <cstdint>
#endif  // ARDUINO

#include "constants.hpp"
#include "log/e32c_log.hpp"

#define TYPES_LIST(X) \
    X(Sensor, "S")    \
    X(Control, "C")

class Modules {
   public:
#define GENERATE_ENUM(id, msg) id,
    enum class Typ : uint8_t { TYPES_LIST(GENERATE_ENUM) Count };
#undef GENERATE_ENUM

#define GENERATE_STRING(id, msg) msg,
    static constexpr const char* const Types[] = {TYPES_LIST(GENERATE_STRING)};
#undef GENERATE_STRING

    // lazy singleton
    static Modules& getInstance(void) {
        static Modules instance;
        return instance;
    }
    Modules(const Modules&) = delete;
    Modules& operator=(const Modules&) = delete;

    static constexpr const char* get_name(Typ code) {
        return Types[static_cast<uint8_t>(code)];
    }

   protected:
    // hidden creator
    Modules(void) {}
};

static Modules& mods = Modules::getInstance();

class Module {
   public:
    const char* get_name(void) { return name; }
    Module(Modules::Typ _type, const char* _kind, uint8_t _loc, uint8_t _id) {
        type = _type;
        loc = _loc;
        id = _id;
        size_t len = snprintf(name, sizeof(name), "%s.%s.%u.%u",
                              Modules::get_name(type), _kind, loc, id);
        if (len >= sizeof(name)) {
            LOG_E(Log::Uni::Mod, Log::Err::StringTooBig, "module create");
        }
    }

   protected:
    char name[Constants::name_size];
    Modules::Typ type;
    uint8_t loc;
    uint8_t id;
};
