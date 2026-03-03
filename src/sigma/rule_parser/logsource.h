#pragma once
#include <unordered_map>
#include <vector>
#include <string>

namespace sigma {

    struct EarlyFilter {
        std::string key;
        std::vector<std::string> possibleValues;
    };
    using EarlyFilters = std::vector<EarlyFilter>;

    struct LogSource {
        std::string product;
        std::string category;
        std::string service;
    };

    // Constants
    inline const std::string EventID = "EventID";
    inline const EarlyFilter SYSMON_CHANNEL_FILTER_WIN = { "Channel", {"Microsoft-Windows-Sysmon/Operational"} };

    // Function declarations
    EarlyFilters SysmonFilterEvents(const std::vector<std::string>& ids);
    EarlyFilters SearchByService(const std::string& product, const std::string& service);
    EarlyFilters SearchByCategory(const std::string& product, const std::string& category);
    EarlyFilters GetEarlyFilters(const LogSource& logsource);

} // namespace sigma
