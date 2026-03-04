#include "sysmon_normalizer.h"
#include <pugixml.hpp>
#include <iostream>
#define TRACE_PREFIX "Sysmon normalizer:\t"
#include <quill/logging.h>

namespace channel {

Event SysmonNormalizer::Normalize(const RawEvent& xmlData)
{
    Event event;
    event.channel = Channel::Sysmon;

    try {
        if (xmlData.empty()) {
            throw std::invalid_argument("Sysmon XML empty");
        }

        pugi::xml_document doc;
        pugi::xml_parse_result result = doc.load_string(xmlData.c_str());

        if (!result) {
            throw std::runtime_error(result.description());
        }

        pugi::xml_node eventNode = doc.child("Event");
        if (!eventNode) {
            throw std::runtime_error("Missing <Event>");
        }

        pugi::xml_node systemNode = eventNode.child("System");

        if (systemNode) {
            for (const auto& child : systemNode.children()) {

                std::string name = child.name();

                // Node value (e.g. EventID)
                if (!child.children().empty()) {
                    event.fields[name] = child.child_value();
                }

                // Attributes (Provider Name=..., TimeCreated SystemTime=...)
                for (const auto& attr : child.attributes()) {
                    std::string key = name + "_" + attr.name();
                    event.fields[key] = attr.value();
                }
            }
        }

        // -------------------------
        // EventData section
        // -------------------------
        pugi::xml_node eventDataNode = eventNode.child("EventData");

        if (eventDataNode) {
            for (const auto& dataNode : eventDataNode.children("Data")) {
                const char* nameAttr = dataNode.attribute("Name").value();
                const char* value = dataNode.child_value();

                if (nameAttr && *nameAttr) {
                    event.fields[nameAttr] = value ? value : "";
                }
            }
        }
    }
    catch (const std::exception& e) {
        LOG_ERROR("{}", e.what());
    }

    return event;
}

} // namespace channel