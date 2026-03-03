#include "rule_listbox.h"
#include <format>
#include <imgui.h>

namespace ui {

    RuleListBox::RuleListBox(std::shared_ptr<bl::EngineConfig> engineConfig)
        : m_engineConfig(std::move(engineConfig))
    {
    }

    void RuleListBox::Show()
    {
        ImGui::PushID(this);

        if (ImGui::BeginListBox("##Rules", ImVec2(-FLT_MIN, -ImGui::GetFrameHeight())))
        {
            const auto rulePaths = m_engineConfig->GetRulePaths();
            for (const auto& rulePath : rulePaths)
            {
                ImGui::PushID(rulePath.string().c_str());

                // Perhaps the widget itself should track the engine, rather than maind window having to remind it
                bool changedSinceSnapshot = false;
                auto it = m_snapshots.find(rulePath.string());
                if (it != m_snapshots.end()) {
                    auto lastSnapshotChangeTime = it->second;
                    auto currentChangeTime = std::filesystem::last_write_time(rulePath);
                    changedSinceSnapshot = currentChangeTime > lastSnapshotChangeTime;
                }

                bool isSelected = (m_selectedPath == rulePath);

                std::u8string utf8Path = rulePath.filename().u8string();
                auto displayName = std::string(reinterpret_cast<char const*>(utf8Path.data()), utf8Path.size());
                if (changedSinceSnapshot)
                {
                    displayName += '*';
                    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 200, 0, 255));
                }
                if (ImGui::Selectable(displayName.c_str(), isSelected))
                {
                    if (m_selectedPath != rulePath) {
                        m_selectedPath = rulePath;
                        SetIsUpdated(true);
                    }
                }
                if (changedSinceSnapshot)
                {
                    if (m_selectedPath == rulePath) SetIsUpdated(true);
                    ImGui::PopStyleColor();
                    ImGui::SetItemTooltip("Rule modified since engine start (needs restart to apply)");
                }

                ImGui::PopID();
            }

            ImGui::EndListBox();
        }

        ImGui::PopID();
    }

    void RuleListBox::CaptureSnapshots()
    {
        m_snapshots.clear();
        const auto paths = m_engineConfig->GetRulePaths();
        for (const auto& p : paths) {
            try {
                m_snapshots[p.string()] = std::filesystem::last_write_time(p);
            }
            catch (const std::exception& e) {
                std::u8string utf8Path = p.u8string();
                auto path = std::string(reinterpret_cast<char const*>(utf8Path.data()), utf8Path.size());

                throw std::runtime_error(std::format("Error accessing last write time of: {}", path));
            }
        }
    }

    void RuleListBox::ClearSnapshots()
    {
        m_snapshots.clear();
    }

    std::filesystem::path RuleListBox::GetSelectedHandle() const
    {
        return m_selectedPath;
    }

    void RuleListBox::ClearCurrentSelection()
    {
        m_selectedPath.clear();
    }

} // namespace ui