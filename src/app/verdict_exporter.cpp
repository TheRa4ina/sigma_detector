#include "verdict_exporter.h"
#include <ryml.hpp>
#include <ryml_std.hpp>

namespace bl::exporter {

    bool ExportJSON(const std::filesystem::path& file,
        const std::vector<sigma::Verdict>& verdicts)
    {
        ryml::Tree tree;
        auto root = tree.rootref();
        root |= ryml::SEQ;

        for (const auto& v : verdicts)
        {
            auto node = root.append_child();
            node |= ryml::MAP;

            node["event_id"] << v.event_id;
            node["rule_matched"] << v.rule_matched;
            node["result"] << v.result;

            auto payload = node["payload"];
            payload |= ryml::MAP;

            payload["channel"] << static_cast<int>(v.payload.channel);

            auto fields = payload["fields"];
            fields |= ryml::MAP;

            for (const auto& [k, val] : v.payload.fields)
            {
                fields[c4::to_csubstr(k)] << val;
            }
        }

        FILE* f = fopen(file.string().c_str(), "wb");
        if (!f)
            return false;

        ryml::emit_json(tree, f);
        fclose(f);

        return true;
    }

}