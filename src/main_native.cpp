
#ifndef ARDUINO

#include <iostream>

#ifndef PIO_UNIT_TESTING

void test(void) {
    uint16_t times = 0;
    LOG_N(Log::Uni::Unnamed, Log::Sev::All, Log::Note::Starting);
    log_version();
    std::cout << std::hex
              << "Unit: " << static_cast<uint32_t>(lg.get_unit_mask())
              << std::endl;
    std::cout << "Sev: " << static_cast<uint16_t>(lg.get_severity())
              << std::endl;
    std::cout << std::dec << "loops: " << Global::loop_counter << std::endl;
    std::cout << "times: " << times << std::endl;
    while (times < 3) {
        if (Global::loop_counter > Constants::loop_interval) {
            LOG_N(Log::Uni::Main, Log::Sev::Inf, Log::Note::LoopedN,
                  Global::loop_counter);
            Global::loop_counter = 0;
            times++;
        }
        Global::loop_counter++;
    }
    LOG_E(Log::Uni::Main, Log::Err::UnexpectedError);
    LOG(Log::Uni::Main, Log::Sev::Wrn, "A test %d", 45);
    std::cout << "loops: " << Global::loop_counter << std::endl;
    std::cout << "times: " << times << std::endl;
    LOG_N(Log::Uni::Main, Log::Sev::Inf, Log::Note::Done);
}

int main(int argc, char* argv[]) {
    std::cout << "Starting" << std::endl;
    log_version();

    std::cout << std::endl << "Everything" << std::endl;
    lg.set_unit_mask(Log::Uni::Last);
    lg.set_severity(Log::Sev::Dbg);
    test();
    std::cout << std::endl << "Only Errors+" << std::endl;
    lg.set_severity(Log::Sev::Err);
    test();

    std::cout << std::endl << "Warnings+ from Main" << std::endl;
    lg.set_unit_mask(Log::Uni::Main);
    lg.set_severity(Log::Sev::Wrn);
    test();

    // restore to full
    lg.set_unit_mask(Log::Uni::Last);
    lg.set_severity(Log::Sev::Dbg);

    // basic circular queue test

#if USE_QUEUE
    std::cout << std::endl << "Circual Queue" << std::endl;
    static constexpr size_t test_buffer_size = 10;
    static constexpr uint16_t test_entries = 3;
    CircularQueue circularQueue(test_buffer_size, test_entries);
    LOG_N(Log::Uni::Main, Log::Sev::Inf, Log::Note::Starting);
    LOG_N(Log::Uni::Main, Log::Sev::Inf, Log::Note::FreeCapacity,
          circularQueue.get_free_capacity());
    LOG_N(Log::Uni::Main, Log::Sev::Inf, Log::Note::FreeEntries,
          circularQueue.get_free_entries());
#endif  // USE_QUEUE

    LOG_N(Log::Uni::Main, Log::Sev::Inf, Log::Note::Done);
    std::cout << std::endl << "Done" << std::endl;
}
#endif  // PIO_UNIT_TESTING

#endif  // !ARDUINO