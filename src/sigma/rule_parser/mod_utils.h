#pragma once
#include <sigma/rule_parser/mod_types.h>
#include <sigma/rule_parser/modifier.h>
#include <sigma/rule_parser/predicate.h>
#include <functional>
#include <unordered_set>
#include <string>

namespace sigma {

    namespace ModUtils {
        std::string ToString(Modifier mod);
        std::string ToString(ModType type);

        ModType GetModType(Modifier mod);
        Modifier FromString(const std::string& s);

        bool IsCompatibleModType(ModType selectorType, ModType modType);
        bool IsExpandable(Modifier mod);
    } // ModUtils
} // sigma