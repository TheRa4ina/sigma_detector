#pragma once
#include <boost/asio.hpp>
#include <mutex>
#include <functional>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <unordered_set>
#include <vector>
#include <atomic>
#include <string>
#include <blockingconcurrentqueue.h>
#include <sigma/rule_parser/rule_parser.h>
#include <sigma/rule_parser/rule.h>
#include <event/logsources/windows/sysmon/sysmon_receiver.h>
#include <c4/substr.hpp>
#include <c4/std/string.hpp>
#include <c4/charconv.hpp>
#include <chrono>
#include <quill/Logger.h>

namespace sigma {

struct Verdict {
    int event_id;
    Event payload;
    std::string rule_matched;
    bool result = false;
};

class Engine {
public:
    using VerdictCallback = std::function<void(const Verdict&)>;

    Engine();
    void Subscribe(VerdictCallback cb);
    void SetRules(const std::vector<std::string>& rules);
    void SetRules(const std::unordered_set<std::filesystem::path>& rulePathss);
    bool IsRunning();

    void Start();
    void Stop();

private:
    Verdict MatchRules(const Event& event);
    void SetRules(std::vector<sigma::Rule> rulesContent);
    void DispatchLoop();
    void ConsumeLoop();
    void NotifySubscribers(const Verdict& v);

    std::shared_ptr<moodycamel::BlockingConcurrentQueue<Event>> m_eventQueue;
    std::shared_ptr<moodycamel::BlockingConcurrentQueue<Verdict>> m_verdictQueue;

    channel::SysmonReceiver m_receiver;
    boost::asio::thread_pool m_pool;
    std::vector<sigma::Rule> m_rules;

    std::vector<VerdictCallback> m_subscribers;
    std::mutex m_subMutex;

    std::thread m_dispatcher;
    std::thread m_consumer;
    std::atomic<bool> m_running;
    sigma::RuleParser m_parser;
};

} //sigma