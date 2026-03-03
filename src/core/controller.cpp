#include "core/controller.hpp"
#include "core/logger.hpp"
#include <cmath>

MainController::MainController(AlarmManager& alarm, int64_t window_ms)
    : alarm_(alarm), window_ms_(window_ms) {
}

MainController::~MainController() { stop(); }

void MainController::start() {
    running_.store(true);
    th_ = std::thread(&MainController::run, this);
}

void MainController::stop() {
    if (!running_.exchange(false)) return;
    q_.close();
    if (th_.joinable()) th_.join();
}

BlockingQueue<Event>& MainController::inbox() { return q_; }

void MainController::run() {
    log_line("[CTRL] Started.");
    while (running_.load()) {
        auto evOpt = q_.pop();
        if (!evOpt) break;

        std::visit([&](auto&& ev) {
            using T = std::decay_t<decltype(ev)>;

            if constexpr (std::is_same_v<T, AuthGranted>) {
                last_auth_ms_.store(ev.t_ms);
                log_line("[AUTH] Granted by " + ev.who);
            }
            else if constexpr (std::is_same_v<T, DoorOpen>) {
                auto la = last_auth_ms_.load();
                bool ok = (la >= 0) && (std::llabs(ev.t_ms - la) <= window_ms_);
                log_line("[DOOR] Open. auth_match=" + std::string(ok ? "true" : "false"));

                if (!ok) alarm_.trigger();
            }
            else if constexpr (std::is_same_v<T, DoorClose>) {
                log_line("[DOOR] Close.");
            }
            }, *evOpt);
    }
    log_line("[CTRL] Stopped.");
}