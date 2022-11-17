#include "TaskController.hpp"
#include <iostream>
#include <chrono>
#include <mutex>

std::mutex m;
std::mutex g_lockprint;
std::mutex g_lock;
std::condition_variable cv;
bool g_done;


Args splitInput(std::string input) {
    size_t pos = 0;
    std::string token;
    Args commands;
    char delimiter = ' ';
    while ((pos = input.find(delimiter)) != std::string::npos) {
        token = input.substr(0, pos);
        commands.push_back(token);
        input.erase(0, pos + 1);
    }
    if ((pos = input.find(delimiter)) == std::string::npos) {
        commands.push_back(input);
    }
    return commands;
}


Args TaskController::analyze(std::string answer) {
    int cmd;
    Args command = splitInput(answer);
    if (command[0].empty()) {
        command[0] = ERROR_FLAG;
    } else {
        cmd = std::stoi(command[0]);

        if (cmd == 2 || cmd == 4) {
            if (command.size() < 2 || command[1].empty())
                command[0] = ERROR_FLAG;
        }
        else if (cmd == 3) {
            if (command.size() < 3 || command[1].empty() || command[2].empty())
                command[0] = ERROR_FLAG;
            else if (std::stoi(command[1]) != 1 && std::stoi(command[1]) != 2) {
                std::cout << "\nPRESSURE INPUT FAIL: wrong sensor name. Type ___1___ or ___2___" << std::endl;
                command[0] = ERROR_FLAG;
            }
        }
        else if (cmd != 1 && cmd != 5) {
            command[0] = ERROR_FLAG;
        }
    }
    return command;
}


void TaskController::setPeriod(unsigned int uPeriod) {
    _period = uPeriod;
}

void TaskController::showStatus() {
    userPressure s1Pressure = _sensor1.getPressure();
    userPressure s2Pressure = _sensor2.getPressure();
    std::cout << "\n\nSTATUS:" << std::endl;
    std::cout << "Pump state:\t\t\t" << _pump.getState() << std::endl;
    std::cout << "Current pump's speed:\t\t" << _pump.getSpeed() << std::endl;
    std::cout << "Pressure sensor #1:\t\t" << s1Pressure << std::endl;
    std::cout << "Pressure sensor #2:\t\t" << s2Pressure << std::endl;
    std::cout << "Pressure difference #1-#2:\t";
    std::cout << s1Pressure - s2Pressure << std::endl;
}

TaskController::~TaskController() {
    Shutdown();
}

void TaskController::Start() {
    std::cout << "\nSYSTEM INFO: Loading...\n\n";

    if (_commandThread.joinable()) {
        std::cout << "\nSYSTEM INFO: System is running already\n";
        return;
    }

    _commandQueue.clear();
    _commandThread = std::thread(&TaskController::run, this);
}

void TaskController::QueueCommand(Args input) {
    
    if (!_running) {
        std::cout << "\nWARNING: System is shut down. Type 'r' to restart\n";
        return;
    }
    //std::cout << "Queuing command\n";
    _commandQueue.push_back(input);
    
}

void TaskController::run() {
    //starting thread
    _running = true;
    while (_running) {

        if (_commandQueue.size() == 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(_period));
            executeCommand(SHOW_STATUS); //print status here
            continue;
        }

        auto cmds = _commandQueue.front();

        if (cmds[0][0] != SET_PERIOD && cmds[0][0] != EXIT_THREAD && cmds[0][0] != SHOW_STATUS)
            std::this_thread::sleep_for(std::chrono::milliseconds(_period)); //sleep for 2 sec

        executeCommand(cmds[0][0], cmds);
        _commandQueue.pop_front();
    }
    //shutting down thread
}

void TaskController::executeCommand(Command command, Args args) {
    switch (command) {
    case SHOW_STATUS:
        showStatus();
        break;
    case SET_PUMPSPEED: {
        userSpeed speed;
        speed = std::stod(args[1]);
        _pump.setSpeed(speed);
    }
        break;
    case SET_PRESSURE: {
        userPressure pressure = std::stod(args[2]);
        if (args[1][0] == '1')
            _sensor1.setPressure(pressure);
        else if (args[1][0] == '2')
            _sensor2.setPressure(pressure);
    }
        break;
    case SET_PERIOD: {
        unsigned int period = std::stoul(args[1]);
        setPeriod(period);
    }
        break;
    }
}

void TaskController::Shutdown() {
    if (_commandThread.joinable()) {
        _running = false;
        _commandThread.join();
    }
    std::cout << "\nSYSTEM INFO: The system has been shut down" << std::endl;
}

