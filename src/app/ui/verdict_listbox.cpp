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

        constexpr int PAGE_SIZE = 50;
        size_t totalVerdicts = m_verdictBuffer.GetTotalVerdicts();
        int totalPages = (totalVerdicts + PAGE_SIZE - 1) / PAGE_SIZE;

        m_currentPage = std::clamp(m_currentPage, 0, totalPages > 0 ? totalPages - 1 : 0);

        auto verdictsPage = m_verdictBuffer.GetVerdictsPage(m_currentPage, PAGE_SIZE);
        int verdictStartIdx = m_currentPage * PAGE_SIZE;

        if (ImGui::BeginListBox("##Verdicts", ImVec2(-FLT_MIN, -ImGui::GetFrameHeightWithSpacing() - ImGui::GetTextLineHeight())))
        {
            for (int i = 0; i < (int)verdictsPage.size(); ++i)
            {
                const auto& verdict = verdictsPage[i];

                int eventRecordId = 0;
                auto it = verdict.payload.fields.find("EventRecordID");
                if (it != verdict.payload.fields.end()) {
                    eventRecordId = std::stoi(it->second);
                }

                bool isSelected = (m_selectedVerdictIdx == verdictStartIdx + i);

                std::string displayName = std::format("{} #{:<5}| {}",
                    verdict.result ? "●" : "○",
                    verdict.event_id,
                    eventRecordId
                );

                ImGui::PushID(i);

                ImGui::PushStyleColor(ImGuiCol_Text,
                    verdict.result ? ImGui::GetStyleColorVec4(ImGuiCol_Text) :
                    ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled));

                if (ImGui::Selectable(displayName.c_str(), isSelected))
                {
                    if (m_selectedVerdictIdx != verdictStartIdx + i) {
                        m_selectedVerdictIdx = verdictStartIdx + i;
                        SetIsUpdated(true);
                    }
                }

                ImGui::PopStyleColor();
                ImGui::PopID();
            }

            ImGui::EndListBox();
        }

        // Pagination controls
        ImGui::Spacing();
        if (ImGui::Button("<") && m_currentPage > 0) m_currentPage--;
        ImGui::SameLine();
        ImGui::Text("Page %d / %d", m_currentPage + 1, totalPages);
        ImGui::SameLine();
        if (ImGui::Button(">") && m_currentPage < totalPages - 1) m_currentPage++;

        ImGui::SameLine();
        ImGui::Text("%d-%d / %d", verdictStartIdx + 1, verdictStartIdx + (int)verdictsPage.size(), (int)totalVerdicts);

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

    std::vector<sigma::Verdict> VerdictListBox::GetVerdicts() const
    {
        return m_verdictBuffer.GetVerdicts();
    }
} // namespace ui