#pragma once
#include <string>
#include <event/event.h>

namespace channel {
	using RawEvent = std::wstring;

	class SysmonNormalizer
	{
	public:
		Event Normalize(RawEvent rawData);
	};
} // channel