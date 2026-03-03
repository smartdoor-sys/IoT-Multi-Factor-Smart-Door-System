#pragma once
#include "core/blocking_queue.hpp"
#include "core/events.hpp"
#include <atomic>
#include <thread>

class CommandInput {
public:
    CommandInput() = default;
    ~CommandInput() { stop(); }

    void start(BlockingQueue<Event>& out);
    void wait();   
    void stop();

private:
    std::atomic<bool> running_{ false };
    std::thread th_;
};