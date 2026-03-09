#include "verdict_listbox.h"
#include <imgui.h>
#include <cfloat>
#include <stdexcept>
#include <format>

namespace ui {

    VerdictListBox::VerdictListBox(std::shared_ptr<sigma::Engine> engine)
        : m_verdictBuffer(bl::VerdictBuffer(*engine)) {}

    void VerdictListBox::Show()
    {
        ImGui::PushID(this);

        if (ImGui::BeginListBox("##Verdicts", ImVec2(-FLT_MIN, -ImGui::GetFrameHeight())))
        {
            const auto& verdicts = m_verdictBuffer.GetVerdicts();

            for (int i = 0; i < (int)verdicts.size(); ++i)
            {
                const auto& verdict = verdicts[i];

                int eventRecordId = 0;
                auto it = verdict.payload.fields.find("EventRecordID");
                if (it != verdict.payload.fields.end()) {
                    eventRecordId = std::stoi(it->second);
                }

                bool isSelected = (m_selectedVerdictIdx == i);

                std::string displayName = std::format("{} #{:<5}| {}",
                    verdict.result ? "●" : "○",
                    verdict.event_id,
                    eventRecordId
                );
                ImGui::PushID(i);

                if (verdict.result) {
                    ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_Text));
                }
                else {
                    ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled));
                }

                if (ImGui::Selectable(displayName.c_str(), isSelected))
                {
                    if (m_selectedVerdictIdx != i) {
                        m_selectedVerdictIdx = i;
                        SetIsUpdated(true);
                    }
                }
                ImGui::PopStyleColor();

                ImGui::PopID();
            }

            ImGui::EndListBox();
        }

        ImGui::PopID();
    }

    sigma::Verdict VerdictListBox::GetSelectedHandle() const
    {
        const auto& verdicts = m_verdictBuffer.GetVerdicts();

        if (m_selectedVerdictIdx < 0 ||
            m_selectedVerdictIdx >= (int)verdicts.size())
        {
            throw std::runtime_error("No verdict selected");
        }

        return verdicts[m_selectedVerdictIdx];
    }

    void VerdictListBox::ClearCurrentSelection()
    {
        m_selectedVerdictIdx = -1;
    }

} // namespace ui