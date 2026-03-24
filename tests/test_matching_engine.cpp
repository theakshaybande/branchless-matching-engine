#include "matching_engine.hpp"

#include <cstdlib>
#include <iostream>

namespace {

void require(const bool condition, const char* message) {
    if (!condition) {
        std::cerr << "Test failure: " << message << '\n';
        std::exit(1);
    }
}

bme::Order make_order(const std::uint64_t id,
                      const double price,
                      const std::int32_t quantity,
                      const bme::Side side) {
    return bme::Order{id, price, quantity, side};
}

void test_price_cross() {
    auto buy = make_order(1, 100.50, 300, bme::Side::Buy);
    auto sell = make_order(2, 100.25, 200, bme::Side::Sell);

    const auto result = bme::MatchingEngine::match_branching(buy, sell);
    require(result.matched, "buy.price > sell.price should match");
    require(result.traded_quantity == 200, "trade quantity should be minimum quantity");
    require(buy.quantity == 100, "buy should be decremented");
    require(sell.quantity == 0, "sell should be decremented");
}

void test_equal_price_cross() {
    auto buy = make_order(3, 100.00, 120, bme::Side::Buy);
    auto sell = make_order(4, 100.00, 100, bme::Side::Sell);

    const auto result = bme::MatchingEngine::match_branching(buy, sell);
    require(result.matched, "buy.price == sell.price should match");
    require(result.traded_quantity == 100, "equal-price match should fill minimum quantity");
}

void test_no_match() {
    auto buy = make_order(5, 99.90, 150, bme::Side::Buy);
    auto sell = make_order(6, 100.10, 150, bme::Side::Sell);

    const auto result = bme::MatchingEngine::match_branching(buy, sell);
    require(!result.matched, "buy.price < sell.price should not match");
    require(result.traded_quantity == 0, "no match should trade zero quantity");
    require(buy.quantity == 150, "buy quantity should remain unchanged");
    require(sell.quantity == 150, "sell quantity should remain unchanged");
}

void test_partial_fill_buy_smaller() {
    auto buy = make_order(7, 101.00, 80, bme::Side::Buy);
    auto sell = make_order(8, 100.50, 200, bme::Side::Sell);

    const auto result = bme::MatchingEngine::match_branching(buy, sell);
    require(result.traded_quantity == 80, "smaller buy should fully fill");
    require(buy.quantity == 0, "buy should be fully filled");
    require(sell.quantity == 120, "sell should retain remainder");
}

void test_partial_fill_sell_smaller() {
    auto buy = make_order(9, 101.00, 250, bme::Side::Buy);
    auto sell = make_order(10, 100.50, 100, bme::Side::Sell);

    const auto result = bme::MatchingEngine::match_branching(buy, sell);
    require(result.traded_quantity == 100, "smaller sell should fully fill");
    require(buy.quantity == 150, "buy should retain remainder");
    require(sell.quantity == 0, "sell should be fully filled");
}

void test_equivalence() {
    auto buy_a = make_order(11, 100.25, 350, bme::Side::Buy);
    auto sell_a = make_order(12, 100.00, 125, bme::Side::Sell);
    auto buy_b = buy_a;
    auto sell_b = sell_a;

    const auto branching = bme::MatchingEngine::match_branching(buy_a, sell_a);
    const auto branchless = bme::MatchingEngine::match_branchless(buy_b, sell_b);

    require(branching.matched == branchless.matched, "match flags should agree");
    require(branching.traded_quantity == branchless.traded_quantity, "traded quantity should agree");
    require(branching.execution_price == branchless.execution_price, "execution price should agree");
    require(buy_a.quantity == buy_b.quantity, "buy remainder should agree");
    require(sell_a.quantity == sell_b.quantity, "sell remainder should agree");
}

}  // namespace

int main() {
    test_price_cross();
    test_equal_price_cross();
    test_no_match();
    test_partial_fill_buy_smaller();
    test_partial_fill_sell_smaller();
    test_equivalence();

    std::cout << "All matching engine tests passed.\n";
    return 0;
}
