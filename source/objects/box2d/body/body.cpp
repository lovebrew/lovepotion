#include "objects/box2d/body/body.h"

using namespace love;

love::Type Body::type("Body", &Object::type);

Body::~Body()
{}

bool Body::GetConstant(const char* in, Body::Type& out)
{
    return types.Find(in, out);
}

bool Body::GetConstant(Body::Type in, const char*& out)
{
    return types.Find(in, out);
}

std::vector<const char*> Body::GetConstants(Body::Type)
{
    return types.GetNames();
}

// clang-format off

constexpr StringMap<Body::Type, Body::BODY_MAX_ENUM>::Entry typeEntries[] =
{
    { "static",    Body::BODY_STATIC    },
    { "dynamic",   Body::BODY_DYNAMIC   },
    { "kinematic", Body::BODY_KINEMATIC }
};

constinit const StringMap<Body::Type, Body::BODY_MAX_ENUM> Body::types(typeEntries);

// clang-format on
