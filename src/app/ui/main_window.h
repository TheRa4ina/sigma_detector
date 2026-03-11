#pragma once
#include <app/ui/rule_listbox.h>
#include <app/ui/verdict_listbox.h>
#include <app/engine_config.h>
#include <sigma/engine.h>
#include <memory>
#include <string>
#include <vector>

namespace ImGui {
    class FileBrowser;
}

namespace ui {

    class MainWindow {
    public:
        MainWindow(std::shared_ptr<sigma::Engine> engine);
        
        void Show();

    private:

        enum class FileBrowserPurpose {
            AddRule,
            ExportVerdicts
        };

        std::string m_errorMessage;

        std::string m_viewerBuffer;
        std::shared_ptr<sigma::Engine> m_engine;// Todo pretty sure can make it unique ptr
        std::shared_ptr<bl::EngineConfig> m_config;
        std::shared_ptr<ImGui::FileBrowser> m_fileBrowser;// forced to use shared ptr because whole implementation is in header
        FileBrowserPurpose m_fileBrowserPurpose = FileBrowserPurpose::AddRule;

        RuleListBox m_ruleListBox;
        VerdictListBox m_verdictListBox;
    };

} // ui