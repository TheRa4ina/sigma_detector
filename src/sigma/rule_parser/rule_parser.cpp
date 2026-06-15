#include "rule_parser.h"
#include <rule_parser/utilities.h>
#include <rule_parser/logsource.h>
#include <rule_parser/rule.h>
#include <ryml_std.hpp>
#include <format>
namespace sigma {

    namespace {
        std::vector<std::string> parseValues(c4::yml::ConstNodeRef& selectorYaml) {
            std::vector<std::string> vals;
            if (selectorYaml.is_keyval()) {
                c4::csubstr rawVal = selectorYaml.val();
                std::string val = utils::ToString(rawVal);
                vals.push_back(val);
            }
            else if (selectorYaml.is_seq()) {
                for (const auto& valYaml : selectorYaml) {
                    c4::csubstr rawVal = valYaml.val();
                    std::string val = utils::ToString(rawVal);

                    vals.push_back(val);
                }
            }
            else {
                throw std::runtime_error("selector yaml is nor keyval or seq");
            }
            return vals;
        }
    }

    RuleParser::RuleParser()
        : m_evtHandler(), m_parser(&m_evtHandler)
    {
        m_ruleTree.reserve(30);
        m_ruleTree.reserve_arena(1024);
    }

    Rule RuleParser::Parse(std::string rawRule)
    {
        m_ruleTree.clear();
        ryml::parse_in_place(&m_parser, ryml::to_substr(rawRule), m_ruleTree);
        auto selectionsYaml = m_ruleTree["detection"];
        assert(selectionsYaml.is_map());
        size_t childNum = selectionsYaml.num_children();
        size_t selectorsNum = childNum - 1;
        Selections selections;

        for (size_t i = 0; i < selectorsNum; i++) {
            const ryml::ConstNodeRef& selectionYaml = selectionsYaml.child(i);
            std::vector<std::unique_ptr<ISelectorBase>> selectors;

            bool isAll = selectionYaml.is_map();

            for (auto selectorYaml : selectionYaml) {
                if (!isAll) {
                    if (selectorYaml.num_children() != 1) {
                        throw std::runtime_error(std::format(
                            "single list item should contain one child. Current amount is [{}]"
                            , selectorYaml.num_children()));
                    }
                    selectorYaml = selectorYaml.child(0);
                }
                c4::csubstr key = selectorYaml.key();
                auto keyParts = key.split('|');

                auto fieldName = utils::ToString(*keyParts.begin());

                std::vector<std::string> modifiers;
                for (auto it = ++(keyParts.begin()); it != keyParts.end(); it++) {
                    c4::csubstr mod = *it;
                    modifiers.push_back(utils::ToString(mod));
                }

                std::vector<std::string> vals = parseValues(selectorYaml);

                std::vector<sigma::Modifier> mods;
                for (const auto& mod : modifiers) {
                    mods.push_back(ModUtils::FromString(mod));
                }

                selectors.emplace_back(m_selectorFactory.Create(fieldName, mods, vals) );
            }

            std::string selectionName = utils::ToString(selectionYaml.key());
            selections.emplace_back(SelectionFactory::Create(selectionName,std::move(selectors), isAll));
        }
        auto condition = selectionsYaml.child(childNum - 1);

        LogSource logsource;
        auto logsourceYaml = m_ruleTree["logsource"];
        if(logsourceYaml.has_child("product"))
             logsourceYaml.at("product") >> logsource.product;
        if(logsourceYaml.has_child("category"))
            logsourceYaml.at("category") >> logsource.category;
        if(logsourceYaml.has_child("service"))
            logsourceYaml.at("service") >> logsource.service;

        RuleMeta meta;

        ryml::id_type rootNodeId = m_ruleTree.root_id();
        if(!m_ruleTree.has_child(rootNodeId, "id"))
        {
            throw std::runtime_error("rule missing required field: id");
        }
        m_ruleTree["id"] >> meta.id;

        return Rule(std::move(selections), utils::ToString(condition.val()), logsource, meta);
    }

} // sigma