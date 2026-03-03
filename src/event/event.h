#pragma once
#include <string>
#include <variant>
#include <unordered_map>
#include "logsources/channel.h"

struct Event {
    std::unordered_map < std::string, std::string > fields;
    Channel channel;
};