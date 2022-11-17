#ifndef TASK_CONTROLLER_HPP
#define TASK_CONTROLLER_HPP

#define ERROR_FLAG '0'
#define EXIT_THREAD '5'
#define SHOW_STATUS '1'
#define SET_PUMPSPEED '2'
#define SET_PRESSURE '3'
#define SET_PERIOD '4'

#include "DeviceController.hpp"
#include <thread>
#include <deque>
#include <vector>

using Command = char;
using Args = std::vector<std::string>;

struct TaskController {
public:
    TaskController() = default;
    ~TaskController();

    void Start();
    void QueueCommand(Args);
    void Shutdown();
    Args analyze(std::string);
private:
    void run();
    void executeCommand(Command, Args args = {});
    void setPeriod(unsigned int);
    void showStatus();
private:
    std::thread _commandThread;
    std::deque<Args> _commandQueue{};
    bool _running{ false };
    unsigned int _period{ 5000 };
    PumpController _pump{};
    Sensors _sensor1{};
    Sensors _sensor2{};
};

#endif