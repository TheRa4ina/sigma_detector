#include <rule_parser/mod_utils.h>
#include <rule_parser/encoding_utils.h>
#include <rule_parser/utilities.h>
#include <base64.hpp>
#include <unordered_map>
#include <memory>
#include <string>
#include <stdexcept>
#include <c4/substr.hpp>
#include <c4/std/string.hpp>
#include <c4/charconv.hpp>
#include <cstdint>
namespace sigma {


sigma::Modifier ModUtils::FromString(const std::string& s) {
    static const std::unordered_map<std::string, sigma::Modifier> map = {
        {"all",         Modifier::All},
        {"startswith",  Modifier::StartsWith},
        {"endswith",    Modifier::EndsWith},
        {"contains",    Modifier::Contains},
        {"exists",      Modifier::Exists},
        {"cased",       Modifier::Cased},
        {"neq",         Modifier::Neq},
        {"windash",     Modifier::WinDash},
        {"re",          Modifier::Re},
        {"i",           Modifier::I},
        {"m",           Modifier::M},
        {"s",           Modifier::S},
        {"base64",      Modifier::Base64},
        {"base64offset",Modifier::Base64Offset},
        {"utf16le",     Modifier::Utf16Le},
        {"utf16be",     Modifier::Utf16Be},
        {"utf16",       Modifier::Utf16},
        {"wide",        Modifier::Wide},
        {"lt",          Modifier::Lt},
        {"lte",         Modifier::Lte},
        {"gt",          Modifier::Gt},
        {"gte",         Modifier::Gte},
        {"minute",      Modifier::Minute},
        {"hour",        Modifier::Hour},
        {"day",         Modifier::Day},
        {"week",        Modifier::Week},
        {"month",       Modifier::Month},
        {"year",        Modifier::Year},
        {"cidr",        Modifier::Cidr}
    };

    auto it = map.find(s);
    if (it == map.end()) throw std::invalid_argument("Unknown Modifier string: \"" + s + "\"");
    return it->second;
}

std::string ModUtils::ToString(Modifier mod)
{
    switch (mod) {
    case Modifier::All:         return "All";
    case Modifier::StartsWith:  return "StartsWith";
    case Modifier::EndsWith:    return "EndsWith";
    case Modifier::Contains:    return "Contains";
    case Modifier::Exists:      return "Exists";
    case Modifier::Cased:       return "Cased";
    case Modifier::Neq:         return "Neq";
    case Modifier::WinDash:     return "WinDash";
    case Modifier::Re:          return "Re";
    case Modifier::I:           return "I";
    case Modifier::M:           return "M";
    case Modifier::S:           return "S";
    case Modifier::Base64:      return "Base64";
    case Modifier::Base64Offset:return "Base64Offset";
    case Modifier::Utf16Le:     return "Utf16Le";
    case Modifier::Utf16Be:     return "Utf16Be";
    case Modifier::Utf16:       return "Utf16";
    case Modifier::Wide:        return "Wide";
    case Modifier::Lt:          return "Lt";
    case Modifier::Lte:         return "Lte";
    case Modifier::Gt:          return "Gt";
    case Modifier::Gte:         return "Gte";
    case Modifier::Minute:      return "Minute";
    case Modifier::Hour:        return "Hour";
    case Modifier::Day:         return "Day";
    case Modifier::Week:        return "Week";
    case Modifier::Month:       return "Month";
    case Modifier::Year:        return "Year";
    case Modifier::Cidr:        return "Cidr";
    default: return "Unknown Modifier(" + std::to_string(static_cast<int32_t>(mod))+")";
    }
}
std::string ModUtils::ToString(ModType type)
{
    switch (type) {
    case ModType::Generic:      return "Generic";
    case ModType::String:       return "String";
    case ModType::Regular:      return "Regular";
    case ModType::RegularSub:   return "RegularSub";
    case ModType::Encoding:     return "Encoding";
    case ModType::Numeric:      return "Numeric";
    case ModType::Time:         return "Time";
    case ModType::Ip:           return "Ip";
    case ModType::Specific:     return "Specific";
    default: return "Unknown ModType(" + std::to_string(static_cast<int32_t>(type))+")";
    }
}

ModType ModUtils::GetModType(Modifier m)
{
    const static std::unordered_map<Modifier, ModType> modTypeMap{
        { Modifier::All,         ModType::Generic },
        { Modifier::StartsWith,  ModType::Generic },
        { Modifier::EndsWith,    ModType::Generic },
        { Modifier::Contains,    ModType::Generic },
        { Modifier::Exists,      ModType::Generic },
        { Modifier::Cased,       ModType::Generic },
        { Modifier::Neq,         ModType::Generic },
        { Modifier::WinDash,     ModType::String },
        { Modifier::Re,          ModType::Regular },
        { Modifier::I,           ModType::RegularSub },
        { Modifier::M,           ModType::RegularSub },
        { Modifier::S,           ModType::RegularSub },
        { Modifier::Base64,      ModType::Encoding },
        { Modifier::Base64Offset,ModType::Encoding },
        { Modifier::Utf16Le,     ModType::Encoding },
        { Modifier::Utf16Be,     ModType::Encoding },
        { Modifier::Utf16,       ModType::Encoding },
        { Modifier::Wide,        ModType::Encoding },
        { Modifier::Lt,          ModType::Numeric},
        { Modifier::Lte,         ModType::Numeric},
        { Modifier::Gt,          ModType::Numeric},
        { Modifier::Gte,         ModType::Numeric},
        { Modifier::Minute,      ModType::Time},
        { Modifier::Hour,        ModType::Time},
        { Modifier::Day,         ModType::Time},
        { Modifier::Week,        ModType::Time},
        { Modifier::Month,       ModType::Time},
        { Modifier::Year,        ModType::Time},
        { Modifier::Cidr,        ModType::Ip }
    };

    auto it = modTypeMap.find(m);
    if (it == modTypeMap.end()) throw std::invalid_argument("Unknown Modifier enum: " + ToString(m));
    return it->second;
}

bool ModUtils::IsCompatibleModType(ModType selectorType, ModType modType)
{
    if  (  (selectorType == ModType::Generic    || modType == ModType::Generic ) 
        || (selectorType == ModType::String     && modType == ModType::String  )
        || (selectorType == ModType::String     && modType == ModType::Encoding)
        || (selectorType == ModType::String     && modType == ModType::Regular )
        || (selectorType == ModType::Regular    && modType == ModType::RegularSub)
        || (selectorType == ModType::RegularSub && modType == ModType::RegularSub)
        )
    {
        return true;
    }
    else return false;
}

bool ModUtils::IsExpandable(Modifier mod)
{
    return mod == Modifier::Base64Offset || mod == Modifier::WinDash;
}
} // sigma