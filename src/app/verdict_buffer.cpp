#include "verdict_buffer.h"

namespace bl {

    VerdictBuffer::VerdictBuffer(sigma::Engine& engine) {
        m_uiVerdicts.reserve(MAX_VERDICTS);

        engine.Subscribe([this](const sigma::Verdict& v) {
            m_buffer.enqueue(v);
            });

        // Consumer thread
        m_consumerThread = std::jthread([this](std::stop_token st) {
            constexpr size_t BULK_SIZE = 32;
            assert(BULK_SIZE < MAX_VERDICTS);

            sigma::Verdict bulk[BULK_SIZE];

            while (!st.stop_requested()) {
                size_t count = m_buffer.wait_dequeue_bulk_timed(bulk, BULK_SIZE, std::chrono::milliseconds(100));

                if (count > 0) {
                    std::lock_guard<std::mutex> lock(m_mutex);

                    if (m_uiVerdicts.size() + count > MAX_VERDICTS) {
                        size_t overflow = m_uiVerdicts.size() + count - MAX_VERDICTS;
                        m_uiVerdicts.erase(m_uiVerdicts.begin(),
                            m_uiVerdicts.begin() + overflow);
                    }
                    m_uiVerdicts.insert(m_uiVerdicts.end(), bulk, bulk + count);
                }
            }
            });
    }

    std::vector<sigma::Verdict> VerdictBuffer::GetVerdicts() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_uiVerdicts;
    }

    std::vector<sigma::Verdict> VerdictBuffer::GetVerdictsPage(size_t page, size_t pageSize) const
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        size_t total = m_uiVerdicts.size();
        size_t start = page * pageSize;
        if (start >= total) return {};

        size_t end = std::min(start + pageSize, total);
        return std::vector<sigma::Verdict>(m_uiVerdicts.begin() + start,
            m_uiVerdicts.begin() + end);
    }

    size_t VerdictBuffer::GetTotalVerdicts() const
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_uiVerdicts.size();
    }
} // namespace bl