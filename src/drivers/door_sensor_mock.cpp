#include "drivers/door_sensor.hpp"
#include "core/logger.hpp"

class DoorSensorMock : public DoorSensor {
public:
    void start(BlockingQueue<Event>&) override {
        log_line("[MOCK] DoorSensor ready (input handled by CommandInput)");
    }

    void stop() override {
        log_line("[MOCK] DoorSensor stopped.");
    }
};

DoorSensor* create_door_sensor_mock() {
    return new DoorSensorMock();
}