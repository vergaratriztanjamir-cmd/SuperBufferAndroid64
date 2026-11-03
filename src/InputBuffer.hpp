
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include "InputTracker.hpp"

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

    struct Queue {
        std::array<BufferedInput, CAPACITY> events{};
        std::size_t head = 0;
        std::size_t count = 0;

        bool enqueue(const BufferedInput& event) {
            if (count >= CAPACITY) {
                return false;
            }

            const std::size_t index =
                (head + count) % CAPACITY;

            events[index] = event;
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

        void removeFront() {
            if (count == 0) {
                return;
            }

            head = (head + 1) % CAPACITY;
            count--;
        }

        void clear() {
            head = 0;
            count = 0;
        }
    };

    // Normal P1, Normal P2,
    // Practice P1, Practice P2.
    std::array<Queue, 4> queues{};

    std::size_t getQueueIndex(
        PlayMode mode,
        bool isPlayer1
    ) const {
        return
            static_cast<std::size_t>(mode) * 2 +
            (isPlayer1 ? 0 : 1);
    }

public:
    bool enqueue(
        PlayMode mode,
        uint64_t frame,
        int button,
        bool pressed,
        bool isPlayer1,
        uint32_t window
    ) {
        BufferedInput event{
            frame,
            button,
            pressed,
            isPlayer1,
            window
        };

        return queues[
            getQueueIndex(mode, isPlayer1)
        ].enqueue(event);
    }

    bool peekNext(
        PlayMode mode,
        bool isPlayer1,
        uint64_t currentFrame,
        BufferedInput& output
    ) {
        return queues[
            getQueueIndex(mode, isPlayer1)
        ].peekNext(currentFrame, output);
    }

    bool consumeNext(
        PlayMode mode,
        bool isPlayer1
    ) {
        return queues[
            getQueueIndex(mode, isPlayer1)
        ].consumeNext();
    }

    void clear() {
        for (auto& queue : queues) {
            queue.clear();
        }
    }

    std::size_t size(
        PlayMode mode,
        bool isPlayer1
    ) const {
        return queues[
            getQueueIndex(mode, isPlayer1)
        ].count;
    }

    bool empty(
        PlayMode mode,
        bool isPlayer1
    ) const {
        return size(mode, isPlayer1) == 0;
    }

    // Compatibility helpers for the existing self-test.
    bool enqueue(
        uint64_t frame,
        int button,
        bool pressed,
        bool isPlayer1,
        uint32_t window
    ) {
        return enqueue(
            PlayMode::Normal,
            frame,
            button,
            pressed,
            isPlayer1,
            window
        );
    }

    bool peekNext(
        uint64_t currentFrame,
        BufferedInput& output
    ) {
        return peekNext(
            PlayMode::Normal,
            true,
            currentFrame,
            output
        );
    }

    bool consumeNext() {
        return consumeNext(
            PlayMode::Normal,
            true
        );
    }

    bool empty() const {
        return empty(
            PlayMode::Normal,
            true
        );
    }
};

}
