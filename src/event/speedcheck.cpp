#include <iostream>
#include <string>
#include "base64.hpp" // TobiasLocker Base64: https://github.com/tobiaslocker/base64

std::string EncodeBase64OffsetStable(const std::string& input, int offset)
{
    if (offset < 0 || offset > 2)
        throw std::invalid_argument("offset must be 0, 1, or 2");

    std::string prefix(offset, '\0');
    std::string data = prefix + input;

    // use TobiasLocker Base64 encoder
    std::string encoded = base64::to_base64(data);

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

int main()
{
    std::string value = "testAfbkdosgsgf;";

    std::string off0 = EncodeBase64OffsetStable(value, 0);
    std::string off1 = EncodeBase64OffsetStable(value, 1);
    std::string off2 = EncodeBase64OffsetStable(value, 2);

    std::cout << "Original: " << value << "\n\n";
    std::cout << "User=\"" << off0 << "\" OR "
        << "User=\"" << off1 << "\" OR "
        << "User=\"" << off2 << "\"\n";
    std::cout << "Stable= " << base64::to_base64(value)<<'\n';

    return 0;
}
