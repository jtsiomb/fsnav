#include <stdlib.h>

/** Generates a random number in [0, range) */
static inline scalar_t frand(scalar_t range)
{
	return range * (scalar_t)rand() / (scalar_t)RAND_MAX;
}

/** Generates a random vector on the surface of a sphere */
static inline vec3_t sphrand(scalar_t rad)
{
	scalar_t u = (scalar_t)rand() / RAND_MAX;
	scalar_t v = (scalar_t)rand() / RAND_MAX;

	scalar_t theta = 2.0 * M_PI * u;
	scalar_t phi = acos(2.0 * v - 1.0);

	vec3_t res;
	res.x = rad * cos(theta) * sin(phi);
	res.y = rad * sin(theta) * sin(phi);
	res.z = rad * cos(phi);
	return res;
}
