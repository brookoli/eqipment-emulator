#include "DeviceController.hpp"

#include <chrono>
#include <random>
#include <iostream>


template <typename T>
T randomNoise(void) {
    T percVal = 0.01;
    std::default_random_engine generator(std::chrono::system_clock::now().time_since_epoch().count());
    std::uniform_real_distribution<T> distribution(-percVal, percVal);
    return distribution(generator);
}

void PumpController::setSpeed(userSpeed uSpeed) {
    if (uSpeed > 0) {
        _state = "SPEED";
        _speed = uSpeed;
    } else {
        _state = "STOPPED";
        _speed = 0;
    }
}

userSpeed PumpController::getSpeed() {
    return _speed + randomNoise<userSpeed>();
}

std::string PumpController::getState() {
    return _state;
}

void Sensors::setPressure(userPressure uPressure) {
        _pressure = uPressure;
}

userPressure Sensors::getPressure() {
    return _pressure + randomNoise<userPressure>();
}