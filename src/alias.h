#pragma once
#include <cstddef>
#include <glm/glm.hpp>

#include <string>
#include <string_view>

namespace engine {

using Real = double;
using Index = int32_t;

const static Real kInfinity = 1e9;
const static Real kEps = 1e-4;

using Vector2 = glm::vec<2, Real>;
using Vector3 = glm::vec<3, Real>;
using Vector4 = glm::vec<4, Real>;
using Matrix3 = glm::mat<3, 3, Real>;
using Matrix4 = glm::mat<4, 4, Real>;

using Triangle3D = std::array<Vector3, 3>;
using Triangle2D = std::array<Vector2, 3>;

template <class T, class Tag>
struct Alias {
    explicit Alias(T value) : value_(value) {
    }

    operator T() const {
        return value_;
    }

private:
    T value_;
};

using Width = Alias<Index, struct width_tag>;
using Height = Alias<Index, struct height_tag>;

using Color = glm::vec<3, Index>;

namespace colors {

constexpr static Color kColorWhite = {255, 255, 255};
constexpr static Color kColorLightGray = {191, 191, 191};
constexpr static Color kColorGray = {127, 127, 127};
constexpr static Color kColorBlack = {0, 0, 0};

constexpr static Color kColorRed = {250, 0, 0};
constexpr static Color kColorDarkRed = {139, 0, 0};
constexpr static Color kColorOrange = {255, 165, 0};
constexpr static Color kColorYellow = {250, 250, 0};
constexpr static Color kColorPink = {250, 105, 180};
constexpr static Color kColorBrown = {205, 133, 63};

constexpr static Color kColorGreen = {0, 128, 0};
constexpr static Color kColorLime = {50, 210, 50};
constexpr static Color kColorDarkGreen = {0, 90, 0};

constexpr static Color kColorBlue = {0, 0, 250};
constexpr static Color kColorDarkBlue = {0, 0, 139};
constexpr static Color kColorCyan = {0, 250, 250};
constexpr static Color kColorPurple = {150, 0, 210};
constexpr static Color kColorDarkPurple = {75, 0, 130};

Color FromHex(std::string_view hex_code);
std::string ToHex(Color color);

bool IsCorrect(Color color);

}  // namespace colors

}  // namespace engine
