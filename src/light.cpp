#include "light.h"
#include "alias.h"

namespace engine {

LightSource::LightSource(Type energy, Color emission_color)
    : color_{emission_color}, energy_(energy) {
}

void LightSource::SetColor(Color new_color) {
    assert(engine::colors::IsCorrect(new_color) && "Light: Invalid color value");
    color_ = new_color;
}

void LightSource::SetEnergy(Type new_energy) {
    assert(new_energy <= 1 && new_energy >= 0 && "Light: Invalid energy value");
    energy_ = new_energy;
}

Color LightSource::GetColor() const {
    return color_;
}

LightSource::Type LightSource::GetEnergy() const {
    return energy_;
}

}  // namespace engine
