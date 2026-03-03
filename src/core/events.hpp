#pragma once
#include <cstdint>
#include <string>
#include <variant>

inline int64_t now_ms(); // It can also be implemented in logger.cpp

struct DoorOpen { int64_t t_ms; };
struct DoorClose { int64_t t_ms; };

struct AuthGranted { int64_t t_ms; std::string who; };

using Event = std::variant<DoorOpen, DoorClose, AuthGranted>;
