#pragma once

#include <systemd/sd-journal.h>
#include <memory>
#include <atomic>
#include <thread>
#include <blockingconcurrentqueue.h>
#include "sysmon_normalizer.h"

namespace channel {

class SysmonReceiver
{
public:
    SysmonReceiver() = default;
    ~SysmonReceiver();

    void Start(std::shared_ptr<moodycamel::BlockingConcurrentQueue<Event>> queue);
    void Stop();

private:
    void ProcessEvent();

    sd_journal* m_journal{nullptr};
    std::thread m_thread;
    std::atomic<bool> m_running{false};

    channel::SysmonNormalizer m_normalizer;
    std::shared_ptr<moodycamel::BlockingConcurrentQueue<Event>> m_queue;
};

}