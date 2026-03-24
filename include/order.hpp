#pragma once

#include <cstdint>
#include <ostream>

namespace bme {

enum class Side : std::uint8_t {
    Buy,
    Sell
};

struct alignas(32) Order {
    std::uint64_t id {};
    double price {};
    std::int32_t quantity {};
    Side side {Side::Buy};
};

inline const char* to_string(const Side side) noexcept {
    return side == Side::Buy ? "Buy" : "Sell";
}

inline std::ostream& operator<<(std::ostream& os, const Order& order) {
    os << "Order{id=" << order.id
       << ", side=" << to_string(order.side)
       << ", price=" << order.price
       << ", quantity=" << order.quantity
       << '}';
    return os;
}

}  // namespace bme
