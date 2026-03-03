#pragma once
#include "core/blocking_queue.hpp"
#include "core/events.hpp"

class DoorSensor {
public:
    virtual ~DoorSensor() = default;
    virtual void start(BlockingQueue<Event>& out) = 0;
    virtual void stop() = 0;
};
