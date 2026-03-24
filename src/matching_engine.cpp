#include "matching_engine.hpp"

#include <algorithm>

namespace bme {

namespace {

MatchResult make_result(const bool matched,
                        const std::int32_t traded_quantity,
                        const double execution_price,
                        const Order& buy,
                        const Order& sell) noexcept {
    return MatchResult{
        matched,
        traded_quantity,
        execution_price,
        buy.quantity,
        sell.quantity
    };
}

}  // namespace

MatchResult MatchingEngine::match_branching(Order& buy, Order& sell) noexcept {
    if (buy.price >= sell.price) {
        const std::int32_t traded_quantity = std::min(buy.quantity, sell.quantity);
        buy.quantity -= traded_quantity;
        sell.quantity -= traded_quantity;
        return make_result(true, traded_quantity, sell.price, buy, sell);
    }

    return make_result(false, 0, 0.0, buy, sell);
}

MatchResult MatchingEngine::match_branchless(Order& buy, Order& sell) noexcept {
    const std::int32_t candidate_quantity = std::min(buy.quantity, sell.quantity);
    const std::int32_t crosses = static_cast<std::int32_t>(buy.price >= sell.price);
    const std::int32_t traded_quantity = candidate_quantity * crosses;

    buy.quantity -= traded_quantity;
    sell.quantity -= traded_quantity;

    const double execution_price = sell.price * static_cast<double>(crosses);
    const bool matched = traded_quantity > 0;

    return make_result(matched, traded_quantity, execution_price, buy, sell);
}

}  // namespace bme
