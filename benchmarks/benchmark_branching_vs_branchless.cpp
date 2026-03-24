#include "matching_engine.hpp"
#include "timer.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <limits>
#include <random>
#include <string_view>
#include <vector>

namespace {

struct BenchmarkStats {
    std::chrono::nanoseconds runtime {};
    std::uint64_t checksum {};
};

using OrderPairs = std::pair<std::vector<bme::Order>, std::vector<bme::Order>>;

OrderPairs generate_orders(const std::size_t count) {
    std::mt19937_64 rng(42);
    std::uniform_real_distribution<double> buy_prices(99.0, 101.0);
    std::uniform_real_distribution<double> sell_offset(-0.50, 0.50);
    std::uniform_int_distribution<std::int32_t> quantities(1, 1000);

    std::vector<bme::Order> buys;
    std::vector<bme::Order> sells;
    buys.reserve(count);
    sells.reserve(count);

    for (std::size_t i = 0; i < count; ++i) {
        const double buy_price = buy_prices(rng);
        const double sell_price = buy_price + sell_offset(rng);

        buys.push_back(bme::Order{
            static_cast<std::uint64_t>(i * 2),
            buy_price,
            quantities(rng),
            bme::Side::Buy
        });

        sells.push_back(bme::Order{
            static_cast<std::uint64_t>(i * 2 + 1),
            sell_price,
            quantities(rng),
            bme::Side::Sell
        });
    }

    return {std::move(buys), std::move(sells)};
}

template <typename MatchFn>
BenchmarkStats run_benchmark(const std::vector<bme::Order>& base_buys,
                             const std::vector<bme::Order>& base_sells,
                             const MatchFn match_fn) {
    auto buys = base_buys;
    auto sells = base_sells;
    std::uint64_t checksum = 0;

    bme::Timer timer;
    for (std::size_t i = 0; i < buys.size(); ++i) {
        const auto result = match_fn(buys[i], sells[i]);
        checksum += static_cast<std::uint64_t>(result.traded_quantity);
    }

    return BenchmarkStats{timer.elapsed(), checksum};
}

void print_stats(const std::string_view label,
                 const BenchmarkStats& stats,
                 const std::size_t count) {
    const double average_ns =
        static_cast<double>(stats.runtime.count()) / static_cast<double>(count);

    std::cout << std::left << std::setw(16) << label
              << " runtime: " << std::setw(10) << stats.runtime.count() << " ns"
              << " avg: " << std::fixed << std::setprecision(2) << std::setw(8) << average_ns << " ns/match"
              << " checksum: " << stats.checksum << '\n';
}

}  // namespace

int main() {
    constexpr std::size_t order_count = 1'000'000;
    constexpr int runs = 5;

    const auto [base_buys, base_sells] = generate_orders(order_count);

    BenchmarkStats best_branching{std::chrono::nanoseconds::max(), 0};
    BenchmarkStats best_branchless{std::chrono::nanoseconds::max(), 0};

    for (int run = 0; run < runs; ++run) {
        const auto branching = run_benchmark(
            base_buys,
            base_sells,
            [](bme::Order& buy, bme::Order& sell) {
                return bme::MatchingEngine::match_branching(buy, sell);
            });

        const auto branchless = run_benchmark(
            base_buys,
            base_sells,
            [](bme::Order& buy, bme::Order& sell) {
                return bme::MatchingEngine::match_branchless(buy, sell);
            });

        best_branching = std::min(best_branching, branching, [](const auto& lhs, const auto& rhs) {
            return lhs.runtime < rhs.runtime;
        });
        best_branchless = std::min(best_branchless, branchless, [](const auto& lhs, const auto& rhs) {
            return lhs.runtime < rhs.runtime;
        });
    }

    std::cout << "Branching vs Branchless Matching Benchmark\n";
    std::cout << "=========================================\n";
    std::cout << "order pairs: " << order_count << '\n';
    std::cout << "runs: " << runs << " (best runtime shown)\n\n";

    print_stats("branching", best_branching, order_count);
    print_stats("branchless", best_branchless, order_count);

    std::cout << "\nNote: microbenchmarks are sensitive to CPU model, compiler, optimization level,\n";
    std::cout << "and the fraction of orders that actually cross.\n";

    return best_branching.checksum == best_branchless.checksum ? 0 : 1;
}
