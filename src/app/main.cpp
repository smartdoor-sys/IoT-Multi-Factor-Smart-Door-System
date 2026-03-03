#include "core/controller.hpp"
#include "core/alarm_manager.hpp"
#include "drivers/door_sensor.hpp"
#include "drivers/access_verifier.hpp"
#include "app/command_input.hpp"
#include <memory>

DoorSensor* create_door_sensor_mock();
AccessVerifier* create_access_verifier_mock();

int main() {
    AlarmManager alarm;
    MainController controller(alarm, 2000);
    controller.start();

    std::unique_ptr<DoorSensor> door(create_door_sensor_mock());
    std::unique_ptr<AccessVerifier> access(create_access_verifier_mock());

    door->start(controller.inbox());
    access->start(controller.inbox());

    CommandInput input;
    input.start(controller.inbox());
    input.wait();  

    door->stop();
    access->stop();
    controller.stop();
    return 0;
}