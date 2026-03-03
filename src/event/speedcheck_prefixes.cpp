#include <chrono>
#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <cassert>
#include <random>
#include <windows.h>

// Mock wideToUtf8 - simulate cost of converting wide string to UTF-8.
// Replace with your actual implementation.
static std::string wideToUtf8_mock(const std::wstring& w)
{
    if (w.empty()) return {};
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, w.data(), (int)w.size(), nullptr, 0, nullptr, nullptr);
    if (size_needed <= 0) return {};
    std::string out(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, w.data(), (int)w.size(), &out[0], size_needed, nullptr, nullptr);
    return out;
}

// Variant using operator+ with literal char*
std::string variant_plus_static(const std::string& name, const std::wstring& attrW) {
    return name + "_" + wideToUtf8_mock(attrW);
}

// Variant using temporary prefix (name + '_') then append
std::string variant_temp_prefix(const std::string& name, const std::wstring& attrW) {
    std::string namePrefix = name + '_';
    return namePrefix + wideToUtf8_mock(attrW);
}

// Variant: precompute UTF-8, reserve exact size, append pieces
std::string variant_reserve_append(const std::string& name, const std::wstring& attrW) {
    std::string attrUtf8 = wideToUtf8_mock(attrW);
    std::string attributeName;
    attributeName.append(name);
    attributeName.push_back('_');
    attributeName.append(attrUtf8);
    return attributeName;
}

// Variant: build into pre-sized string and write into data() (requires C++17 writable string data)
std::string variant_prealloc_write(const std::string& name, const std::wstring& attrW) {
    std::string attrUtf8 = wideToUtf8_mock(attrW);
    std::string out(name.size() + 1 + attrUtf8.size(), '*');
    // copy name
    std::memcpy(&out[0], name.data(), name.size());
    out[name.size()] = '_';
    std::memcpy(&out[0] + name.size() + 1, attrUtf8.data(), attrUtf8.size());
    return out;
}

// Variant: build into pre-sized string and write into data() (requires C++17 writable string data)
std::string variant_prealloc_write_ai(const std::string& name, const std::wstring& attrW) {
    std::string attrUtf8 = wideToUtf8_mock(attrW);
    std::string out;
    out.resize(name.size() + 1 + attrUtf8.size());
    // copy name
    std::memcpy(&out[0], name.data(), name.size());
    out[name.size()] = '_';
    std::memcpy(&out[0] + name.size() + 1, attrUtf8.data(), attrUtf8.size());
    return out;
}

std::string variant_prealloc_write_3(const std::string& name, const std::wstring& attrW) {
    std::string attrUtf8 = wideToUtf8_mock(attrW);
    std::string out{ name + "_" + attrUtf8};
    return out;
}

// Variant: std::format (if available) -- guarded out if not compiled with format
#ifdef __cpp_lib_format
#include <format>
std::string variant_format(const std::string& name, const std::wstring& attrW) {
    return std::format("{}_{}", name, wideToUtf8_mock(attrW));
}
#endif

using Clock = std::chrono::high_resolution_clock;

uint64_t time_many(const std::function<std::string()>& fn, size_t iterations) {
    // run once to warm caches
    volatile auto warm = fn();
    (void)warm;

    auto t0 = Clock::now();
    std::string last;
    for (size_t i = 0; i < iterations; ++i) {
        last = fn();
        // prevent optimizing away
        if (last.empty()) std::cout << "";
    }
    auto t1 = Clock::now();
    (void)last;
    return std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count();
}

int main() {
    const std::string name = "exampleName";
    const std::wstring attrW = L"attributeNameWithSomeChars123";
    const size_t iterations = 2'000'000;

    std::vector<std::pair<std::string, std::function<std::string()>>> tests;

    tests.push_back({ "plus_static", [&] { return variant_plus_static(name, attrW); } });
    tests.push_back({ "temp_prefix", [&] { return variant_temp_prefix(name, attrW); } });
    tests.push_back({ "reserve_append", [&] { return variant_reserve_append(name, attrW); } });
    tests.push_back({ "prealloc_write", [&] { return variant_prealloc_write(name, attrW); } });
    tests.push_back({ "prealloc_write_ai", [&] { return variant_prealloc_write_ai(name, attrW); } });
    tests.push_back({ "prealloc_write_3", [&] { return variant_prealloc_write_3(name, attrW); } });
#ifdef __cpp_lib_format
    tests.push_back({ "format", [&] { return variant_format(name, attrW); } });
#endif

    std::cout << "Iterations: " << iterations << "\n";
    for (size_t i = 0; i < 2; i++)
    {
        for (auto& t : tests) {
            // run multiple trials and take best to reduce noise
            uint64_t best = UINT64_MAX;
            for (int trial = 0; trial < 5; ++trial) {
                uint64_t ns = time_many(t.second, iterations);
                if (ns < best) best = ns;
            }
            double per = double(best) / iterations;
            std::cout << t.first << ": total " << best << " ns, per-iter " << per << " ns\n";
        }
    }


    return 0;
}
