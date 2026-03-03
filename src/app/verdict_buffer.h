#pragma once

#include <concurrentqueue.h>
#include <sigma/engine.h>
#include <vector>

namespace bl
{

class VerdictBuffer {
    moodycamel::ConcurrentQueue<sigma::Verdict> m_buffer;
    std::vector<sigma::Verdict> m_uiVerdicts;
    static constexpr size_t MAX_VERDICTS = 1000;

public:
    explicit VerdictBuffer(sigma::Engine& engine);

    void Update();

    const std::vector<sigma::Verdict>& GetVerdicts() const { return m_uiVerdicts; }

    size_t GetUnreadCount() const { return m_buffer.size_approx(); }
};

} // namespace bl