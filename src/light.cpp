#include "light.h"

namespace engine {

LightSource::LightSource(Type energy, Color emission_color)
    : color_{emission_color}, energy_(energy) {
}

void LightSource::SetColor(Color new_color) {
    color_ = new_color;
}

void LightSource::SetEnergy(Type new_energy) {
    energy_ = new_energy;
}

Color LightSource::GetColor() const {
    return color_;
}

LightSource::Type LightSource::GetEnergy() const {
    return energy_;
}

}  // namespace engine
