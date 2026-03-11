#pragma once
#include <sigma/engine.h>
#include <vector>
#include <thread>
#include <atomic>
#include <mutex>
#include <blockingconcurrentqueue.h>

namespace bl {

    class VerdictBuffer {
    public:
        explicit VerdictBuffer(sigma::Engine& engine);
        ~VerdictBuffer() = default;

        std::vector<sigma::Verdict> GetVerdicts() const;
        std::vector<sigma::Verdict> GetVerdictsPage(size_t page, size_t pageSize) const;
        size_t GetTotalVerdicts() const;

    private:
        std::vector<sigma::Verdict> m_uiVerdicts;
        static constexpr size_t MAX_VERDICTS = 100000;

        moodycamel::BlockingConcurrentQueue<sigma::Verdict> m_buffer;
        std::jthread m_consumerThread;
        mutable std::mutex m_mutex; // protects m_uiVerdicts
    };

} // namespace bl