#ifndef ENGINE_CONFIG_H
#define ENGINE_CONFIG_H

#include <filesystem>
#include <unordered_set>
#include <mutex>
#include <vector>

namespace bl {

    class EngineConfig {
    public:
        EngineConfig(const std::filesystem::path& configPath = "engine_rules.txt");

        std::unordered_set<std::filesystem::path> GetRulePaths();

        void SetRulePaths(const std::vector<std::filesystem::path>& paths);
        void AddRulePath(const std::filesystem::path& path);
        void AddRulePaths(const std::vector<std::filesystem::path>& paths);
        void RemoveRulePath(const std::filesystem::path& path);
        void RemoveRulePaths(const std::vector<std::filesystem::path>& paths);

    private:
        void EnsureConfigExists();
        void Load();
        void Save();

        std::filesystem::path m_configPath;
        std::unordered_set<std::filesystem::path> m_rulePaths;
        std::mutex m_mutex;
    };

} // namespace bl

#endif // ENGINE_CONFIG_H