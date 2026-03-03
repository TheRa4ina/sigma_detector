#pragma once
#define NOMINMAX
#include <windows.h>
#include <winevt.h>
#include <string>
#include <queue>
#include <mutex>
#include "sysmon_normalizer.h"
#include <event/event.h>
#include <blockingconcurrentqueue.h>

#pragma comment(lib, "wevtapi.lib")

namespace channel {

	struct EvtHandleDeleter {
		void operator()(EVT_HANDLE h) const noexcept;
	};
	using EvtHandle = std::unique_ptr<std::remove_pointer_t<EVT_HANDLE>, EvtHandleDeleter>;

	class SysmonReceiver
	{
	public:
		SysmonReceiver() = default;
		~SysmonReceiver();

		void Start(std::shared_ptr<moodycamel::BlockingConcurrentQueue<Event>> queue);
		void Stop();
	private:
		DWORD WINAPI OnRecieveEvent(EVT_SUBSCRIBE_NOTIFY_ACTION action, EVT_HANDLE event);
		channel::SysmonNormalizer m_normalizer;
		EvtHandle m_eventSubscription;
		std::shared_ptr<moodycamel::BlockingConcurrentQueue<Event>> m_queue;
	};
} // channel