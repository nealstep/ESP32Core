#pragma once

#define E32C_UNIT_LIST(X) \
    X(Main, "Main")       \
    X(Log, "Log")         \
    X(Net, "Net")

#define E32C_ERROR_LIST(X)                     \
    X(Died, "Died")                            \
    X(NoPrefU, "No Pref %s, defaulting to %u") \
    X(NoPrefS, "No Pref %s, defaulting to %s")

#define E32C_NOTICE_LIST(X)       \
    X(KeepAlive, "Keep alive")    \
    X(Starting, "Starting up")    \
    X(Started, "Started up")      \
    X(LoopedN, "looped %d times") \
    X(Done, "Done")

#define E32C_WORD_LIST(X)
