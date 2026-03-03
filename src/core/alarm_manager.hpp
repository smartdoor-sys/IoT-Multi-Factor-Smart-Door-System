#pragma once
#include <atomic>

class AlarmManager {
public:
    void trigger();
    void reset();
    bool is_active() const;

private:
    std::atomic<bool> active_{ false };
};