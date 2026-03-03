#include "main_window.h"
#include <imgui_internal.h>
#include <app/fonts/IconFontAwesome5.h>
#include <misc/cpp/imgui_stdlib.h> 
#include <imgui.h>
#include <imfilebrowser.h>
#include <string>
#include <sigma/engine.h>
#include <format>

static constexpr const char* VERDICTS = "Verdicts";
static constexpr const char* RULES = "Rules";
static constexpr const char* VIEWER = "Viewer";

namespace {
    std::string ReadFileToString(const std::filesystem::path& path) {

        std::ifstream file(path);
        if (!file.is_open()) {
            std::u8string utf8Path = path.u8string();
            auto displayPath = std::string(reinterpret_cast<char const*>(utf8Path.data()), utf8Path.size());
            throw std::runtime_error(std::format("Cannot open file: {}", displayPath));
        }

        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }

    template<typename K, typename V>
    std::string UnorderedMapToString(const std::unordered_map<K, V>& map) {
        std::stringstream ss;
        ss << "{\n";
        bool first = true;
        for (const auto& pair : map) {
            ss << pair.first << ": " << pair.second << (!first ? ',' : ' ') << '\n';
            first = false;
        }
        ss << "}";
        return ss.str();
    }
}

namespace ui
{

MainWindow::MainWindow(std::shared_ptr<sigma::Engine> engine)
    : m_engine(engine)
    , m_config(std::make_shared<bl::EngineConfig>())
    , m_verdictListBox(m_engine)
    , m_ruleListBox(m_config)
    , m_fileBrowser(std::make_shared<ImGui::FileBrowser>(ImGuiFileBrowserFlags_MultipleSelection))
{
    m_fileBrowser->SetTitle("Select a rule file");
    m_fileBrowser->SetTypeFilters({ ".yml",".yaml" });
}

void MainWindow::Show()
{
    ImGuiID dockspace_id = ImGui::GetID("DockSpace");
    ImGuiViewport* viewport = ImGui::GetMainViewport();

    if (ImGui::DockBuilderGetNode(dockspace_id) == nullptr)
    {
        ImGuiID dock_main_id = dockspace_id;
        ImGui::DockBuilderAddNode(dock_main_id, ImGuiDockNodeFlags_DockSpace);
        ImGui::DockBuilderSetNodeSize(dock_main_id, viewport->WorkSize);

        ImGuiID dock_left_id, dock_right_id, dock_center_id;
        ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Left, 0.2f, &dock_left_id, &dock_center_id);
        ImGui::DockBuilderSplitNode(dock_center_id, ImGuiDir_Right, 0.25f, &dock_right_id, &dock_center_id);

        ImGui::DockBuilderDockWindow(RULES, dock_left_id);
        ImGui::DockBuilderDockWindow(VERDICTS, dock_right_id);
        ImGui::DockBuilderDockWindow(VIEWER, dock_center_id);

        ImGui::DockBuilderFinish(dock_main_id);
    }

    ImGui::DockSpaceOverViewport(dockspace_id, viewport, ImGuiDockNodeFlags_PassthruCentralNode);
    try
    {
        ImGui::Begin(RULES);

        ImGui::BeginDisabled(m_engine->IsRunning());
        if (ImGui::Button(ICON_FA_PLUS_SQUARE)) {
            m_fileBrowser->Open();
        }
        ImGui::SetItemTooltip("Add rule from file");

        ImGui::SameLine();
        if (ImGui::Button(ICON_FA_MINUS_SQUARE)) {
            m_config->RemoveRulePath(m_ruleListBox.GetSelectedHandle());
            m_ruleListBox.ResetSelected();
        }
        ImGui::SetItemTooltip("Remove rule from list");
        ImGui::EndDisabled();

        m_ruleListBox.Show();
        ImGui::End(); // RULES

        m_fileBrowser->Display();
        if (m_fileBrowser->HasSelected())
        {
            m_config->AddRulePath(m_fileBrowser->GetSelected());
            m_fileBrowser->ClearSelected();
        }

        ImGui::Begin(VERDICTS);

        if (m_engine->IsRunning())
        {
            if (ImGui::Button(ICON_FA_STOP)) {
                m_engine->Stop();

                m_ruleListBox.ClearSnapshots();
            }
            ImGui::SetItemTooltip("Stop engine");
        }
        else
        {
            if (ImGui::Button(ICON_FA_PLAY)) {
                auto rulePaths = m_config->GetRulePaths();
                try {
                    m_engine->SetRules(rulePaths);
                    m_engine->Start();
                    
                    m_ruleListBox.CaptureSnapshots();
                }
                catch (const std::exception& e) {
                    m_errorMessage = std::format("Error starting engine:\n {}", e.what());

                }
            }
            ImGui::SetItemTooltip("Start engine");
        }

        m_verdictListBox.Show();
        ImGui::End(); // VERDICTS

        ImGui::Begin(VIEWER);

        if (m_verdictListBox.IsSelectionUpdated()) {
            m_ruleListBox.ResetSelected();

            auto selectedVerdict = m_verdictListBox.GetSelectedHandle();

            std::string formattedFields = UnorderedMapToString(selectedVerdict.payload.fields);
            std::string displayVerdict = std::format("result: {}\n", selectedVerdict.result? "matched" : "unmatched");
            if (selectedVerdict.result) {
                displayVerdict += std::format("rule matched id: {}\n", selectedVerdict.rule_matched);
            }
            displayVerdict += std::format("Event fields:\n{}", formattedFields);

            m_viewerBuffer.assign(std::move(displayVerdict));
        }
        else if (m_ruleListBox.IsSelectionUpdated()) {
            m_verdictListBox.ResetSelected();

            std::string fileContents = ReadFileToString(m_ruleListBox.GetSelectedHandle().string());
            m_viewerBuffer.assign(fileContents);
        }

        ImVec2 editor_size = ImGui::GetContentRegionAvail();
        ImGui::InputTextMultiline("##editor", &m_viewerBuffer, editor_size, ImGuiInputTextFlags_WordWrap | ImGuiInputTextFlags_ReadOnly);

        ImGui::End(); // VIEWER
    }
    catch (const std::exception& e)
    {
        m_errorMessage = e.what();
    }

    if(!m_errorMessage.empty()) {
        ImGui::OpenPopup("Error");
        //ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        //ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        if (ImGui::BeginPopupModal("Error", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Spacing();

            ImGuiStyle& style = ImGui::GetStyle();

            ImGui::PushFont(NULL, style.FontSizeBase * 2.15);
            ImGui::Text(ICON_FA_EXCLAMATION_TRIANGLE);
            ImGui::PopFont();

            ImGui::SameLine();
            ImGui::Text(("Something went wrong\n" + m_errorMessage).c_str());

            ImGui::Spacing();

            ImGui::Separator();

            if (ImGui::Button("OK", ImVec2(120, 0))) { m_errorMessage.clear(); }

            ImGui::EndPopup();
        }
    }
}

} // namespace ui