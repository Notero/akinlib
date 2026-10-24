#pragma once

#include <memory>
#include <optional>
#include <cstddef>

template<typename T> 
class RingBuffer {
private:

    size_t head;
    size_t tail;
    size_t count;
    const size_t capacity;
    //RAII memory
    std::unique_ptr<T[]> buffer;

public:
    //make unique can appoint memory that deletes itself.
    explicit RingBuffer(const size_t cap): head(0), tail(0), count(0), capacity(cap), buffer(std::make_unique<T[]>(capacity)) {}
    class Iterator
    {
    private:
        T* ptr; // head or tail
        T* firstAddress; // first address of the array buffer.get();
        const T* lastAddress; // last address of the arrray buffer.get() + capacity;
        int mem;

    public:
    using value_type = T;
    using pointer = T*;
    using reference = T&;
    using difference_type = std::ptrdiff_t;
    using iterator_category = std::forward_iterator_tag;

        Iterator(T* h, T* firstAddy, T* lastAddy, int count) : ptr(h), firstAddress(firstAddy), lastAddress(lastAddy), mem(count) {}

        T& operator*() const {return *ptr;} //deference
        Iterator& operator++() {
            if(mem != 0) mem--;
            if (ptr == lastAddress - 1) {ptr = firstAddress;}else ptr++;
            return *this;
            
        } // advance to next slot in the loop
        bool operator==(const Iterator& other) const {
            return mem == other.mem;
        }
        bool operator!=(const Iterator& other) const {   // are we at the same position?
            return mem != other.mem;  
        }
    };
    
    Iterator begin(){return Iterator(buffer.get() + head, buffer.get(), buffer.get() + capacity, count);}
    Iterator end(){return Iterator(buffer.get() + tail, buffer.get(), buffer.get() + capacity, 0);}

    bool push(T value){
        //if count == cap means we are at the end of the buffer
        if (count == capacity){
                return false;
        }
        buffer[tail] = value;
        count++;
        tail = (tail + 1) % capacity;
        return true;
    }

    std::optional<T> pop(){
        //means there is nothing to read;
        if(count == 0){
            return std::nullopt;
        }
        count--;
        T temp = buffer[head];
        head = (head + 1) % capacity;
        return temp;
    }

    std::optional<T> peek(){
        if(count == 0){
            return std::nullopt;
        }
        return buffer[head];
    }

};