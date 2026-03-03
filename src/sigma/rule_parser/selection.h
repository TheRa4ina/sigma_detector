#pragma once

#include <sigma/rule_parser/modifier.h>
#include <sigma/rule_parser/mod_utils.h>
#include <sigma/rule_parser/selector.h>
#include <event/event.h>



namespace sigma {
    class ISelection {
    public:
        ISelection() = default; // <- add this
        ISelection(const ISelection&) = delete;
        ISelection& operator=(const ISelection&) = delete;
        ISelection(ISelection&&) noexcept = default;
        ISelection& operator=(ISelection&&) noexcept = default;

        virtual ~ISelection() = default;
        virtual bool Match(const Event& event) const = 0;
        virtual std::string GetName() const = 0;
    };

    template<typename Policy>
    class Selection : public ISelection {
    public:
        Selection(std::string name, std::vector<std::unique_ptr<ISelectorBase>>&& selectors)
            : m_name(std::move(name)), m_selectors(std::move(selectors)) {
        }

        bool Match(const Event& event) const override {
            return !m_selectors.empty() &&
                Policy::match(m_selectors, [&event](const std::unique_ptr<ISelectorBase>& s) {
                std::string fieldName = s->GetFieldName();
                auto it = event.fields.find(fieldName);
                if (it == event.fields.end()) {
                    return false;
                }

                return s->Match(event.fields.at(s->GetFieldName()));
                    });
        }

        std::string GetName() const override { return m_name; }

    private:
        std::vector<std::unique_ptr<ISelectorBase>> m_selectors;
        std::string m_name;
    };

    using SelectionPtr = std::unique_ptr<ISelection>;
    using Selections = std::vector<SelectionPtr>;

    namespace SelectionFactory {
        std::unique_ptr<ISelection> Create(std::string name, std::vector<std::unique_ptr<ISelectorBase>> selectors, bool isAll);
    } // SelectionFactory
} // sigma