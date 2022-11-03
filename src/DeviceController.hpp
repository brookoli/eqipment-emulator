#ifndef DEVICE_CONTROLLER_HPP
#define DEVICE_CONTROLLER_HPP

#include <string>

using userSpeed = double;
using userPressure = double;

struct PumpController {
public:
    PumpController() = default;
    //~PumpController();

    userSpeed getSpeed();
    std::string getState();
    void setSpeed(userSpeed);
private:
    std::string _state{ "STOPPED" };
    userSpeed _speed{ 0 };
};

struct Sensors {
public:
    Sensors() = default;
    //~Sensors();

    userPressure getPressure();
    void setPressure(userPressure);
private:
    userPressure _pressure{ 0 };
};

#endif