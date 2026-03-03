#pragma once
#include <string>
#include <sigma/rule_parser/modifier.h>
#include <sigma/rule_parser/selector.h>
#include <memory>

namespace sigma {
    class SelectorFactory {
    public:
        SelectorFactory() = default;

        std::unique_ptr<ISelectorBase> Create(
            const std::string& field,
            const std::vector<Modifier>& mods,
            const std::vector<std::string>& values,
            const sigma::ModType initialModType = ModType::Generic) const;
    };

} // sigma