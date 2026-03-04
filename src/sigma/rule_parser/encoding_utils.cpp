#pragma once

#include <rule_parser/encoding_utils.h>
#include <base64.hpp>
#include <vector>
#include <stdexcept>

#ifdef _WIN32
    #include <windows.h>
    #include <codecvt>
    #include <locale>
#else
    #include <iconv.h>
    #include <cstring>
#endif

namespace utils {

    namespace {
        bool ContainsWildcard(const std::string& str) {
            return str.find('*') != str.npos
                || str.find('?') != str.npos;
        }
    } // namespace
    std::string EncodeBase64(const std::string& data){
        if (ContainsWildcard(data)) {
            throw std::runtime_error(" Base64 encoding of strings with wildcards is not allowed");
        }
        return base64::to_base64(data);
    }

    std::string EncodeBase64OffsetStable(const std::string& input, int offset){
        if (offset < 0 || offset > 2)
            throw std::invalid_argument("offset must be 0, 1, or 2");

        std::string prefix(offset, '\0');
        std::string data = prefix + input;

        // use TobiasLocker Base64 encoder
        std::string encoded = EncodeBase64(input);

        int startTrim = 0;
        int endTrim = 0;

        switch (offset)
        {
        case 0:
            // normal alignment → drop last incomplete group (3 chars)
            endTrim = 3;
            break;

        case 1:
            // 1-byte misalignment → first 2 + last 1 are unstable
            startTrim = 2;
            endTrim = 1;
            break;

        case 2:
            // 2-byte misalignment → first 3 are from prefix
            startTrim = 3;
            endTrim = 0;
            break;
        }

        if (encoded.size() > (size_t)(startTrim + endTrim))
            encoded = encoded.substr(startTrim, encoded.size() - startTrim - endTrim);
        else
            encoded.clear();

        return encoded;
    }

#ifdef _WIN32
    std::u16string Utf8ToUtf16(const std::string& s) {
        std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> conv;
        return conv.from_bytes(s);
    }
#elif __linux__ // _WIN32
        std::u16string Utf8ToUtf16(const std::string& s)
    {
        if (s.empty()) return {};

        iconv_t cd = iconv_open("UTF-16LE", "UTF-8");
        if (cd == (iconv_t)-1)
            throw std::runtime_error("iconv_open failed");

        size_t inBytes = s.size();
        size_t outBytes = inBytes * 2 + 2;

        std::vector<char> buffer(outBytes);
        char* inBuf = const_cast<char*>(s.data());
        char* outBuf = buffer.data();

        if (iconv(cd, &inBuf, &inBytes, &outBuf, &outBytes) == (size_t)-1) {
            iconv_close(cd);
            throw std::runtime_error("iconv conversion failed");
        }

        size_t convertedSize = buffer.size() - outBytes;
        iconv_close(cd);

        return std::u16string(
            reinterpret_cast<char16_t*>(buffer.data()),
            convertedSize / 2
        );
    }
#endif // __linux__

    std::string ToUtf16leBytes(const std::u16string& u16, bool bom) {
        std::vector<uint8_t> out;
        if (bom) { out.push_back(0xFF); out.push_back(0xFE); }
        out.reserve((bom ? 2 : 0) + u16.size() * 2);
        for (char16_t ch : u16) {
            out.push_back(static_cast<uint8_t>(ch & 0xFF));        // LSB
            out.push_back(static_cast<uint8_t>((ch >> 8) & 0xFF)); // MSB
        }
        return std::string(reinterpret_cast<const char*>(out.data()), out.size());
    }

    std::string ToUtf16beBytes(const std::u16string& u16) {
        std::vector<uint8_t> out; out.reserve(u16.size() * 2);
        for (char16_t ch : u16) {
            out.push_back(static_cast<uint8_t>((ch >> 8) & 0xFF)); // MSB
            out.push_back(static_cast<uint8_t>(ch & 0xFF));        // LSB
        }
        return std::string(reinterpret_cast<const char*>(out.data()), out.size());
    }

}