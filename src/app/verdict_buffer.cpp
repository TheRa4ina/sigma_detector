#include "verdict_buffer.h"

namespace bl
{

    VerdictBuffer::VerdictBuffer(sigma::Engine& engine) {
        engine.Subscribe([this](const sigma::Verdict& v) {
            m_buffer.enqueue(v);
            });
    }

    void VerdictBuffer::Update() {
        sigma::Verdict v;
        while (m_buffer.try_dequeue(v)) {
            m_uiVerdicts.push_back(v);

            if (m_uiVerdicts.size() > MAX_VERDICTS) {
                m_uiVerdicts.erase(m_uiVerdicts.begin());
            }
        }
    }

} // namespace bl