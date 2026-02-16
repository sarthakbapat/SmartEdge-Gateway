#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include <iostream>
#include <vector>
#include <mutex>

template <typename T>
class RingBuffer {
private:
    size_t head_, tail_, size;
    std::vector<T> buffer_;
    std::mutex l_mutex;

public:
    // Constructor
    RingBuffer(size_t size): size(size), buffer_(size) {
        head_ = 0;
        tail_ = 0;
    }

    bool writeToBuffer(const T& data) {
        std::lock_guard<std::mutex> l_guard(l_mutex); 
        if (isBufferFull()) {
            return false;
        }
        else {
            buffer_[head_] = data;
            head_ = (head_ + 1) % size;
            return true;
        }

    }


    bool readFromBuffer(T& read_data) {
        std::lock_guard<std::mutex> l_guard(l_mutex); 
        if (!isBufferEmpty()) {
            read_data = buffer_[tail_];
            tail_ = (tail_ + 1) % size;
            return true;
        }
        else {
            return false;
        }
        
    }


    bool isBufferFull() const {
        if (tail_ == ((head_ + 1) % size)) {
            return true;
        }
        return false;
    }


    bool isBufferEmpty() const {
        if (head_ == tail_)
            return true;
        return false;
    }


    size_t getBufferCurrentSize() const {
        std::lock_guard<std::mutex> l_guard(l_mutex); 
        return (head_ + size - tail_) % size;
    }

};

#endif
