#include <benchmark/benchmark.h>
#include <string>
#include <string_view>
#include <c4/substr.hpp>
#include <c4/std/std.hpp>
#include <random>

// ================================================================
// 🔧 global random string generation (outside benchmarks)
static std::string random_string(size_t len) {
    static const char charset[] =
        "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    static thread_local std::mt19937 rng{ std::random_device{}() };
    static thread_local std::uniform_int_distribution<size_t> dist(0, sizeof(charset) - 2);

    std::string s;
    s.reserve(len);
    for (size_t i = 0; i < len; ++i)
        s.push_back(charset[dist(rng)]);
    return s;
}

static std::string g_a_str = random_string(26);
static std::string g_b_str = random_string(26);
static std::string_view g_a_view = g_a_str;
static std::string_view g_b_view = g_b_str;
static c4::substr g_a_c4 = c4::to_substr(g_a_str);
static c4::substr g_b_c4 = c4::to_substr(g_b_str);

// ================================================================
// 🧪 Benchmarks

// std::string full comparison
static void BM_StringCompare_Full(benchmark::State& state) {
    for (auto _ : state)
        benchmark::DoNotOptimize(g_a_str == g_b_str);
}
BENCHMARK(BM_StringCompare_Full);

// std::string_view full comparison
static void BM_StringViewCompare_Full(benchmark::State& state) {
    for (auto _ : state)
        benchmark::DoNotOptimize(g_a_view == g_b_view);
}
BENCHMARK(BM_StringViewCompare_Full);

// c4::csubstr full comparison
static void BM_C4CSubstrCompare_Full(benchmark::State& state) {
    for (auto _ : state)
        benchmark::DoNotOptimize(g_a_c4.compare(g_b_c4) == 0);
}
BENCHMARK(BM_C4CSubstrCompare_Full);

// std::string substr comparison
static void BM_StringCompare_Substr(benchmark::State& state) {
    for (auto _ : state)
        benchmark::DoNotOptimize(g_a_str.substr(0, 20) == g_b_str.substr(0, 20));
}
BENCHMARK(BM_StringCompare_Substr);

// std::string_view substr comparison
static void BM_StringViewCompare_Substr(benchmark::State& state) {
    std::string_view sa = g_a_view.substr(0, 20);
    std::string_view sb = g_b_view.substr(0, 20);
    for (auto _ : state)
        benchmark::DoNotOptimize(sa == sb);
}
BENCHMARK(BM_StringViewCompare_Substr);

// c4::csubstr substr comparison
static void BM_C4CSubstrCompare_Substr(benchmark::State& state) {
    c4::csubstr sa = g_a_c4.sub(0, 20);
    c4::csubstr sb = g_b_c4.sub(0, 20);
    for (auto _ : state)
        benchmark::DoNotOptimize(sa.compare(sb) == 0);
}
BENCHMARK(BM_C4CSubstrCompare_Substr);

// std::string → c4::substr conversion + comparison
static void BM_StringToC4SubstrCompare(benchmark::State& state) {
    for (auto _ : state) {
        c4::substr sa = c4::to_substr(g_a_str);
        c4::substr sb = c4::to_substr(g_b_str);
        benchmark::DoNotOptimize(sa.compare(sb) == 0);
    }
}
BENCHMARK(BM_StringToC4SubstrCompare);

// ================================================================
BENCHMARK_MAIN();
