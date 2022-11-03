#include "inputController.hpp"
#include <iostream>

void inputController::RegisterCallback(EventType type, EventCallback cb) {
    _callbacks[type].push_back(cb);
}


void inputController::Dispatch(EventType type, std::string msg) {
    for (auto cb : _callbacks[type]) {
        // if cb returns true, the message has been handled and should not propagate further
        if (cb(msg)) return;
    }
}
