#include "inputController.hpp"
#include "TaskController.hpp"
#include "DeviceController.hpp"
#include <iostream>
#include <string>


/*
1. Самый существенный - нет синхронизации доступа к очереди команд.
Контейнеры std::vector и std::deque сами по себе не являются потокобезопасными.
Их нужно защищать с помощью мьютекса, или семафора, или критической секции, или spin-lock.
Другой вариант - использовать нестандартный класс конкурентной очереди.

4. Для завершения программы или изменения периода текущий период не прерывается.
Функция std::this_thread::sleep_for не является прерываемой, ее следует заменить.
Варианты:
а) простой – цикл с проверкой флага выхода, прошедшего времени и с меньшим шагом по времени для sleep_for (аналог spin-lock).
б) хороший - std::condition_variable

5. Случайный шум должен добавляться вне зависимости от того, выполняется вывод состояния в консоль или нет.
Значения, созданные эмулятором, должны сохраняться до конца следующего периода.
Таким образом сколько бы раз ни была вызвана печать состояния, до конца периода оно не изменится.
*/


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
            case 'r':
                // restart the system
                _taskController.Start();
                break;
            case EXIT_THREAD:
                _running = false;
                break;
            case ERROR_FLAG:
                std::cout << "\nERROR: Wrong input\n" << std::endl;
                break;
            default:
                _taskController.QueueCommand(cmds);
            }
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