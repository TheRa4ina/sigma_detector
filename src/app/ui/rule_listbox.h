#pragma once
#include "ilistbox.h"
#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>
#include <app/engine_config.h>

namespace ui {

    class RuleListBox : public IListBox<std::filesystem::path>
    {
    public:
        explicit RuleListBox(std::shared_ptr<bl::EngineConfig> engineConfig);

        void Show() override;
        void CaptureSnapshots();
        void ClearSnapshots();
        std::filesystem::path GetSelectedHandle() const override;

    protected:
        void ClearCurrentSelection() override;

    private:
        std::shared_ptr<bl::EngineConfig> m_engineConfig;
        std::filesystem::path m_selectedPath;

        std::unordered_map<std::string, std::filesystem::file_time_type> m_snapshots;
    };

} // namespace ui