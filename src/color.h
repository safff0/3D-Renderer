#pragma once
#include "alias.h"

#include <glm/glm.hpp>

namespace engine {

using Color = glm::vec<3, Index>;

namespace colors {

constexpr inline Color kColorWhite = {255, 255, 255};
constexpr inline Color kColorLightGray = {191, 191, 191};
constexpr inline Color kColorGray = {127, 127, 127};
constexpr inline Color kColorBlack = {0, 0, 0};

constexpr inline Color kColorRed = {250, 0, 0};
constexpr inline Color kColorDarkRed = {139, 0, 0};
constexpr inline Color kColorOrange = {255, 165, 0};
constexpr inline Color kColorYellow = {250, 250, 0};
constexpr inline Color kColorPink = {250, 105, 180};
constexpr inline Color kColorBrown = {205, 133, 63};

constexpr inline Color kColorGreen = {0, 128, 0};
constexpr inline Color kColorLime = {50, 210, 50};
constexpr inline Color kColorDarkGreen = {0, 90, 0};

constexpr inline Color kColorBlue = {0, 0, 250};
constexpr inline Color kColorDarkBlue = {0, 0, 139};
constexpr inline Color kColorCyan = {0, 250, 250};
constexpr inline Color kColorPurple = {150, 0, 210};
constexpr inline Color kColorDarkPurple = {75, 0, 130};

Color FromHex(std::string_view hex_code);
std::string ToHex(Color color);

bool IsCorrect(Color color);

}  // namespace colors

}  // namespace engine
