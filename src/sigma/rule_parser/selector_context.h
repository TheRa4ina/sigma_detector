#pragma once
#include <sigma/rule_parser/modifier.h>
#include <sigma/rule_parser/predicate.h>
#include <vector>

namespace sigma {
    enum class GroupingMode {
        FlatAny,
        FlatAll,
        Grouped
    };

    struct SelectorContext {
        std::string field;
        ModType type;
        GroupingMode grouping;
        bool isExpanded = false;

        std::unique_ptr<IPredicate> predicate;
        std::vector<std::vector<std::string>> groupedValues;

        SelectorContext(
            const std::string& f,
            const std::vector<std::string>& initialValues,
            ModType initialType = ModType::Generic
        )
            : field(f), type(initialType), grouping(GroupingMode::FlatAny)
        {
            groupedValues.reserve(initialValues.size());
            for (auto& v : initialValues)
                groupedValues.push_back({ v });
            predicate = std::make_unique<WildcardPredicate>();
        }
    };
} // sigma