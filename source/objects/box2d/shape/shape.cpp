#include "objects/box2d/shape/shape.h"

using namespace love;

love::Type Shape::type("Shape", &Object::type);

Shape::~Shape()
{}

bool Shape::GetConstant(const char* in, Shape::Type& out)
{
    return types.Find(in, out);
}

bool Shape::GetConstant(Shape::Type in, const char*& out)
{
    return types.Find(in, out);
}

// clang-format off
constexpr StringMap<Shape::Type, Shape::SHAPE_MAX_ENUM>::Entry typeEntries[] =
{
    { "circle",  Shape::SHAPE_CIRCLE  },
    { "polygon", Shape::SHAPE_POLYGON },
    { "edge",    Shape::SHAPE_EDGE    },
    { "chain",   Shape::SHAPE_CHAIN   }
};

constinit const StringMap<Shape::Type, Shape::SHAPE_MAX_ENUM> Shape::types(typeEntries);
// clang-format on
