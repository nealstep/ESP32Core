#pragma once

#include "modules.hpp"

#ifdef M_S_DUMMY

class M_S_Dummy : public Module {
   public:
    static constexpr const char* const kind = "dummy";
    static constexpr const char* const s1 = "S1";

    M_S_Dummy(uint8_t _loc, uint8_t _id)
        : Module(Modules::Typ::Sensor, kind, _loc, _id) {}

    uint8_t get_s1(void) {
        uint8_t value = random(0, 100);
        LOG_N(Log::Uni::Mod, Log::Sev::Dbg, Log::Note::SimpleValueUInt, s1,
              value);
        return value;
    }
};

#endif  // M_S_DUMMY
