#include "sysmon_receiver.h"
#include <iostream>

#define TRACE_PREFIX "Sysmon receiver\t"
#include <quill/logging.h>

static constexpr const wchar_t* CHANNEL = L"Microsoft-Windows-Sysmon/Operational";
static constexpr DWORD FLAGS = EvtSubscribeToFutureEvents;


namespace channel {
    void EvtHandleDeleter::operator()(EVT_HANDLE h) const noexcept {
        if (h) EvtClose(h);
    }

    SysmonReceiver::~SysmonReceiver()
    {
        Stop();
    }

    void SysmonReceiver::Start(std::shared_ptr<moodycamel::BlockingConcurrentQueue<Event>> queue)
    {
        m_queue = queue;
        m_eventSubscription.reset(EvtSubscribe(
            NULL,
            NULL,
            CHANNEL,
            NULL,
            NULL,
            this,
            [](EVT_SUBSCRIBE_NOTIFY_ACTION action, PVOID context, EVT_HANDLE event) -> DWORD {
                return static_cast<SysmonReceiver*>(context)->OnRecieveEvent(action, event);
            },
            FLAGS
        ));

        if (!m_eventSubscription) {
            LOG_ERROR("Failed to subscribe:{} \t", GetLastError());
        }
        else {
            LOG_INFO("Receiver started on Sysmon channel\n");
        }
    }

void SysmonReceiver::Stop()
{
    if (m_eventSubscription) {
        m_eventSubscription.reset();
        LOG_INFO("Receiver stopped.\n");
    }
    m_queue.reset();
}

DWORD WINAPI SysmonReceiver::OnRecieveEvent(EVT_SUBSCRIBE_NOTIFY_ACTION action, EVT_HANDLE event)
{
    if (action != EvtSubscribeActionDeliver) return 0;

    DWORD bufferUsed = 0, propertyCount = 0;

    // First call to get the required buffer size
    if (!EvtRender(NULL, event, EvtRenderEventXml, 0, NULL, &bufferUsed, &propertyCount) &&
        GetLastError() == ERROR_INSUFFICIENT_BUFFER)
    {
        std::wstring buffer;
        buffer.resize(bufferUsed / sizeof(wchar_t));
        if (EvtRender(NULL, event, EvtRenderEventXml, bufferUsed, buffer.data(), &bufferUsed, &propertyCount)) {
            m_queue->enqueue(m_normalizer.Normalize(buffer.data()));
        }
    }

    return 0;
}

} // channel