#pragma once
#include <string>
#include <memory>
#include <functional>
#include <sigma/rule_parser/utilities.h>
#include <c4/substr.hpp>
#include <c4/std/std.hpp>

namespace sigma {
    struct IPredicate {
        virtual bool Match(const std::string& ruleVal, const std::string& matchVal) const = 0;
        virtual ~IPredicate() = default;
    };

    struct WildcardPredicate : IPredicate {
        bool Match(const std::string& ruleVal, const std::string& matchVal) const override {
            return utils::IsWildcardMatchInsensitive(ruleVal.c_str(), matchVal.c_str());
        }
    };

    struct CaseSensitiveWildcardPredicate : IPredicate {
        bool Match(const std::string& ruleVal, const std::string& matchVal) const override {
            return utils::IsWildcardMatch(ruleVal.c_str(), matchVal.c_str());
        }
    };

    struct NumericPredicate : IPredicate {
        explicit NumericPredicate(std::function<bool(double, double)> cmp) : cmpFunc(std::move(cmp)) {}

        bool Match(const std::string& ruleVal, const std::string& matchVal) const override {
            c4::csubstr c4r = c4::to_csubstr(ruleVal);
            c4::csubstr c4m = c4::to_csubstr(matchVal);

            double rVal = 0.0, mVal = 0.0;
            bool res = c4::atod(c4r, &rVal);
            res &= c4::atod(c4m, &mVal);

            if (!res)
                throw std::invalid_argument("Numeric conversion failed: (" + ruleVal + ") and (" + matchVal + ")");

            return cmpFunc(mVal, rVal); // matchVal first to preserve original semantics
        }

    private:
        std::function<bool(double, double)> cmpFunc;
    };

    struct ExistsPredicate : IPredicate {
        bool Match(const std::string&, const std::string&) const override {
            // because if it wouldn't exist, then it wouldnt even reach until here
            // because early filtering would not even start matching event with this selector
            return true;
        }
    };

    struct NotEqualityPredicate : IPredicate {
        explicit NotEqualityPredicate(std::unique_ptr<IPredicate> inner) : innerPred(std::move(inner)) {}
        bool Match(const std::string& ruleVal, const std::string& matchVal) const override {

            return !innerPred->Match(ruleVal, matchVal);
        }
    private:
        std::unique_ptr<IPredicate> innerPred;
    };

    struct CidrPredicate : IPredicate {
        bool Match(const std::string& ruleVal, const std::string& matchVal) const override {
            auto parseIp = [](c4::csubstr s) -> uint32_t {
                uint32_t val = 0;
                size_t start = 0;
                for (int i = 0; i < 4; i++) {
                    auto dotPos = s.find('.', start);
                    c4::csubstr octet = (dotPos == c4::csubstr::npos) ? s.sub(start) : s.sub(start, dotPos - start);
                    uint32_t n = 0;
                    for (size_t j = 0; j < octet.len; j++) {
                        n = n * 10 + (octet.str[j] - '0');
                    }
                    val = (val << 8) | n;
                    start = dotPos + 1;
                }
                return val;
                };

            c4::csubstr cidr = c4::to_csubstr(ruleVal);
            auto slashPos = cidr.find('/');
            if (slashPos == c4::csubstr::npos) throw std::invalid_argument("Invalid CIDR: " + ruleVal);

            c4::csubstr baseIp = cidr.sub(0, slashPos);
            c4::csubstr maskStr = cidr.sub(slashPos + 1);

            int maskLen = 0;
            if (!c4::atoi(maskStr, &maskLen) || maskLen > 32)
                throw std::invalid_argument("Invalid mask: " + std::string(maskStr.str, maskStr.len));

            uint32_t mask = maskLen == 0 ? 0 : 0xFFFFFFFF << (32 - maskLen);

            uint32_t baseInt = parseIp(baseIp);
            uint32_t ipInt = parseIp(c4::to_csubstr(matchVal));

            return (ipInt & mask) == (baseInt & mask);
        }
    };

}
