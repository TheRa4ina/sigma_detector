#pragma once
#include <vector>
#include <string>
#include <memory>
#include <functional>
#include <sigma/rule_parser/modifier.h>
#include <sigma/rule_parser/mod_utils.h>

namespace sigma {
    struct MatchesAny {
        template<typename Range, typename F>
        static bool match(const Range& r, F&& f) {
            return std::any_of(r.begin(), r.end(), std::forward<F>(f));
        }
    };

    struct MatchesAll {
        template<typename Range, typename F>
        static bool match(const Range& r, F&& f) {
            return std::all_of(r.begin(), r.end(), std::forward<F>(f));
        }
    };

    class ISelectorBase {
    public:
        ISelectorBase(const ISelectorBase&) = delete;
        ISelectorBase& operator=(const ISelectorBase&) = delete;
        ISelectorBase(ISelectorBase&&) noexcept = default;
        ISelectorBase& operator=(ISelectorBase&&) noexcept = default;

        virtual ~ISelectorBase() = default;
        virtual bool Match(const std::string& input) const = 0;
        const std::string& GetFieldName() const { return fieldName; }

    protected:
        ISelectorBase(std::string field, std::unique_ptr<IPredicate> pred)
            : fieldName(std::move(field)), predicate(std::move(pred)) {
        }

        std::string fieldName;
        std::unique_ptr<IPredicate> predicate;
    };

    template<typename Policy>
    class Selector : public ISelectorBase {
    public:
        Selector(std::string field, std::vector<std::string> vals, std::unique_ptr<IPredicate> pred)
            : ISelectorBase(std::move(field), std::move(pred)), values(std::move(vals)) {
        }

        Selector(const Selector&) = delete;
        Selector& operator=(const Selector&) = delete;
        Selector(Selector&&) noexcept = default;
        Selector& operator=(Selector&&) noexcept = default;

        bool Match(const std::string& input) const override {
            return Policy::match(values, [&](const auto& v) { return predicate->Match(v, input); });
        }

    private:
        std::vector<std::string> values;
    };

    class GroupedSelector : public ISelectorBase {
    public:
        GroupedSelector(std::string field, std::vector<std::vector<std::string>> grps, std::unique_ptr<IPredicate> pred)
            : ISelectorBase(std::move(field), std::move(pred)), groups(std::move(grps)) {
        }

        GroupedSelector(const GroupedSelector&) = delete;
        GroupedSelector& operator=(const GroupedSelector&) = delete;
        GroupedSelector(GroupedSelector&&) noexcept = default;
        GroupedSelector& operator=(GroupedSelector&&) noexcept = default;

        bool Match(const std::string& input) const override {
            return MatchesAll::match(groups, [&](const auto& group) {
                return MatchesAny::match(group, [&](const auto& v) { return predicate->Match(v, input); });
                });
        }

    private:
        std::vector<std::vector<std::string>> groups;
    };

} // namespace sigma
