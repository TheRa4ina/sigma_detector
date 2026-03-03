#include <windows.h>
#include <winevt.h>
#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <unordered_map>
#include "event.h"

#include <pugixml.hpp>

#pragma comment(lib, "wevtapi.lib")

// RAII wrapper for EVT_HANDLE
struct EvtHandleDeleter {
    void operator()(EVT_HANDLE h) const noexcept {
        if (h) EvtClose(h);
    }
};
using UniqueEvtHandle = std::unique_ptr<std::remove_pointer_t<EVT_HANDLE>, EvtHandleDeleter>;

static std::string WideToUtf8(const std::wstring& w)
{
    if (w.empty()) return {};
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, w.data(), (int)w.size(), nullptr, 0, nullptr, nullptr);
    if (size_needed <= 0) return {};
    std::string out(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, w.data(), (int)w.size(), &out[0], size_needed, nullptr, nullptr);
    return out;
}

void ParseEventXml(const wchar_t* xmlData)
{
    if (!xmlData) {
        std::cerr << "xmlData is null\n";
        return;
    }

    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_buffer(xmlData, std::wcslen(xmlData) * sizeof(wchar_t));

    if (!result) {
        // result.description() returns a narrow const char* even in wchar mode
        std::cerr << "XML parse error: " << result.description() << '\n';
        return;
    }

    // print full data (convert once for output)
    std::wstring wfull(xmlData);
    std::cout << "FULL_DATA\n" << WideToUtf8(wfull) << "\n\n";

    pugi::xml_node systemNode = doc.child(L"Event").child(L"System");
    if (systemNode) {
        std::cout << "\nNEW EVENT\n---------\n\n";
        for (const auto& child : systemNode.children()) {
            // child.name(), child.child_value(), attr.value() return wchar_t* in wchar mode
            std::wstring wname = child.name() ? child.name() : L"";
            std::wstring wvalue = child.child_value() ? child.child_value() : L"";
            std::cout << WideToUtf8(wname) << ": " << WideToUtf8(wvalue) << '\n';

            for (const auto& attr : child.attributes()) {
                std::wstring aname = attr.name() ? attr.name() : L"";
                std::wstring aval = attr.value() ? attr.value() : L"";
                std::cout << WideToUtf8(aname) << ": " << WideToUtf8(aval) << '\n';
            }
        }
    }

    pugi::xml_node EventDataNode = doc.child(L"Event").child(L"EventData");
    if (EventDataNode) {
        std::cout << "-Data-\n";
        for (const auto& child : EventDataNode.children(L"Data")) {
            std::wstring wname = child.attribute(L"Name").as_string();
            std::wstring wval = child.child_value() ? child.child_value() : L"";
            std::cout << WideToUtf8(wname) << ": " << WideToUtf8(wval) << '\n';
        }
    }
}

// Helper to convert GetLastError to string
std::string GetLastErrorAsString() {
    DWORD error = GetLastError();
    if (!error) return {};

    LPSTR messageBuffer = nullptr;
    size_t size = FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, error,
        MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
        (LPSTR)&messageBuffer, 0, NULL
    );

    std::string message(messageBuffer, size);
    LocalFree(messageBuffer);
    return message;
}

// Callback must return DWORD
DWORD WINAPI EventCallback(EVT_SUBSCRIBE_NOTIFY_ACTION action, PVOID, EVT_HANDLE event)
{
    if (action == EvtSubscribeActionDeliver) {
        DWORD bufferUsed = 0, propertyCount = 0;
        if (!EvtRender(NULL, event, EvtRenderEventXml, 0, NULL, &bufferUsed, &propertyCount) &&
            GetLastError() == ERROR_INSUFFICIENT_BUFFER)
        {
            std::vector<wchar_t> buffer(bufferUsed / sizeof(wchar_t));
            if (EvtRender(NULL, event, EvtRenderEventXml, bufferUsed, buffer.data(), &bufferUsed, &propertyCount)) {
                wprintf(L"Sysmon Event:\n%s\n\n", buffer.data());
            }
        }
    }
    return 0;
}

DWORD WINAPI SysmonParseEventCallback(EVT_SUBSCRIBE_NOTIFY_ACTION action, PVOID, EVT_HANDLE event) {
    if (action != EvtSubscribeActionDeliver) return 0;

    DWORD bufferUsed = 0, propertyCount = 0;

    // First call to get the required buffer size
    if (!EvtRender(NULL, event, EvtRenderEventXml, 0, NULL, &bufferUsed, &propertyCount) &&
        GetLastError() == ERROR_INSUFFICIENT_BUFFER)
    {
        std::vector<wchar_t> buffer(bufferUsed / sizeof(wchar_t));

        if (EvtRender(NULL, event, EvtRenderEventXml, bufferUsed, buffer.data(), &bufferUsed, &propertyCount)) {
            ParseEventXml(buffer.data());  // direct wide-char parsing
        }
    }

    return 0;
}

int main() {
    SetConsoleOutputCP(CP_UTF8);

    const wchar_t* channelPath = L"Microsoft-Windows-Sysmon/Operational";

    UniqueEvtHandle subscription{
        EvtSubscribe(
            NULL,
            NULL,
            channelPath,
            NULL,
            NULL,
            NULL,
            SysmonParseEventCallback, // no cast needed
            EvtSubscribeToFutureEvents
        )
    };

    if (!subscription) {
        std::cerr << "Failed to subscribe: " << GetLastErrorAsString() << std::endl;
        return 1;
    }

    std::wcout << L"Listening for Sysmon events... Press Ctrl+C to exit.\n";
    WaitForSingleObject(GetCurrentThread(), INFINITE); // blocks instead of busy-loop
    return 0;
}
