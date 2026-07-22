#include "constants.hpp"

namespace Commands {
static constexpr const char PrintPrefs = 'P';
static constexpr const char SetPref = 'S';
static constexpr const char GetPref = 'G';
static constexpr const char WritePref = 'W';
static constexpr const char ReadPPref = 'W';
static constexpr const char PrintMods = 'p';
static constexpr const char SetMod = 's';
static constexpr const char GetMod = 'g';
static constexpr const char System = 'Y';
static constexpr const char Info = 'I';
static constexpr const char Help = 'H';
namespace SystemC {
static constexpr const char Reboot = 'R';
}  // namespace SystemC
namespace InfoC {
static constexpr const char Module = 'M';
static constexpr const char Git = 'G';
static constexpr const char Firmware = 'F';
static constexpr const char BuildTime = 'T';
static constexpr const char BuildId = 'I';
static constexpr const char ChipName = 'C';
static constexpr const char ChipId = 'c';
}  // namespace InfoC
}  // namespace Commands


// globals
extern char commands[Constants::command_q][Constants::command_size];
extern uint8_t command_index = 0;
extern uint8_t command_last = 0;

void command_handler(void);
