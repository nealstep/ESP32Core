#ifdef ARDUINO
#include <Arduino.h>
#else  // !ARDUINO
#include <iostream>
#endif  // ARDUINO !ARDUINO

#include "e32c_log.hpp"
#include "log_output_impl.hpp"
#include "prefs/prefs.hpp"
#ifdef LOG_UDP
#include "net/e32c_net.hpp"
#endif  // LOG_UDP

void log_output_impl(const char* str, bool error, bool truncated) {
    if (error) {
        // there was a an error expanding the string
#ifdef LOG_SERIAL
        if (prefs.use_serial) LOG_SERIAL.printf("Format!: %s\n", str);
#elifdef LOG_STDOUT
        std::cout << "Format!: " << str << std::endl;
#endif  // LOG_SERIAL LOG_STDOUT
#ifdef LOG_UDP
        esp32Net.log_str(str);
#endif  // LOG_UDP
    } else if (truncated) {
        // the line got truncated
#ifdef LOG_SERIAL
        if (prefs.use_serial) LOG_SERIAL.printf("Trunc!: %s\n", str);
#elifdef LOG_STDOUT
        std::cout << "Trunc!: " << str << std::endl;
#endif  // LOG_SERIAL LOG_STDOUT
#ifdef LOG_UDP
        esp32Net.log_str(str);
#endif  // LOG_UDP
    } else {
        // all is good
#ifdef LOG_SERIAL
        if (prefs.use_serial) LOG_SERIAL.println(str);
#elifdef LOG_STDOUT
        std::cout << str << std::endl;
#endif  // LOG_SERIAL LOG_STDOUT
#ifdef LOG_UDP
        if (str[0] == Constants::brd) {
            //     str[0] = Constants::dat;
            esp32Net.broadcast_str(str);
        }
        esp32Net.log_str(str);
#endif  // LOG_UDP
    }
}
