#include "inputController.hpp"
#include "TaskController.hpp"
#include "DeviceController.hpp"
#include <iostream>
#include <string>


void menu() {
    std::cout << "1 - Show system's status" << std::endl;
    std::cout << "2 - Set pump speed" << std::endl;
    std::cout << "3 - Set pressure" << std::endl;
    std::cout << "4 - Set period" << std::endl;
    std::cout << "5 - Quit & Stop the system" << std::endl;
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

            std::cout << "Choose command: ";
            std::cout << "(you may input integer with args if needed) \n";
            menu();
            std::cout << "> ";
            getline(std::cin, inputCmd);

            command = inputCmd[0];

            switch (command) {
            case 'r':
                // restart the system
                _taskController.Start();
                break;
            case '6':
                _taskController.QueueCommand(inputCmd);
                _running = false;
                break;
            default:
                _taskController.QueueCommand(inputCmd);
            }
        }

        std::cout << "\nSYSTEM INFO: Preparing to close...\n\n";
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