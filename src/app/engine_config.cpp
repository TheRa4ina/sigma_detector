#include "engine_config.h"
#include <filesystem>
#include <fstream>

namespace bl {

    namespace fs = std::filesystem;

    EngineConfig::EngineConfig(const fs::path& configPath)
        : m_configPath(configPath)
    {
        EnsureConfigExists();
        Load();
    }

    std::unordered_set<fs::path> EngineConfig::GetRulePaths() {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_rulePaths;
    }

    void EngineConfig::SetRulePaths(const std::vector<fs::path>& paths) {
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_rulePaths.clear();
            m_rulePaths.insert(paths.begin(), paths.end());
        }
        Save();
    }

    void EngineConfig::AddRulePath(const fs::path& path) {
        bool inserted = false;
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            inserted = m_rulePaths.insert(path).second;
        }
        if (inserted) Save();
    }

    void EngineConfig::AddRulePaths(const std::vector<fs::path>& paths) {
        bool anyInserted = false;
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            for (const auto& path : paths) {
                anyInserted |= m_rulePaths.insert(path).second;
            }
        }
        if (anyInserted) Save();
    }

    void EngineConfig::RemoveRulePath(const fs::path& path) {
        bool erased = false;
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            erased = m_rulePaths.erase(path) > 0;
        }
        if (erased) Save();
    }

    void EngineConfig::RemoveRulePaths(const std::vector<fs::path>& paths) {
        bool anyErased = false;
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            for (const auto& path : paths) {
                anyErased |= m_rulePaths.erase(path) > 0;
            }
        }
        if (anyErased) Save();
    }

    void EngineConfig::EnsureConfigExists() {
        if (!fs::exists(m_configPath)) {
            std::ofstream file(m_configPath); // create empty file
        }
    }

    void EngineConfig::Load() {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_rulePaths.clear();
        std::ifstream file(m_configPath);
        if (!file.is_open()) return;

        std::string line;
        while (std::getline(file, line)) {
            if (!line.empty()) {
                m_rulePaths.insert(fs::path(line));
            }
        }
    }

    void EngineConfig::Save() {
        std::unordered_set<fs::path> copy;
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            copy = m_rulePaths;
        }

        std::ofstream file(m_configPath, std::ios::trunc);
        if (!file.is_open()) return;

        for (const auto& path : copy) {
            file << path.string() << '\n';
        }
    }

} // namespace bl