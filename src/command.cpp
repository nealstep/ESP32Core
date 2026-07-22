#ifdef ARDUINO
#include <Arduino.h>
#include <Preferences.h>
#endif  // ARDUINO

#include "command.hpp"

// globals
char commands[Constants::command_q][Constants::command_size];
uint8_t command_index = 0;
uint8_t command_last = 0;

void command_handler(void) {
    while (command_last != command_index) {
#ifdef LOG_SERIAL
        LOG_SERIAL.println(commands[command_last]);
#endif  // LOG_SERIAL

        switch (commands[command_last][0]) {
            case Commands::PrintPrefs:
                break;
            default:
                break;
        }
        if (++command_last >= Constants::command_q) {
            command_last = 0;
        }
    }
}
