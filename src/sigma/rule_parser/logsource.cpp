#include "logsource.h"

namespace sigma {

    static const std::unordered_map<std::string, std::unordered_map<std::string, EarlyFilters>> SERVICE_MAP = {
        {"windows", {{"sysmon", {SYSMON_CHANNEL_FILTER_WIN}}}},
    };

    static const std::unordered_map<std::string, std::unordered_map<std::string, EarlyFilters>> CATEGORY_MAP = {
    {"windows", {
        {"process_creation",    SysmonFilterEvents({"1"}) },
        {"file_change",         SysmonFilterEvents({"2"}) },
        {"network_connection",  SysmonFilterEvents({"3"}) },
        {"sysmon_status",       SysmonFilterEvents({"4","16"}) },
        {"process_termination", SysmonFilterEvents({"5"}) },
        {"driver_load",         SysmonFilterEvents({"6"}) },
        {"image_load",          SysmonFilterEvents({"7"}) },
        {"create_remote_thread",SysmonFilterEvents({"8"}) },
        {"process_access",      SysmonFilterEvents({"10"}) },
        {"registry_event",      SysmonFilterEvents({"12","13","14"}) },
        {"dns_query",           SysmonFilterEvents({"22"}) },
        {"file_delete",         SysmonFilterEvents({"23"}) },
        {"clipboard_capture",   SysmonFilterEvents({"24"}) },
        {"sysmon_error",        SysmonFilterEvents({"25"}) },
        {"ps_classic_start", {
            {"Channel", {"Windows PowerShell"}},
            {EventID, {"400"}}
        }},
        {"ps_classic_script", {
            {"Channel", {"Windows PowerShell"}},
            {EventID, {"800"}}
        }},
        {"ps_script", {
            {"Channel", {"Microsoft-Windows-PowerShell/Operational", "PowerShellCore/Operational"}},
            {EventID, {"4104"}}
        }},
        {"file_access", {
            {"ETW Provider", {"Microsoft-Windows-Kernel-File"}}
        }},
        {"file_rename", {
            {"ETW Provider", {"Microsoft-Windows-Kernel-File"}}
        }},
    }}
    };

    EarlyFilters SysmonFilterEvents(const std::vector<std::string>& ids) {
        return { SYSMON_CHANNEL_FILTER_WIN, {EventID, ids } };
    }

    inline const EarlyFilters& findOrEmpty(
        const std::unordered_map<std::string, std::unordered_map<std::string, EarlyFilters>>& outer,
        const std::string& k1, const std::string& k2)
    {
        static const EarlyFilters empty{};
        auto outerIt = outer.find(k1);
        if (outerIt == outer.end()) return empty;
        auto innerIt = outerIt->second.find(k2);
        if (innerIt == outerIt->second.end()) return empty;
        return innerIt->second;
    }

    EarlyFilters SearchByService(const std::string& product, const std::string& service) {
        return findOrEmpty(SERVICE_MAP, product, service);
    }

    EarlyFilters SearchByCategory(const std::string& product, const std::string& category) {
        return findOrEmpty(CATEGORY_MAP, product, category);
    }

    EarlyFilters GetEarlyFilters(const LogSource& logsource) {
        if (!logsource.service.empty())
            return SearchByService(logsource.product, logsource.service);
        if (!logsource.category.empty())
            return SearchByCategory(logsource.product, logsource.category);
        return {};
    }

} // namespace sigma
