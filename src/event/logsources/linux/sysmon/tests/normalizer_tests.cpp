#include <gtest/gtest.h>
#include <windows/sysmon/normalizer.h>
#include <string>
#include <fstream>
#include <locale>
#include <codecvt>

using namespace sysmon;

class NormalizerTest : public ::testing::Test {
protected:
    Normalizer normalizer;

    void SetUp() override {
    }

    void TearDown() override {
    }
};

TEST_F(NormalizerTest, normalize_LocalizedString_ParsedToUtf8Correctly)
{

    std::wstring xmlWithCyrrillic = L"<Event><System><Localized>\u041A\u0438\u0440\u0438\u043B\u043B\u0438\u0446\u0430</Localized></System></Event>";
    Event event = normalizer.Normalize(xmlWithCyrrillic);
    EXPECT_EQ(event.logsource, LogSource::Sysmon);
    EXPECT_EQ(event.fields["Localized"], "Кириллица");
}

TEST_F(NormalizerTest, normalize_windashUtf16String_ParsedToUtf8Correctly)
{
    std::wstring xmlWithCyrrillic = L"<Event><System>"
        L"<Horizontal_Bar>\u2015</Horizontal_Bar>"
        L"<Em_Dash>\u2014</Em_Dash>"
        L"<En_Dash>\u2013</En_Dash>"
        L"</System></Event>";
    Event event = normalizer.Normalize(xmlWithCyrrillic);
    EXPECT_EQ(event.logsource, LogSource::Sysmon);
    EXPECT_EQ(event.fields.at("Horizontal_Bar"), "\xe2\x80\x95");
    EXPECT_EQ(event.fields.at("Em_Dash"), "\xe2\x80\x94");
    EXPECT_EQ(event.fields.at("En_Dash"), "\xe2\x80\x93");
}


TEST_F(NormalizerTest, normalize_FullSysmonEvent_AllFieldsAreParsed)
{
    std::ifstream input1("res/event_id_1.txt", std::ios::binary);
    std::string xmlDataUtf8((std::istreambuf_iterator<char>(input1)), {});
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::wstring xmlDataUtf16 = converter.from_bytes(xmlDataUtf8);
    Event event;
    ASSERT_NO_THROW(event = normalizer.Normalize(xmlDataUtf16));
    ASSERT_EQ(event.fields.size(), 38);
    EXPECT_EQ(event.logsource, LogSource::Sysmon);

    // System fields
    EXPECT_EQ(event.fields["Provider_Name"], "Microsoft-Windows-Sysmon");
    EXPECT_EQ(event.fields["Provider_Guid"], "{5770385F-C22A-43E0-BF4C-06F5698FFBD9}");
    EXPECT_EQ(event.fields["EventID"], "1");
    EXPECT_EQ(event.fields["Version"], "5");
    EXPECT_EQ(event.fields["Level"], "4");
    EXPECT_EQ(event.fields["Task"], "1");
    EXPECT_EQ(event.fields["Opcode"], "0");
    EXPECT_EQ(event.fields["Keywords"], "0x8000000000000000");
    EXPECT_EQ(event.fields["TimeCreated_SystemTime"], "2024-04-28T22:08:22.025812200Z");
    EXPECT_EQ(event.fields["EventRecordID"], "757");
    EXPECT_EQ(event.fields["Execution_ProcessID"], "3216");
    EXPECT_EQ(event.fields["Execution_ThreadID"], "3964");
    EXPECT_EQ(event.fields["Channel"], "Microsoft-Windows-Sysmon/Operational");
    EXPECT_EQ(event.fields["Computer"], "rfsH.lab.local");
    EXPECT_EQ(event.fields["Security_UserID"], "S-1-5-18");

    // EventData fields
    EXPECT_EQ(event.fields["RuleName"], "-");
    EXPECT_EQ(event.fields["UtcTime"], "2024-04-28 22:08:22.025");
    EXPECT_EQ(event.fields["ProcessGuid"], "{A23EAE89-BD56-5903-0000-0010E9D95E00}");
    EXPECT_EQ(event.fields["ProcessId"], "6228");
    EXPECT_EQ(event.fields["Image"], "C:\\Windows\\System32\\wbem\\WmiPrvSE.exe");
    EXPECT_EQ(event.fields["FileVersion"], "10.0.22621.1 (WinBuild.160101.0800)");
    EXPECT_EQ(event.fields["Description"], "WMI Provider Host");
    EXPECT_EQ(event.fields["Product"], "Microsoft® Windows® Operating System");
    EXPECT_EQ(event.fields["Company"], "Microsoft Corporation");
    EXPECT_EQ(event.fields["OriginalFileName"], "Wmiprvse.exe");
    EXPECT_EQ(event.fields["CommandLine"], "C:\\Windows\\system32\\wbem\\wmiprvse.exe -secured -Embedding");
    EXPECT_EQ(event.fields["CurrentDirectory"], "C:\\Windows\\system32\\");
    EXPECT_EQ(event.fields["User"], "LAB\\rsmith");
    EXPECT_EQ(event.fields["LogonGuid"], "{A23EAE89-B357-5903-0000-002005EB0700}");
    EXPECT_EQ(event.fields["LogonId"], "0x7eb05");
    EXPECT_EQ(event.fields["TerminalSessionId"], "1");
    EXPECT_EQ(event.fields["IntegrityLevel"], "System");
    EXPECT_EQ(event.fields["Hashes"], "SHA1=91180ED89976D16353404AC982A422A707F2AE37,MD5=7528CCABACCD5C1748E63E192097472A,SHA256=196CABED59111B6C4BBF78C84A56846D96CBBC4F06935A4FD4E6432EF0AE4083,IMPHASH=144C0DFA3875D7237B37631C52D608CB");
    EXPECT_EQ(event.fields["ParentProcessGuid"], "{A23EAE89-BD28-5903-0000-00102F345D00}");
    EXPECT_EQ(event.fields["ParentProcessId"], "580");
    EXPECT_EQ(event.fields["ParentImage"], "C:\\Windows\\System32\\svchost.exe");
    EXPECT_EQ(event.fields["ParentCommandLine"], "C:\\Windows\\system32\\svchost.exe -k DcomLaunch -p");
    EXPECT_EQ(event.fields["ParentUser"], "NT Authority\\SYSTEM");
}