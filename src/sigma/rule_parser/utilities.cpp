#include <rule_parser/utilities.h>
#include <c4/substr.hpp>
namespace utils {
    bool IsWildcardMatch(const char* pat, const char* str)
    {
        const char* locp = nullptr;
        const char* locs = nullptr;
        bool escape = false;

        while (*str) {
            escape = false;
            /* we encounter a star */
            if (*pat == '\\') {
                escape = true;
                pat++;
            }
            if (!escape && *pat == '*') {
                locp = ++pat;
                locs = str;
                if (*pat == '\0') {
                    return true;
                }
                continue;
            }
            /* we have a mismatch */
            if (*str != *pat && (escape || *pat != '?')) {
                if (!locp) {
                    return false;
                }
                str = ++locs;
                pat = locp;
                continue;
            }
            pat++, str++;
        }
        /* check if the pattern's ended */
        while (*pat == '*') {
            pat++;
        }
        return (*pat == '\0');
    }

    bool IsWildcardMatchInsensitive(const char* pat, const char* str)
    {
        const char* locp = nullptr;
        const char* locs = nullptr;
        bool escape = false;

        while (*str) {
            escape = false;
            /* we encounter a star */
            if (*pat == '\\') {
                escape = true;
                pat++;
            }
            if (!escape && *pat == '*') {
                locp = ++pat;
                locs = str;
                if (*pat == '\0') {
                    return true;
                }
                continue;
            }
            /* we have a mismatch */
            if (std::tolower(static_cast<unsigned char>(*str)) != std::tolower(static_cast<unsigned char>(*pat))
                && (escape || *pat != '?')
                ) {
                if (!locp) {
                    return false;
                }
                str = ++locs;
                pat = locp;
                continue;
            }
            pat++, str++;
        }
        /* check if the pattern's ended */
        while (*pat == '*') {
            pat++;
        }
        return (*pat == '\0');
    }

    bool ConditionPatternMatch(const char* pat, const char* str)
    {
        const char* locp = nullptr;
        const char* locs = nullptr;
        while (*str) {
            /* we encounter a star */
            if (*pat == '*') {
                locp = ++pat;
                locs = str;
                if (*pat == '\0') {
                    return true;
                }
                continue;
            }
            /* we have a mismatch */
            if (*str != *pat) {
                if (!locp) {
                    return false;
                }
                str = ++locs;
                pat = locp;
                continue;
            }
            pat++, str++;
        }
        /* check if the pattern's ended */
        while (*pat == '*') {
            pat++;
        }
        return (*pat == '\0');
    }
    std::string ToString(const c4::csubstr& str)
    {
        return std::string(str.str, str.len);
    }

    std::string ToString(const c4::substr& str)
    {
        return std::string(str.str, str.len);
    }
}