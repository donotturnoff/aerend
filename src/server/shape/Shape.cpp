#include "Shape.h"
#include "net/Client.h"

namespace aerend {

Shape::Shape(Client& client, Colour colour) : sid(client.next_sid()), colour{colour} {}

void Shape::set_colour(Colour colour) {
    this->colour = colour;
}

Colour Shape::get_colour() const noexcept {
    return colour;
}

uint32_t Shape::get_sid() const noexcept {
    return sid;
}

}
