#pragma once
#include <string>
#include <event/event.h>

namespace channel {

    using RawEvent = std::string;   // journald gives UTF-8

    class SysmonNormalizer
    {
    public:
        Event Normalize(const RawEvent& rawData);
    };

} // channel