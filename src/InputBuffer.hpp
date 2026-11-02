
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

namespace superbuffer {

struct BufferedInput {
    uint64_t frame = 0;
    int button = 0;
    bool pressed = false;
    bool isPlayer1 = true;
    uint32_t window = 0;
};

class InputBuffer {
private:
    static constexpr std::size_t CAPACITY = 64;

    std::array<BufferedInput, CAPACITY> events{};

    std::size_t head = 0;
    std::size_t count = 0;

    void removeFront() {
        if (count == 0) {
            return;
        }

        head = (head + 1) % CAPACITY;
        count--;
    }

public:
    bool enqueue(
        uint64_t frame,
        int button,
        bool pressed,
        bool isPlayer1,
        uint32_t window
    ) {
        if (count >= CAPACITY) {
            return false;
        }

        const std::size_t index =
            (head + count) % CAPACITY;

        events[index] = {
            frame,
            button,
            pressed,
            isPlayer1,
            window
        };

        count++;
        return true;
    }

    bool peekNext(
        uint64_t currentFrame,
        BufferedInput& output
    ) {
        while (count > 0) {
            const auto& event = events[head];

            const uint64_t age =
                currentFrame >= event.frame
                ? currentFrame - event.frame
                : 0;

            if (age > event.window) {
                removeFront();
                continue;
            }

            output = event;
            return true;
        }

        return false;
    }

    bool consumeNext() {
        if (count == 0) {
            return false;
        }

        removeFront();
        return true;
    }

    void clear() {
        head = 0;
        count = 0;
    }

    std::size_t size() const {
        return count;
    }

    bool empty() const {
        return count == 0;
    }
};

}
