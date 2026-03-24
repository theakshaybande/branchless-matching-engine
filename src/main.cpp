#include "matching_engine.hpp"

#include <iomanip>
#include <iostream>

namespace {

void print_result(const char* label,
                  const bme::MatchResult& result,
                  const bme::Order& buy,
                  const bme::Order& sell) {
    std::cout << label << '\n';
    std::cout << "  matched: " << std::boolalpha << result.matched << '\n';
    std::cout << "  traded quantity: " << result.traded_quantity << '\n';
    std::cout << "  execution price: " << std::fixed << std::setprecision(2) << result.execution_price << '\n';
    std::cout << "  remaining buy: " << buy.quantity << '\n';
    std::cout << "  remaining sell: " << sell.quantity << "\n\n";
}

}  // namespace

int main() {
    std::cout << "Branchless Matching Engine Demo\n";
    std::cout << "===============================\n\n";

    bme::Order buy_one{1, 100.25, 400, bme::Side::Buy};
    bme::Order sell_one{2, 100.00, 250, bme::Side::Sell};

    auto branching_result = bme::MatchingEngine::match_branching(buy_one, sell_one);
    print_result("Branching match", branching_result, buy_one, sell_one);

    bme::Order buy_two{1, 100.25, 400, bme::Side::Buy};
    bme::Order sell_two{2, 100.00, 250, bme::Side::Sell};

    auto branchless_result = bme::MatchingEngine::match_branchless(buy_two, sell_two);
    print_result("Branchless match", branchless_result, buy_two, sell_two);

    bme::Order buy_three{3, 99.75, 500, bme::Side::Buy};
    bme::Order sell_three{4, 100.10, 500, bme::Side::Sell};

    auto no_match_result = bme::MatchingEngine::match_branchless(buy_three, sell_three);
    print_result("No-cross example", no_match_result, buy_three, sell_three);

    return 0;
}
