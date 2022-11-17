#include "inputController.hpp"
#include "TaskController.hpp"
#include "DeviceController.hpp"
#include <iostream>
#include <string>


void menu() {
    std::cout << SHOW_STATUS << " - Show system's status" << std::endl;
    std::cout << SET_PUMPSPEED << " - Set pump speed - type '" << SET_PUMPSPEED << " [speed value: any real]'" << std::endl;
    std::cout << SET_PRESSURE << " - Set pressure - type '" << SET_PRESSURE << " [1 or 2] [pressure value: any real]'" << std::endl;
    std::cout << SET_PERIOD << " - Set period - type '" << SET_PERIOD << " [period value: any integer]'" << std::endl;
    std::cout << EXIT_THREAD << " - Quit & Stop the system" << std::endl;
}

struct App {
    void Init() {
        _inputController.RegisterCallback(inputController::EventType::MOUSE,
            [this](std::string message) -> bool {
                handleMouseEvent(message);
                return false;
            });

        // Registers handleMouseEvent with inputManager;
        _inputController.RegisterCallback(inputController::EventType::MOUSE,
            std::bind(&App::handleMouseEvent, this, std::placeholders::_1));
    }

    void Run() {
        _inputController.Dispatch(inputController::EventType::MOUSE, "Bound message");

        _running = true;
        _taskController.Start();

        while (_running) {
            Command command{ 0 };
            std::string inputCmd;
            std::vector<std::string> cmds;

            //start timer here - when it ends it change flag _itstime = true
            //when _itstime = true - run first command from queue or show status

            std::cout << "Choose command: ";
            std::cout << "(you may input integer with args if needed) \n";
            menu();
            std::cout << "> ";
            getline(std::cin, inputCmd);

            cmds = _taskController.analyze(inputCmd);

            switch (cmds[0][0]) {
            case SET_PERIOD:
                _taskController._canWait = false;
                _taskController.QueueCommand(cmds);
                break;
            case EXIT_THREAD:
                _running = false;
                _taskController._canWait = false;
                break;
            case ERROR_FLAG:
                std::cout << "\nERROR: Wrong input\n" << std::endl;
                break;
            default:
                _taskController.QueueCommand(cmds);
            }
            _taskController.cv.notify_all();
        }
        std::cout << "\nSYSTEM INFO: Preparing to close...\n\n";
        _taskController.Shutdown();
    }

    bool handleMouseEvent(std::string message) {
        //std::cout << "The message: " << message << std::endl;
        return true;
    }

private:
    inputController _inputController{};
    TaskController _taskController{};
    bool _running{ false };
};

void handleMouseEvent(std::string message) {
    //std::cout << "The message: " << message << std::endl;
}

int main() {
    App ap{};
    ap.Init();
    ap.Run();

    return 0;
}

#undef bool