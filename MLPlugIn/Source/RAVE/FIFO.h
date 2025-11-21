/*
  ==============================================================================

    Set up lock free FIFO operations

  ==============================================================================
*/

#pragma once
#include <array>
#include <atomic>

template <typename T, size_t Capacity> class LockFreeFIFO {
  public:
    bool push(const T &item) {
        const auto w = writeIndex.load(std::memory_order_relaxed);
        const auto next = (w + 1) % Capacity;

        if (next == readIndex.load(std::memory_order_acquire))
            return false; // full

        buffer[w] = item;
        writeIndex.store(next, std::memory_order_release);
        return true;
    }

    bool pop(T &out) {
        const auto r = readIndex.load(std::memory_order_relaxed);

        if (r == writeIndex.load(std::memory_order_acquire))
            return false; // empty

        out = buffer[r];
        readIndex.store((r + 1) % Capacity, std::memory_order_release);
        return true;
    }
    
    void clear() {
        writeIndex.store(0, std::memory_order_release);
        readIndex.store(0, std::memory_order_release);
    }

  private:
    std::array<T, Capacity> buffer{};
    std::atomic<size_t> writeIndex{0};
    std::atomic<size_t> readIndex{0};
};
