#pragma once

#include <chrono>

namespace bme {

class Timer {
public:
    using clock = std::chrono::steady_clock;

    Timer() noexcept : start_(clock::now()) {}

    void reset() noexcept {
        start_ = clock::now();
    }

    [[nodiscard]] std::chrono::nanoseconds elapsed() const noexcept {
        return std::chrono::duration_cast<std::chrono::nanoseconds>(clock::now() - start_);
    }

    [[nodiscard]] double elapsed_microseconds() const noexcept {
        return static_cast<double>(elapsed().count()) / 1'000.0;
    }

    [[nodiscard]] double elapsed_milliseconds() const noexcept {
        return static_cast<double>(elapsed().count()) / 1'000'000.0;
    }

private:
    clock::time_point start_;
};

}  // namespace bme
