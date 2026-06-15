#include "engine.h"
#include <format>
#define TRACE_PREFIX "Engine:\t"
#include <quill/logging.h>
#include <quill/std/Vector.h>
#include <quill/std/FilesystemPath.h>
#include <quill/std/UnorderedSet.h>


namespace sigma {

namespace {
    // TODO refactor into common/utility module
    std::string ReadFileToString(const std::filesystem::path& path) {
        std::ifstream file(path);
        if (!file.is_open()) {
            std::u8string utf8Path = path.u8string();
            auto displayPath = std::string(reinterpret_cast<char const*>(utf8Path.data()), utf8Path.size());
            throw std::runtime_error(std::format("Cannot open file: {}", displayPath));
        }

        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }
    template<typename K, typename V>
    std::string UnorderedMapToString(const std::unordered_map<K, V>& map) {
        std::stringstream ss;
        ss << "{\n";
        bool first = true;
        for (const auto& pair : map) {
            ss << pair.first << ": " << pair.second << (!first ? ',' : ' ') << '\n';
            first = false;
        }
        ss << "}";
        return ss.str();
    }
}

Engine::Engine()
    : m_eventQueue(std::make_shared<moodycamel::BlockingConcurrentQueue<Event>>()),
    m_verdictQueue(std::make_shared<moodycamel::BlockingConcurrentQueue<Verdict>>()),
    m_pool(4),
    m_running(false)
{
}

void Engine::Subscribe(VerdictCallback cb) {
    std::lock_guard<std::mutex> lock(m_subMutex);
    m_subscribers.push_back(cb);
}

void Engine::SetRules(std::vector<sigma::Rule> rules) {
    assert(!IsRunning());
    m_rules = std::move(rules);
}

void Engine::SetRules(const std::vector<std::string>& rules) {
    LOG_INFO("Set rules called with: {}", rules);
    if (IsRunning()) {
        throw std::runtime_error("Cannot setRules while engine running");
    }
    std::vector<sigma::Rule> newRules;
    m_rules.reserve(rules.size());

    for (const auto rule : rules) {
        newRules.emplace_back(m_parser.Parse(rule));
    }
    SetRules(std::move(newRules));
}

void Engine::SetRules(const std::unordered_set<std::filesystem::path>& rulePaths) {
    LOG_INFO("Set rules called with: {}", rulePaths);
    if (IsRunning()) {
        throw std::runtime_error("Cannot setRules while engine running");
    }

    std::vector<sigma::Rule> newRules;
    m_rules.reserve(rulePaths.size());
    for (const auto rulePath : rulePaths) {
        try {
            std::string ruleContents = ReadFileToString(rulePath);
            sigma::Rule rule = m_parser.Parse(ruleContents);
            newRules.emplace_back(std::move(rule));
        }
        catch (const std::exception& e) {
            // TODO perhaps i should create a wrapper for converting u8 to char const
            std::u8string utf8Path = rulePath.u8string();
            auto displayPath = std::string(reinterpret_cast<char const*>(utf8Path.data()), utf8Path.size());
            throw std::runtime_error(std::format("Error while set rule at: {}\n {}", displayPath, e.what()));
        }
    }
    m_rules.swap(newRules);
}

inline bool Engine::IsRunning()
{
    return m_running.load(std::memory_order_relaxed);
}

void Engine::Start() {
    LOG_INFO("Starting engine");
    if (m_running.exchange(true, std::memory_order_relaxed)) return;

    m_receiver.Start(m_eventQueue);

    m_dispatcher = std::thread([this]() { DispatchLoop(); });
    m_consumer = std::thread([this]() { ConsumeLoop(); });
    LOG_INFO("Engine finished starting");
}

void Engine::Stop() {
    LOG_INFO("Stopping engine");
    if (!m_running.exchange(false)) return;
    m_receiver.Stop();
    m_pool.join();
    m_dispatcher.join();
    m_consumer.join();

    LOG_INFO("Engine stopped");
}

Verdict Engine::MatchRules(const Event& event) {
    Verdict v;
    v.payload = event;
    for (const auto& rule : m_rules) {
        v.result = rule.Match(event);
        if (v.result)
        {
            std::string ruleId = rule.GetMeta().id;
            LOG_INFO("Event id [{}], matched rule: [{}], Event contents: [\n{}]", v.event_id, ruleId, UnorderedMapToString(event.fields));
            v.rule_matched = ruleId;
            break;
        }
    }

    c4::atoi(c4::to_csubstr(event.fields.at("EventID")), &v.event_id);
    return v;
}

void Engine::DispatchLoop() {
    Event e;
    try {
        LOG_INFO("Dispatch loop: started");
        while (m_running) {
            bool got = m_eventQueue->wait_dequeue_timed(e, std::chrono::milliseconds(100));
            if (got)
            {
                LOG_DEBUG("Got event from journal: {}", static_cast<int>(e.channel)); // TODO add formatter for channel
                boost::asio::post(m_pool, [this, e]() {
                    try {
                        Verdict v = MatchRules(e);
                        m_verdictQueue->enqueue(v);
                    }
                    catch (const std::exception& ex) {
                        LOG_ERROR("Match rules exception: {} \n",  ex.what());
                    }
                    });
            }
        }
        LOG_INFO("Dispatch loop: ended");
    }
    catch (const std::exception& ex) {
        LOG_ERROR("Engine Dispatch thread exception: {}\n", ex.what());
    }
}

void Engine::ConsumeLoop() {
    Verdict v;
    LOG_INFO("Consume loop: started");
    while (m_running) {
        bool got = m_verdictQueue->wait_dequeue_timed(v, std::chrono::milliseconds(100));
        if (got) {
            NotifySubscribers(v);
        }
    }
    LOG_INFO("Consume loop: ended");
}

void Engine::NotifySubscribers(const Verdict& v) {
    std::lock_guard<std::mutex> lock(m_subMutex);
    for (auto& cb : m_subscribers)
        cb(v);
}

} // sigma