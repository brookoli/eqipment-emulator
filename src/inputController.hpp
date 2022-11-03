#ifndef INPUT_CONTROLLER_HPP
#define INPUT_CONTROLLER_HPP

#include <string>
#include <unordered_map>
#include <vector>
#include <functional>

class inputController {
public:
    enum class EventType {
        MOUSE,
        KEYBOARD
    };

    using EventCallback = std::function<bool(std::string)>;

    void RegisterCallback(EventType type, EventCallback cb);
    void Dispatch(EventType type, std::string msg);

private:
    std::unordered_map<EventType, std::vector<EventCallback>> _callbacks{};
};

#endif