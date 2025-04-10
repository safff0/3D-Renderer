#pragma once
#include "alias.h"

namespace engine {

class LightSource {
public:
    using Type = Real;

    LightSource() = default;
    LightSource(Type energy, Color emission_color = kDefaultColor);

    void SetColor(Color new_color);
    void SetEnergy(Type new_energy);

    Color GetColor() const;
    Type GetEnergy() const;

private:
    static constexpr Color kDefaultColor = colors::kColorWhite;
    static constexpr Type kDefaultEnergy = 1.0;

    Color color_ = kDefaultColor;
    Type energy_ = kDefaultEnergy;
};

}  // namespace engine
