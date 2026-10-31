#include "RingBuffer.hpp"
#include <iostream>
#include <type_traits>

template<typename... Args>
struct TypeList{

    //returns the count of arguments for a given type list
    static constexpr size_t size(){return sizeof...(Args);}
    //returns true if a type is present in type list
    template<typename T>
    static constexpr bool isPresent() {return (std::is_same_v<T, Args> || ...);}
};


int main()
{
    //Ring Buffer
    static_assert(RingBuffer<int>::pnext(1,10) == 2, "assert pnext failed");

    //Type List
    TypeList<int, bool, char, float, double> a;
    static_assert(a.isPresent<std::string>() == 0, "assert isPresent failed");
    static_assert(a.size() == 5, "assert size of a failed");
    return 0;
}
