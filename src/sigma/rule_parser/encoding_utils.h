#pragma once
#include <string>
#include <vector>
namespace utils{
    std::string EncodeBase64(const std::string& data);
    std::string EncodeBase64OffsetStable(const std::string& input, int offset);
    std::u16string Utf8ToUtf16(const std::string& s);
    std::string ToUtf16leBytes(const std::u16string& u16, bool bom = false);
    std::string ToUtf16beBytes(const std::u16string& u16);
}
