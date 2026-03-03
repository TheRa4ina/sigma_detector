#include <sigma/rule_parser/modifier.h>
#include <sigma/rule_parser/mod_types.h>
#include <sigma/rule_parser/mod_utils.h>
#include <sigma/rule_parser/encoding_utils.h>
#include <sigma/rule_parser/utilities.h>
#include <sigma/rule_parser/selector_context.h>
#include <base64.hpp>
#include <unordered_map>
#include <memory>
#include <string>
#include <stdexcept>
#include <cstdint>
#include <c4/substr.hpp>
#include <c4/std/string.hpp>
#include <c4/charconv.hpp>

namespace sigma {
    namespace {
        std::vector<std::string> ModWindash(const std::string& line) {
            static const std::vector<std::string> dashChars = {
                "-",            // U+002D hyphen-minus
                "/",            // U+002F solidus
                "\xE2\x80\x93", // U+2013 EN DASH
                "\xE2\x80\x94", // U+2014 EM DASH
                "\xE2\x80\x95"  // U+2015 HORIZONTAL BAR
            };

            std::vector<std::size_t> positions;
            std::string tempLine = line;

            for (std::size_t i = 0; i < tempLine.size(); ) {
                bool matched = false;

                for (const auto& dash : dashChars) {
                    if (tempLine.compare(i, dash.size(), dash) == 0) {
                        // Only count if it is at start or preceded by a space
                        if (i == 0 || isspace(static_cast<unsigned char>(tempLine[i - 1]))) {
                            positions.push_back(i);
                        }
                        i += dash.size();
                        matched = true;
                        break;
                    }
                }

                if (!matched) ++i;
            }

            std::size_t count = positions.size();
            if (count == 0) return { line };
            if (count > 5) throw std::runtime_error("Too many dash characters for WinDash modifier");

            std::vector<std::string> lines;
            std::size_t base = dashChars.size();
            std::size_t total = 1;
            for (std::size_t i = 0; i < count; ++i) total *= base;

            for (std::size_t idx = 0; idx < total; ++idx) {
                std::string temp = line;
                std::size_t x = idx;
                for (std::size_t i = 0; i < count; ++i) {
                    std::size_t choice = x % base;
                    x /= base;
                    temp.replace(positions[i], dashChars[0].size(), dashChars[choice]);
                    for (std::size_t j = i + 1; j < count; ++j) {
                        if (positions[j] > positions[i]) positions[j] += dashChars[choice].size() - dashChars[0].size();
                    }
                }
                lines.push_back(temp);
            }

            return lines;
        }

        inline std::vector<std::string> ModBase64Offset(const std::string& value) {
            std::vector<std::string> lines(3, "");
            lines[0] = utils::EncodeBase64OffsetStable(value, 0);
            lines[1] = utils::EncodeBase64OffsetStable(value, 1);
            lines[2] = utils::EncodeBase64OffsetStable(value, 2);
            return lines;
        }

        inline std::string ModStartsWith(std::string value) {
            value.append("*");
            return value;
        }
        inline std::string ModEndsWith(std::string& value) {
            value.insert(0, "*");
            return value;
        }
        inline std::string ModContains(std::string& value) {
            value = '*' + value + '*';
            return value;
        }
        inline std::string ModBase64(std::string& value) {
            return utils::EncodeBase64(value);
        }
        inline std::string ModUtf16Le(std::string& value) {
            return utils::ToUtf16leBytes(utils::Utf8ToUtf16(value));
        }
        inline std::string ModUtf16Be(std::string& value) {
            return utils::ToUtf16beBytes(utils::Utf8ToUtf16(value));
        }
        inline std::string ModUtf16(std::string& value) {
            return utils::ToUtf16leBytes(utils::Utf8ToUtf16(value), true);
        }
        inline std::string ModWide(std::string& value) {
            return ModUtf16Le(value);
        }

        void ApplyToAllValues(SelectorContext& ctx, const std::function<void(std::string&)>& func) {
            for (auto& group : ctx.groupedValues)
                for (auto& value : group)
                    func(value);
        }

        void ApplyExpandableToAllGroups(
            SelectorContext& ctx,
            const std::function<std::vector<std::string>(const std::string&)>& func)
        {
            for (auto& group : ctx.groupedValues) {
                std::vector<std::string> newGroup;
                for (const auto& value : group) {
                    auto expanded = func(value);
                    newGroup.insert(newGroup.end(), expanded.begin(), expanded.end());
                }
                group = std::move(newGroup);
            }
        }

        std::unique_ptr<NumericPredicate> CreateNumericComparator(std::function<bool(double, double)> cmp) {
            return std::make_unique<NumericPredicate>(cmp);
            };
    } // namespace

    void ApplyMod(SelectorContext& ctx, Modifier m) {
        auto ApplyPredicate = [&](auto pred) { ctx.predicate = std::move(pred); };

        switch (m) {
        case Modifier::All:
            ctx.grouping = GroupingMode::FlatAll;
            break;
        case Modifier::StartsWith:
            ApplyToAllValues(ctx, ModStartsWith);
            break;
        case Modifier::EndsWith:
            ApplyToAllValues(ctx,ModEndsWith);
            break;
        case Modifier::Contains:
            ApplyToAllValues(ctx,ModContains);
            break;
        case Modifier::Base64:
            ApplyToAllValues(ctx,ModBase64);
            break;
        case Modifier::Utf16Le:
            ApplyToAllValues(ctx,ModUtf16Le);
            break;
        case Modifier::Utf16Be:
            ApplyToAllValues(ctx,ModUtf16Be);
            break;
        case Modifier::Utf16:
            ApplyToAllValues(ctx,ModUtf16);
            break;
        case Modifier::Wide:
            ApplyToAllValues(ctx,ModWide);
            break;

        case Modifier::WinDash:
            ApplyExpandableToAllGroups(ctx, ModWindash);
            break;
        case Modifier::Base64Offset:
            ApplyExpandableToAllGroups(ctx, ModBase64Offset);
            break;

        case Modifier::Neq:
            ApplyPredicate(std::make_unique<NotEqualityPredicate>(std::move(ctx.predicate)));
            break;
        case Modifier::Cased:
            ApplyPredicate(std::make_unique<CaseSensitiveWildcardPredicate>());
            break;
        case Modifier::Exists:
            ApplyPredicate(std::make_unique<ExistsPredicate>());
            break;
        case Modifier::Gt:
            ApplyPredicate(CreateNumericComparator(std::greater<double>()));
            break;
        case Modifier::Gte:
            ApplyPredicate(CreateNumericComparator(std::greater_equal<double>()));
            break;
        case Modifier::Lt:
            ApplyPredicate(CreateNumericComparator(std::less<double>()));
            break;
        case Modifier::Lte:
            ApplyPredicate(CreateNumericComparator(std::less_equal<double>()));
            break;
        case Modifier::Cidr:
            ApplyPredicate(std::make_unique<CidrPredicate>());
            break;
        default:
            throw std::runtime_error("Unsupported mod: " + std::to_string(static_cast<int32_t>(m)));
        }

        if (ModUtils::IsExpandable(m)) {
            ctx.isExpanded = true;
        }
        if (ctx.grouping == GroupingMode::FlatAll && ctx.isExpanded) {
            ctx.grouping = GroupingMode::Grouped;
        }
    }
} // sigma