#pragma once
#include <vector>
#include <string>
#include <functional>
#include <unordered_set>
#include <sigma/rule_parser/logsource.h>
#include <sigma/rule_parser/selection.h>
#include <sigma/rule_parser/condition.h>

namespace sigma {

    struct RuleMeta
    {
        std::string id;
    };
    //https://github.com/SigmaHQ/sigma-specification/blob/v2.1.0/specification/sigma-appendix-modifiers.md#sigma-modifiers

    class Rule {
    public:
        Rule(const Rule&) = delete;
        Rule& operator=(const Rule&) = delete;
        Rule(Rule&&) noexcept = default;
        Rule& operator=(Rule&&) noexcept = default;

        Rule(Selections&& selections, std::string condition, LogSource logsource, RuleMeta meta);
        bool Match(const Event& event) const;
        RuleMeta GetMeta() const;

    private:

        Selections m_selections;
        LogSource m_logsource;
        EarlyFilters m_filters;
        Condition m_condition;
        RuleMeta m_meta;
    };

} // sigma