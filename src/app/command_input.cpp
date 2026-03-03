#include "app/command_input.hpp"
#include "core/logger.hpp"
#include <iostream>

void CommandInput::start(BlockingQueue<Event>& out) {
    running_.store(true);

    th_ = std::thread([this, &out] {
        log_line("[INPUT] Commands: o=open, c=close, a=auth, q=quit");

        while (running_.load()) {
            char ch;
            if (!(std::cin >> ch)) break;

            if (ch == 'o') out.push(DoorOpen{ now_ms() });
            else if (ch == 'c') out.push(DoorClose{ now_ms() });
            else if (ch == 'a') out.push(AuthGranted{ now_ms(), "mock_user" });
            else if (ch == 'q') {
                running_.store(false);
                break;
            }
        }

        log_line("[INPUT] Exit.");
        });
}

void CommandInput::wait() {
    if (th_.joinable()) th_.join();
}

void CommandInput::stop() {
    running_.store(false);
    if (th_.joinable()) th_.join();
}