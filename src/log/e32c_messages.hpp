#pragma once

#define E32C_UNIT_LIST(X) \
    X(Main, "Main")       \
    X(Log, "Log")         \
    X(CirQ, "CirQ")       \
    X(Pref, "Pref")       \
    X(Net, "Net")         \
    X(Mod, "Mod")

#define E32C_ERROR_LIST(X)                              \
    X(Died, "Died")                                     \
    X(NoPrefs, "No Preferences found")                  \
    X(NoPref, "No Preference found: %s")                \
    X(PrefUnset, "Preference unset: %s")                \
    X(PrefUpdateFailed, "Preference Update Failed: %s") \
    X(NoPrefU, "No Pref %s, defaulting to %u")          \
    X(NoPrefS, "No Pref %s, defaulting to %s")          \
    X(QueueEmpty, "Queue empty")                        \
    X(NoQueue, "No queue")                              \
    X(ItemTooBig, "Item too big")                       \
    X(TimeSyncFailed, "Time sync failed")               \
    X(NoNetwork, "No network")                          \
    X(NoInternet, "No internet")                        \
    X(SerializeError, "Serialize error")                \
    X(DeserializeError, "Deserialize error")            \
    X(CreateQueueFailed, "Create queue failed")         \
    X(FailedToAddToQueue, "Failed to add to queue")     \
    X(UDPListenerFailed, "UDP listener failed")         \
    X(UnknownMessage, "Unknown message")                \
    X(StringTooBig, "String too big: %s")               \
    X(EncryptSetKeyFailed, "Encrypt set key failed")    \
    X(EncryptCryptError, "Encrypt crypt error")         \
    X(UDPBeginPacketFailed, "UDP begin packet failed")  \
    X(UDPWriteFailed, "UDP write failed")               \
    X(UDPEndPacketFailed, "UDP end packet failed")      \
    X(SnprfFailed, "snprintf failed: %s")

#define E32C_NOTICE_LIST(X)                                      \
    X(KeepAlive, "Keep alive")                                   \
    X(LoopedN, "looped %d times")                                \
    X(CirQCreate, "CQ::CircularQueue(%ld, %d)")                  \
    X(ComputedEntries, "Computed entries: %d")                   \
    X(PSRAMFound, "PSRAM found")                                 \
    X(PSRAMNotFound, "PSRAM not found")                          \
    X(PSRAMCreated, "PSRAM %s created")                          \
    X(MallocCreated, "Malloc %s created")                        \
    X(FreeCapacity, "Free capacity: %u")                         \
    X(FreeEntries, "Free entries: %u")                           \
    X(CirQPush, "CQ Push: %d")                                   \
    X(CirQRdyToCopy, "Ready to copy data")                       \
    X(CirQSimpleCopy, "Simple copy")                             \
    X(CirQBrokenCopy, "Broken copy")                             \
    X(CirQPop, "CQ::pop: %d")                                    \
    X(Connected, "Connected")                                    \
    X(GotIP, "GotIP %s")                                         \
    X(Broadcast, "BroadcastIP %s")                               \
    X(Disconnected, "Disconnected")                              \
    X(TimeSynced, "Time synced")                                 \
    X(InternetConnected, "Internet connected")                   \
    X(ReceivedUDPPacket, "Received UDP packet from %s %s: %s")   \
    X(UDPPacket, "UDP packet received")                          \
    X(UDPListenerSetup, "UDP listener setup")                    \
    X(MessageSerialize, "Message Serialize: %s")                 \
    X(MessageDesrialize, "Message Deserialize")                  \
    X(ESP32NetInit, "ESP32Net Init")                             \
    X(CheckInternet, "check_internet (%d): %s")                  \
    X(InternetCheckCode, "Internet check HTTP code: %d")         \
    X(CheckingClock, "Failed to get local time, checking clock") \
    X(CheckClock, "Checking clock")                              \
    X(CheckQueue, "Check queue")                                 \
    X(SendMessage, "Send message (%s:%d): %s")                   \
    X(Encrypting, "Encrypting")                                  \
    X(PrefReadS, "Read Pref %s as %s")                           \
    X(PrefReadU, "Read Pref %s as %u")                           \
    X(PrefUpdatedS, "Updated Pref %s as %s")                     \
    X(PrefUpdatedU, "Updated Pref %s as %u")                     \
    X(Done, "Done")

#define E32C_WORD_LIST(X) \
    X(Buffer, "Buffer")   \
    X(Records, "Records") \
    X(Message, "Message") \
    X(XXX, "XXX")         \
    X(NoTime, "No Time")

#define E32C_NAME_LIST(X)   \
    X(MsgID, "MsgID")       \
    X(ChipName, "ChipName") \
    X(IP, "IP")
