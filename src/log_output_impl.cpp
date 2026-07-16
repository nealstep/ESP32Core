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
        if (prefs.use_serial) LOG_SERIAL.printf("Format!: %s\n", str);
#elifdef LOG_STDOUT
        std::cout << "Format!: " << str << std::endl;
#endif  // LOG_SERIAL LOG_STDOUT
    } else if (truncated) {
        // the line got truncated
#ifdef LOG_SERIAL
        if (prefs.use_serial) LOG_SERIAL.printf("Trunc!: %s\n", str);
#elifdef LOG_STDOUT
        std::cout << "Trunc!: " << str << std::endl;
#endif  // LOG_SERIAL LOG_STDOUT
    } else {
        // all is good
#ifdef LOG_SERIAL
        if (prefs.use_serial) LOG_SERIAL.println(str);
#elifdef LOG_STDOUT
        std::cout << str << std::endl;
#endif  // LOG_SERIAL LOG_STDOUT
    }
}
