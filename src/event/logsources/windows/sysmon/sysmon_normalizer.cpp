#include <event/event.h>
#include <windows.h>
#include <pugixml.hpp>
#include <iostream>
#include "sysmon_normalizer.h"
#define TRACE_PREFIX "Sysmon normalizer:\t"
#include <quill/logging.h>


namespace {
    static std::string WideToUtf8(const std::wstring& w)
    {
        if (w.empty()) return {};
        int size_needed = WideCharToMultiByte(CP_UTF8, 0, w.data(), (int)w.size(), nullptr, 0, nullptr, nullptr);
        if (size_needed <= 0) return {};
        std::string out(size_needed, 0);
        WideCharToMultiByte(CP_UTF8, 0, w.data(), (int)w.size(), &out[0], size_needed, nullptr, nullptr);
        return out;
    }
}

namespace channel {

    Event SysmonNormalizer::Normalize(RawEvent xmlData)
    {
        Event event;
        event.channel = Channel::Sysmon;
        try {
            if (xmlData.empty()) {
                throw std::invalid_argument("xmlData for sysmonEvent arrived empty\n");
            }

            pugi::xml_document doc;
            pugi::xml_parse_result result = doc.load_string(xmlData.c_str());

            if (!result) {
                throw std::invalid_argument(result.description());
            }

            // print full data (convert once for output)
            std::wstring wfull(xmlData);

            pugi::xml_node systemNode = doc.child(L"Event").child(L"System");
            if (systemNode) {
                for (const auto& child : systemNode.children()) {
                    std::string name = WideToUtf8(child.name());
                    if (!child.children().empty()) {
                        std::string value = WideToUtf8(child.child_value());
                        event.fields[name] = value;
                    }
                    for (const auto& attr : child.attributes()) {
                        std::string attributeName = name;
                        attributeName.append("_").append(WideToUtf8(attr.name()));
                        std::string attributeValue = WideToUtf8(attr.value());
                        event.fields[(attributeName)] = (attributeValue);
                    }
                }
            }

            pugi::xml_node EventDataNode = doc.child(L"Event").child(L"EventData");
            if (EventDataNode) {
                for (const auto& child : EventDataNode.children(L"Data")) {
                    std::string name = WideToUtf8(child.attribute(L"Name").as_string());
                    std::string value = WideToUtf8(child.child_value());
                    event.fields[name] = value;
                }
            }
        }
        catch (const std::exception& e) {
            LOG_ERROR("{}",e.what());
        }
        return event;
    }

} // channel