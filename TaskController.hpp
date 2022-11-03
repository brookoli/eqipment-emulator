#ifndef TASK_CONTROLLER_HPP
#define TASK_CONTROLLER_HPP

#include "DeviceController.hpp"
#include <thread>
#include <deque>
#include <vector>

using Command = char;

struct TaskController {
public:
    TaskController() = default;
    ~TaskController();

    void Start();
    void QueueCommand(std::string);
private:
    void run();
    void executeCommand(Command, std::vector<std::string> args = {});
    void shutdown();
    void setPeriod(unsigned int);
    void showStatus();
private:
    std::thread _commandThread;
    std::deque<std::string> _commandQueue{};
    bool _running{ false };
    unsigned int _period{ 5000 };
    PumpController _pump{};
    Sensors _sensor1{};
    Sensors _sensor2{};
};

#endif