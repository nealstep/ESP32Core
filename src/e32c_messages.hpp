#pragma once

#define E32C_UNIT_LIST(X) \
    X(Main, "Main")       \
    X(Log, "Log")         \
    X(CirQ, "CirQ")       \
    X(Net, "Net")

#define E32C_ERROR_LIST(X)                     \
    X(Died, "Died")                            \
    X(NoPrefs, "No Preferences found")         \
    X(NoPrefU, "No Pref %s, defaulting to %u") \
    X(NoPrefS, "No Pref %s, defaulting to %s") \
    X(QueueEmpty, "Queue empty")               \
    X(NoQueue, "No queue")                     \
    X(ItemTooBig, "Item too big")

#define E32C_NOTICE_LIST(X)                     \
    X(KeepAlive, "Keep alive")                  \
    X(Starting, "Starting up")                  \
    X(Started, "Started up")                    \
    X(LoopedN, "looped %d times")               \
    X(CirQCreate, "CQ::CircularQueue(%ld, %d)") \
    X(ComputedEntries, "Computed entries: %d")  \
    X(PSRAMFound, "PSRAM found")                \
    X(PSRAMCreated, "PSRAM %s created")         \
    X(MallocCreated, "Malloc %s created")       \
    X(FreeCapacity, "Free capacity: %u")        \
    X(FreeEntries, "Free entries: %u")          \
    X(CirQPush, "CQ Push: %d")                  \
    X(CirQRdyToCopy, "Ready to copy data")      \
    X(CirQSimpleCopy, "Simple copy")            \
    X(CirQBrokenCopy, "Broken copy")            \
    X(CirQPop, "CQ::pop: %d")                   \
    X(Done, "Done")

#define E32C_WORD_LIST(X) \
    X(Buffer, "Buffer")   \
    X(Records, "Records")
