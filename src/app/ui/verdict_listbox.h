#pragma once
#include "ilistbox.h"
#include <app/verdict_buffer.h>
#include <sigma/engine.h>
#include <vector>

namespace ui {

    class VerdictListBox : public IListBox<sigma::Verdict>
    {
    public:
        explicit VerdictListBox(std::shared_ptr<sigma::Engine> engine);

        void Show() override;
        sigma::Verdict GetSelectedHandle() const override;

        std::vector<sigma::Verdict> GetVerdicts() const;

    protected:
        void ClearCurrentSelection() override;

    private:
        bl::VerdictBuffer m_verdictBuffer;
        int m_selectedVerdictIdx = -1;
        int m_currentPage = 0;
    };

} // namespace ui