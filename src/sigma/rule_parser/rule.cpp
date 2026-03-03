#include "rule.h"
#include <algorithm>
#include <rule_parser/condition.h>
#include <ryml.hpp>
#include <ryml_std.hpp>

#include <iostream>

// ts is ugly
// TODO remake into more readable variant
// maybe make it clear that m_selectors is moved


namespace sigma {

    Rule::Rule(Selections&& selections, std::string condition, LogSource logsource, RuleMeta meta)
        : m_selections{ std::move(selections) }, m_logsource{ logsource }, m_condition{ condition }, m_meta(meta)
    {
        m_filters = sigma::GetEarlyFilters(logsource);
    }


    bool Rule::Match(const Event& event) const
    {
        bool passedFilter = std::all_of(m_filters.begin(), m_filters.end(), [&event](EarlyFilter filter) {
            std::string fieldValue = event.fields.at(filter.key);
            return std::any_of(filter.possibleValues.begin(), filter.possibleValues.end(), [&fieldValue](std::string possibleValue) {
                return fieldValue == possibleValue;
                });
            });
        return m_condition.Evaluate(m_selections, event);
    }

    RuleMeta Rule::GetMeta() const
    {
        return m_meta;
    }

} //sigma