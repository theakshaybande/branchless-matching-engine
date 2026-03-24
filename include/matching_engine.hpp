#pragma once

#include "order.hpp"

#include <cstdint>

namespace bme {

struct MatchResult {
    bool matched {};
    std::int32_t traded_quantity {};
    double execution_price {};
    std::int32_t remaining_buy_quantity {};
    std::int32_t remaining_sell_quantity {};
};

class MatchingEngine {
public:
    static MatchResult match_branching(Order& buy, Order& sell) noexcept;
    static MatchResult match_branchless(Order& buy, Order& sell) noexcept;
};

}  // namespace bme
