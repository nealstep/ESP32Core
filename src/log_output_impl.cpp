#include <e32c_log.hpp>
#include <prefs.hpp>

#ifdef ARDUINO
#include <Arduino.h>
#else  // !ARDUINO
#include <iostream>
#endif  // ARDUINO !ARDUINO

void log_output_impl(const char* str, bool error, bool truncated) {
    if (error) {
        // there was a an error expanding the string
#ifdef LOG_SERIAL
        if (use_serial) LOG_SERIAL.printf("Format!: %s\n", str);
#elifdef LOG_STDERR
        std::cerr << "Format!: " << str << std::endl;
#endif  // LOG_SERIAL LOG_STDERR
    } else if (truncated) {
        // the line got truncated
#ifdef LOG_SERIAL
        if (use_serial) LOG_SERIAL.printf("Trunc!: %s\n", str);
#elifdef LOG_STDERR
        std::cerr << "Trunc!: " << str << std::endl;
#endif  // LOG_SERIAL LOG_STDERR
    } else {
        // all is good
#ifdef LOG_SERIAL
        if (use_serial) LOG_SERIAL.println(str);
#elifdef LOG_STDERR
        std::cerr << str << std::endl;
#endif  // LOG_SERIAL LOG_STDERR
    }
}
