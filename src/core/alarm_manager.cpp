#include "core/alarm_manager.hpp"
#include "core/logger.hpp"

void AlarmManager::trigger() {
    active_.store(true);
    log_line("[ALARM] Triggered! (buzzer/LED + notify would happen here)");
}

void AlarmManager::reset() {
    active_.store(false);
    log_line("[ALARM] Reset.");
}

bool AlarmManager::is_active() const {
    return active_.load();
}