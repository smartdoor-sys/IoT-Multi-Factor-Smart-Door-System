#pragma once
#include "core/blocking_queue.hpp"
#include "core/events.hpp"
#include "core/alarm_manager.hpp"
#include <atomic>
#include <thread>

class MainController {
public:
    explicit MainController(AlarmManager& alarm, int64_t window_ms = 2000);
    ~MainController();

    void start();
    void stop();

    BlockingQueue<Event>& inbox();

private:
    void run();

    AlarmManager& alarm_;
    int64_t window_ms_;
    BlockingQueue<Event> q_;
    std::atomic<bool> running_{ false };
    std::thread th_;

    std::atomic<int64_t> last_auth_ms_{ -1 };
};