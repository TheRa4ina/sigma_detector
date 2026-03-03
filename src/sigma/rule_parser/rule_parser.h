#pragma once
#include <ryml.hpp>
#include <string>
#include <sigma/rule_parser/selector_factory.h>

namespace sigma {
class Rule;

    class RuleParser
    {
    public:
        RuleParser();
        Rule Parse(std::string rawRule);
    private:
        ryml::EventHandlerTree m_evtHandler;
        ryml::Parser m_parser;
        ryml::Tree m_ruleTree;
        SelectorFactory m_selectorFactory;
    };

} // sigma