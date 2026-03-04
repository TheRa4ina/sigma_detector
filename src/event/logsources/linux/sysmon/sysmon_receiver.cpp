#include "sysmon_receiver.h"
#include <iostream>
#include <cstring>
#define TRACE_PREFIX "Sysmon receiver:\t"
#include <quill/logging.h>

namespace channel {

SysmonReceiver::~SysmonReceiver()
{
    Stop();
}

void SysmonReceiver::Start(std::shared_ptr<moodycamel::BlockingConcurrentQueue<Event>> q)
{
    m_queue = q;

    if (sd_journal_open(&m_journal, SD_JOURNAL_LOCAL_ONLY) < 0) {
        LOG_ERROR("Failed to open journal");
        return;
    }

    if (sd_journal_add_match(m_journal, "SYSLOG_IDENTIFIER=sysmon", 0) < 0) {
        LOG_ERROR("Failed to add match");
        return;
    }

    if (sd_journal_seek_tail(m_journal) < 0) {
        LOG_ERROR("Failed to seek tail");
        return;
    }

    // Move to last entry (tail points *after* last entry)
    if (sd_journal_previous(m_journal) < 0) {
        LOG_ERROR("Failed to move to last entry");
        return;
    }

    char *start_cursor = nullptr;
    if (sd_journal_get_cursor(m_journal, &start_cursor) < 0) {
        LOG_ERROR("Failed to get start cursor");
        return;
    }

    m_running = true;
    m_thread = std::thread([this, start_cursor]() {
        while (m_running) {
            // Wait for new events
            if (sd_journal_wait(m_journal, (uint64_t)-1) < 0) break;

            int r;
            while ((r = sd_journal_next(m_journal)) > 0) {
                char *cur = nullptr;
                if (sd_journal_get_cursor(m_journal, &cur) < 0) {
                    continue;
                }

                // Skip any events before or at the start cursor
                if (strcmp(cur, start_cursor) <= 0) {
                    free(cur);
                    continue;
                }

                // Process new event
                ProcessEvent();
                free(cur);
            }
        }
        free(start_cursor);
    });
}

void SysmonReceiver::Stop()
{
    m_running = false;

    if (m_thread.joinable())
        m_thread.join();

    if (m_journal) {
        sd_journal_close(m_journal);
        m_journal = nullptr;
    }

    m_queue.reset();
}

void SysmonReceiver::ProcessEvent()
{
    const void* data;
    size_t length;

    if (sd_journal_get_data(m_journal, "MESSAGE", &data, &length) == 0) {
        std::string msg((const char*)data, length);

        auto pos = msg.find('=');
        if (pos != std::string::npos)
            msg = msg.substr(pos + 1);

        m_queue->enqueue(m_normalizer.Normalize(msg));
    }
}

} // namespace channel