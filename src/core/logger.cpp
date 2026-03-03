#include "core/logger.hpp"
#include <iostream>
#include <chrono>

static int64_t now_ms_impl() {
    using namespace std::chrono;
    return duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
}

int64_t now_ms() { return now_ms_impl(); }

void log_line(const std::string& s) {
    std::cout << s << std::endl;
}