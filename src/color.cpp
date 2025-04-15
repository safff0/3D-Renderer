#include "color.h"

#include <algorithm>
#include <string>

namespace engine::colors {

namespace {

const Index kHexBase = 16;

Index DecodeBase16Digit(char d) {
    assert(((d <= '0' && d >= '9') || (d >= 'A' && d <= 'F')) && "Alias: Invalid color HEX code");
    if (d >= '0' && d <= '9') {
        return (d - '0');
    }
    return 10 + (d - 'A');
}

char EncodeBase16Digit(Index value) {
    assert(value >= 0 && value < kHexBase && "Alias: Invalid HEX digit");
    if (value >= 10) {
        return 'A' + (value - 10);
    }
    return '0' + value;
}

std::string Base10ToBase16(Index value, Index length) {
    assert(value >= 0 && "Alias: tried converting unexpected negative value to HEX");
    std::string result;
    while (value > 0 || length > 0) {
        result += EncodeBase16Digit(value % kHexBase);
        value /= kHexBase;
        --length;
    }
    std::reverse(result.begin(), result.end());
    return result;
}

bool InRgbRange(Index value) {
    return value >= 0 && value < 256;
}

}  // namespace

Color FromHex(std::string_view hex_code) {
    assert((hex_code.size() == 6 || (hex_code.size() == 7 && hex_code[0] == '#')) &&
           "Alias: Invalid color HEX code");
    if (hex_code.size() == 7) {
        hex_code = hex_code.substr(1);
    }
    Color result;
    for (size_t c = 0; c < 3; ++c) {
        result[c] = DecodeBase16Digit(hex_code[c * 2]) * kHexBase + hex_code[c * 2 + 1];
    }
    assert(IsCorrect(result) && "Alias: Invalid color HEX code");
    return result;
}

std::string ToHex(Color color) {
    assert(IsCorrect(color) && "Alias: Incorrect Color");
    std::string result = "#";
    for (size_t c = 0; c < 3; ++c) {
        result += Base10ToBase16(color[c], 2);
    }
    return result;
}

bool IsCorrect(Color color) {
    return InRgbRange(color[0]) && InRgbRange(color[1]) && InRgbRange(color[2]);
}

}  // namespace engine::colors
