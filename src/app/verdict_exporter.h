#pragma once
#include <vector>
#include <filesystem>
#include <sigma/engine.h>

namespace bl::exporter {

    bool ExportJSON(const std::filesystem::path& file,
        const std::vector<sigma::Verdict>& verdicts);

}