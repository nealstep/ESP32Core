#pragma once

#ifdef ARDUINO
#include <Arduino.h>
#else
#include <cstdarg>
#include <cstdint>
#include <cstdio>
#endif  // ARDUINO

#include "constants.hpp"
#include "log_output_impl.hpp"
#include "messages.hpp"

#ifndef DEF_UNIT
#define DEF_UNIT Log::Uni::Last
#endif  // DB_UNIT

#ifndef DEF_SEVERITY
#define DEF_SEVERITY Log::Sev::Dbg
#endif  // DB_SVR

class Log {
   public:
    class Config {
       public:
        // sizes
        static constexpr uint16_t max_message_size = 300;

        // ids
        static constexpr uint16_t unit_unset = 0xFFFF;
        static constexpr uint8_t severity_unset = 0xFF;
    };

    enum class Uni : uint32_t {
        Unnamed = 0,
#define AS_ENUM(name, string) name = 1 << __COUNTER__,
        UNIT_LIST(AS_ENUM)
#undef AS_ENUM
    };

#define GENERATE_ENUM(id, msg) id,
    enum class Sev : uint8_t { SEVERITY_LIST(GENERATE_ENUM) Count };
    enum class Err : uint16_t { ERROR_LIST(GENERATE_ENUM) Count };
    enum class Note : uint16_t { NOTICE_LIST(GENERATE_ENUM) Count };
    enum class Data : uint16_t { DATA_LIST(GENERATE_ENUM) Count };
#undef GENERATE_ENUM

    class Word {
       public:
#define AS_CONSTCHAR(name, string) \
    static constexpr const char* const name = string;
        WORD_LIST(AS_CONSTCHAR)
#undef AS_CONSTCHAR
    };

    class Name {
       public:
#define AS_CONSTCHAR(name, string) \
    static constexpr const char* const name = string;
        NAME_LIST(AS_CONSTCHAR)
#undef AS_CONSTCHAR
    };

#define GENERATE_STRING(id, msg) msg,
    static constexpr const char* const Units[] = {"Unamed",
                                                  UNIT_LIST(GENERATE_STRING)};
    static constexpr const char* const Severities[] = {
        SEVERITY_LIST(GENERATE_STRING)};
    static constexpr const char* const Errors[] = {ERROR_LIST(GENERATE_STRING)};
    static constexpr const char* const Notices[] = {
        NOTICE_LIST(GENERATE_STRING)};
    static constexpr const char* const Datas[] = {DATA_LIST(GENERATE_STRING)};
#undef GENERATE_STRING

    // lazy singleton
    static Log& getInstance(void) {
        static Log instance;
        return instance;
    }
    Log(const Log&) = delete;
    Log& operator=(const Log&) = delete;

    // message functions
    constexpr const char* get_message(Uni code) {
        uint32_t un = static_cast<uint32_t>(code);
        if (un == 0) return Units[0];
        uint8_t bit = __builtin_ctz(un);
        if (bit >= max_unit) {
            return Word::Unknown;
        }
        return Units[bit + 1];
    }
    constexpr const char* get_message(Sev code) {
        return Severities[static_cast<uint8_t>(code)];
    }
    constexpr const char* get_message(Err code) {
        return Errors[static_cast<uint16_t>(code)];
    }
    constexpr const char* get_message(Note code) {
        return Notices[static_cast<uint16_t>(code)];
    }
    constexpr const char* get_message(Data code) {
        return Datas[static_cast<uint16_t>(code)];
    }

    bool inCode(Uni item, Uni code) {
        return (static_cast<uint32_t>(item) & static_cast<uint32_t>(code)) != 0;
    }

    bool get_timestamp(char* buffer, size_t len) {
        struct tm timeinfo;
        bool overrun = false;
        if (!getLocalTime(&timeinfo)) {
            size_t sz = strlcpy(buffer, Log::Word::NoTime, len);
            if (sz >= len) overrun = true;
        } else {
            size_t sz = strftime(buffer, len, Constants::time_fmt, &timeinfo);
            if (sz >= len) overrun = true;
        }
        return overrun;
    }

    void vlog_impl(const char* file, int line, Uni unit, Sev svr,
                   const char* str, va_list args) {
        if (svr != Sev::All) {
            if (svr < severity) return;
            if (unit != Uni::Unnamed)
                if (!inCode(unit, unit_mask)) return;
        }
        char ts[Constants::timestamp_size];
        char buffer[Config::max_message_size];
        bool truncated = false;
        bool error = false;

        truncated = get_timestamp(ts, sizeof(ts));
        int len = snprintf(buffer, sizeof(buffer), Constants::log_fmt,
                           Constants::log, chipid_s, msgid, ts,
                           get_message(unit), get_message(svr), file, line);
        msgid++;
        // reserve msgid 0
        if (msgid == 0) msgid = 1;
        if (len < 0) {
            error = true;
        } else if (len < sizeof(buffer)) {
            len = vsnprintf(buffer + len, sizeof(buffer) - len, str, args);
            if (len < 0) {
                error = true;
            } else if (len >= sizeof(buffer)) {
                truncated = true;
            }
        } else {
            truncated = true;
        }
        if (error)
            log_output_impl(str, error, truncated);
        else
            log_output_impl(buffer, error, truncated);
    }

    void data_impl(char id, Data code, ...) {
        char ts[Constants::timestamp_size];
        char buffer[Config::max_message_size];
        bool truncated = false;
        bool error = false;

        truncated = get_timestamp(ts, sizeof(ts));
        int len = snprintf(buffer, sizeof(buffer), Constants::data_fmt, id,
                           chipid_s, msgid, ts);
        msgid++;
        // reserve msgid 0
        if (msgid == 0) msgid = 1;
        if (len < 0) {
            error = true;
        } else if (len < sizeof(buffer)) {
            va_list(args);
            va_start(args, code);
            len = vsnprintf(buffer + len, sizeof(buffer) - len,
                            get_message(code), args);
            va_end(args);
            if (len < 0) {
                error = true;
            } else if (len >= sizeof(buffer)) {
                truncated = true;
            }
        } else {
            truncated = true;
        }
        if (error)
            log_output_impl(get_message(code), error, truncated);
        else
            log_output_impl(buffer, error, truncated);
    }

    void log_impl(const char* file, int line, Uni unit, Sev svr,
                  const char* str, ...) {
        va_list(args);
        va_start(args, str);
        vlog_impl(file, line, unit, svr, str, args);
        va_end(args);
    }

    void log_notice_impl(const char* file, int line, Uni unit, Sev svr,
                         Note code, ...) {
        va_list(args);
        va_start(args, code);
        vlog_impl(file, line, unit, svr, get_message(code), args);
        va_end(args);
    }

    void log_error_impl(const char* file, int line, Uni unit, Err code, ...) {
        va_list(args);
        va_start(args, code);
        vlog_impl(file, line, unit, Sev::Err, get_message(code), args);
        va_end(args);
    }

    void set_unit_mask(Uni code) {
        unit_mask = code;
        if (unit_mask == Uni::Last) {
            // set to last means all so -1 is the mask
            unit_mask = static_cast<Uni>(static_cast<uint32_t>(Uni::Last) - 1);
        }
    }
    Uni get_unit_mask(void) { return unit_mask; }
    void set_severity(Sev code) { severity = code; }
    Sev get_severity(void) { return severity; }
    uint32_t get_msgid(void) { return msgid; }

   private:
    // settings
    Uni unit_mask;
    uint8_t max_unit;
    Sev severity;

    // ids
    uint64_t chipid = ESP.getEfuseMac();
    uint32_t msgid = 0;
    char chipid_s[Constants::chipid_size];

    // hidden creator
    Log(void) {
        max_unit = __builtin_ctz(static_cast<uint32_t>(Uni::Last));
        set_unit_mask(DEF_UNIT);
        set_severity(DEF_SEVERITY);
        // get unique name for chip
        snprintf(chipid_s, sizeof(chipid_s), "%04X%08X",
                 (uint16_t)(chipid >> 32), (uint32_t)chipid);
        // get likely unique message id
        msgid = esp_random();
        if (msgid == 0) msgid = 1;  // reserve msgid 0
    }
};

static Log& lg = Log::getInstance();

#define LOG(unit, svr, fmt, ...) \
    lg.log_impl(__FILE__, __LINE__, unit, svr, fmt, ##__VA_ARGS__)
#define LOG_N(unit, svr, code, ...) \
    lg.log_notice_impl(__FILE__, __LINE__, unit, svr, code, ##__VA_ARGS__)
#define LOG_E(unit, code, ...) \
    lg.log_error_impl(__FILE__, __LINE__, unit, code, ##__VA_ARGS__)
#define DATA(id, code, ...) lg.data_impl(id, code, ##__VA_ARGS__)
