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
    private:
        std::vector<sigma::Verdict> m_uiVerdicts;
        static constexpr size_t MAX_VERDICTS = 1000;

        moodycamel::BlockingConcurrentQueue<sigma::Verdict> m_buffer;
        std::jthread m_consumerThread;
        mutable std::mutex m_mutex; // protects m_uiVerdicts
    };

} // namespace bl