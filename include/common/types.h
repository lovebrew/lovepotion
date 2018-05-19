#include <string>
#include <vector>
#include <map>

typedef struct
{
	double ox;
	double oy;

	double r;

	double sx;
	double sy;

	double kx;
	double ky;
} StackMatrix;

extern std::vector<const char *> FILE_MODES;

extern std::vector<const char *> SOURCE_TYPES;