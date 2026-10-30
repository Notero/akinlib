#include <any>
#include <typeindex>
#include <unordered_map>
#include <vector>
#include <concepts>
#include <functional>

struct Event{};
struct EOne : Event
{
    int val;
};

// real type check if a T is a actually derived from a Event struct Tag
// a concept definition is just template<typename T> concept Name = <some compile-time boolean expression>;
template<typename E>
concept event = std::derived_from<E, Event>;

template<typename E, typename... H>
concept handlermany = (std::same_as<H, std::function<void(const E&)>> && ...);

class EventHandler
{
private:
    //any can be anything makes it possible to group up different types
    std::unordered_map<std::type_index, std::vector<std::any>> handlers;
    
    
public:
    EventHandler(){};

    //we feed a function of event type T
    template<typename E>
    requires event<E>
    void subscribe(std::function<void(const E&)> sub){
        auto typeIndex = std::type_index(typeid(E));
        std::vector<std::any>& vec = handlers[typeIndex];
        vec.push_back(sub);
    }

    template<typename E, typename... Handlers>
    requires event<E> && handlermany<E, Handlers...>
    void subscribeMany(Handlers... subs){
        auto typeIndex = std::type_index(typeid(E));
        std::vector<std::any>& vec = handlers[typeIndex];
        //packs when to be used in a function fold like this
        (vec.push_back(subs), ...);
    }

    template<typename E>
    requires event<E>
    void dispatch(E dispatchedEvent){
        auto typeIndex = std::type_index(typeid(E));
        std::vector<std::any>& vec = handlers[typeIndex];
        for(auto& h: vec){
            (std::any_cast<std::function<void(const E&)>>(h))(dispatchedEvent);
        }
    }

};