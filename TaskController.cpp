#define EXIT_THREAD '5'
#define SHOW_STATUS '1'
#define SET_PUMPSPEED '2'
#define SET_PRESSURE '3'
#define SET_PERIOD '4'

#include "TaskController.hpp"
#include <iostream>
#include <chrono>


std::vector<std::string> splitInput(std::string input) {
    size_t pos = 0;
    std::string token;
    std::vector<std::string> commands;
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
    shutdown();
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

void TaskController::QueueCommand(std::string input) {
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
        //parse string here??

        if (_commandQueue.size() == 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(_period));
            executeCommand(SHOW_STATUS); //print status here
            continue;
        }

        std::string test = _commandQueue.front();
        auto cmds = splitInput(test);

        if (cmds[0][0] != '6' || cmds[0][0] != '5')
            std::this_thread::sleep_for(std::chrono::milliseconds(_period)); //sleep for 2 sec

        executeCommand(cmds[0][0], cmds);
        _commandQueue.pop_front();
    }
    //shutting down thread
}

void TaskController::executeCommand(Command command, std::vector<std::string> args) {
    switch (command) {
    case EXIT_THREAD:
        shutdown();
        break;
    case SHOW_STATUS:
        showStatus();
        break;
    case SET_PUMPSPEED:
        userSpeed speed;
        if (args.size() < 2) {
            std::cout << "SPEED INPUT FAIL: wrong command.\nType:   " << SET_PUMPSPEED << " <_speed: any real number_>" << std::endl;
        }
        speed = std::stod(args[1]);
        _pump.setSpeed(speed);
        break;
    case SET_PRESSURE:
        if (args.size() >= 3) {
            char name = args[1][0];
            userPressure pressure = stod(args[2]);
            if (name == '1') 
                _sensor1.setPressure(pressure);
            else if (name == '2')
                _sensor2.setPressure(pressure);
            else
                std::cout << "\nPRESSURE INPUT FAIL: wrong sensor name. Type ___1___ or ___2___" << std::endl;
        } else {
            std::cout << "\nPRESSURE INPUT FAIL: wrong command. Type:   " << SET_PRESSURE << " <_sensor: 1 or 2_> <_pressure: any real number_>" << std::endl;
        }
        break;
    case SET_PERIOD:
        if (args.size() < 2) {
            std::cout << "PERIOD INPUT FAIL: wrong command. Type <" << SET_PERIOD << " <period: any integer number>" << std::endl;
        }
        unsigned int period = std::stoul(args[1]);
        setPeriod(period);
        break;
    }
}

void TaskController::shutdown() {
    if (_commandThread.joinable()) {
        _running = false;
        _commandThread.join();
    }
    std::cout << "\nSYSTEM INFO: The system has been shut down" << std::endl;
}

