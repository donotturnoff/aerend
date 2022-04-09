#ifndef SHAPE_H
#define SHAPE_H

#include "style/Colour.h"
#include "bitmap/Bitmap.h"

namespace aerend {

class Client;

class Shape {
public:
    Shape(Client& client, Colour colour);
    virtual ~Shape() = default;
    virtual void paint(Bitmap& bmp) = 0;
    virtual void set_colour(Colour colour);
    virtual Colour get_colour() const noexcept;
    uint32_t get_sid() const noexcept;
protected:
    uint32_t sid;
    Colour colour;
};

}

#endif // SHAPE_H

