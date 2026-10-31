#include <iostream>
#include "EventHandler.hpp"

int main()
{

    EventHandler someGuy;
    std::function<void(const EOne&)> echo = [](const EOne& e) {
        std::cout << e.val << std::endl;
    };
    std::function<void(const EOne&)> plus2 = [](const EOne& e) {
        std::cout << e.val + 2 << std::endl;
    };
    someGuy.subscribe<EOne>(echo);
    someGuy.subscribe<EOne>(plus2);
    EOne e{};
    e.val = 2;
    someGuy.dispatch(e);
    
    return 0;
}
