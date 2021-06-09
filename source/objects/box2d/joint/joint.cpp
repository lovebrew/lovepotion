#include "objects/box2d/joint/joint.h"

using namespace love;

love::Type Joint::type("Joint", &Object::type);

bool Joint::GetConstant(const char* in, Joint::Type& out)
{
    return types.Find(in, out);
}

bool Joint::GetConstant(Joint::Type in, const char*& out)
{
    return types.Find(in, out);
}

// clang-format off
constexpr StringMap<Joint::Type, Joint::JOINT_MAX_ENUM>::Entry typeEntries[] =
{
	{ "distance",  Joint::JOINT_DISTANCE  },
	{ "revolute",  Joint::JOINT_REVOLUTE  },
	{ "prismatic", Joint::JOINT_PRISMATIC },
	{ "mouse",     Joint::JOINT_MOUSE     },
	{ "pulley",    Joint::JOINT_PULLEY    },
	{ "gear",      Joint::JOINT_GEAR      },
	{ "friction",  Joint::JOINT_FRICTION  },
	{ "weld",      Joint::JOINT_WELD      },
	{ "wheel",     Joint::JOINT_WHEEL     },
	{ "rope",      Joint::JOINT_ROPE      },
	{ "motor",     Joint::JOINT_MOTOR     }
};

constinit const StringMap<Joint::Type, Joint::JOINT_MAX_ENUM> Joint::types(typeEntries);
// clang-format on
