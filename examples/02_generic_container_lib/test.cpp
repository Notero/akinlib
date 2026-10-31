#include "RingBuffer.hpp"
#include <cassert>
#include <iostream>
#include <algorithm>

int main(){

    RingBuffer<int> a(15);
    assert(a.pop() == std::nullopt);
    assert(a.peek() == std::nullopt);
    for (size_t i = 0; i < 16; i++)
    {
        if(i != 15){assert(a.push(i) == true);} else assert(a.push(i) == false);
    }

    assert (5 == *std::find(a.begin(),a.end(),5));
    assert (a.end() == std::find(a.begin(),a.end(),15));
    assert (std::count_if(a.begin(),a.end(),[](int x) { return x % 2 == 0; }) == 8);
    assert (std::count_if(a.begin(),a.end(),[](int x) { return x % 2 != 0; }) == 7);
    
    int c = 0;
    for(int b : a){
        assert(b == c);
        c++;
    }

    for (size_t i = 0; i < 5; i++)
    {
        assert(a.pop() == i);
    }
    
    for (size_t i = 0; i < 3; i++)
    {
        assert(a.push(i) == true);
    }

    for(auto b : a){
        std::cout << b << std::endl;
    }

    for (size_t i = 0; i < 13; i++)
    {
        assert(a.pop() != std::nullopt);
    }
    assert(a.pop() == std::nullopt);


    
    return 0;
}