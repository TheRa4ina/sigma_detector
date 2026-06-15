#include <sigma/rule_parser/selector_factory.h>
#include <sigma/rule_parser/selector_context.h>
#include <sigma/rule_parser/modifier.h>
#include <sigma/rule_parser/selector.h>

namespace sigma {
    namespace {
        std::vector<std::string> FlattenGroups(const std::vector<std::vector<std::string>>& groups) {
            std::vector<std::string> flat;
            for (auto& g : groups) flat.insert(flat.end(), g.begin(), g.end());
            return flat;
        }
    }

    std::unique_ptr<ISelectorBase> sigma::SelectorFactory::Create(const std::string& field, const std::vector<Modifier>& mods, const std::vector<std::string>& values, const sigma::ModType initialModType) const
    {
        SelectorContext ctx(field, values);
        ctx.type = initialModType;

        bool isAll = false;
        bool isExpanded = false;

        for (const auto& m : mods)
        {
            ModType modType = ModUtils::GetModType(m);
            if (!ModUtils::IsCompatibleModType(ctx.type, modType))
            {
                throw std::runtime_error("selector type: \"" + ModUtils::ToString(ctx.type) +
                    "is incompatible with mod: \"" + ModUtils::ToString(m) +
                    "\" of type: \"" + ModUtils::ToString(modType) + "\"");
            }
            if (modType != ModType::Generic) {
                ctx.type = modType;
            }

            ApplyMod(ctx, m);
        }

        switch (ctx.grouping) {
        case GroupingMode::Grouped:
            return std::make_unique<GroupedSelector>(
                ctx.field,
                ctx.groupedValues,
                std::move(ctx.predicate)
            );

        case GroupingMode::FlatAll:
            return std::make_unique<Selector<MatchesAll>>(
                ctx.field,
                FlattenGroups(ctx.groupedValues),
                std::move(ctx.predicate)
            );

        case GroupingMode::FlatAny:
        default:
            return std::make_unique<Selector<MatchesAny>>(
                ctx.field,
                FlattenGroups(ctx.groupedValues),
                std::move(ctx.predicate)
            );
        }
    }

} // sigma