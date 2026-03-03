#pragma once 
#include <cstdint>
#include <sigma/rule_parser/mod_types.h>

namespace sigma {
    struct SelectorContext;

    void ApplyMod(SelectorContext& ctx, Modifier m);
}