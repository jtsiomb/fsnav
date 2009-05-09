#include "basis.h"
#include "matrix.h"

void basis_matrix(mat4_t res, vec3_t i, vec3_t j, vec3_t k)
{
	m4_identity(res);
	m4_set_column(res, v4_cons(i.x, i.y, i.z, 1.0), 0);
	m4_set_column(res, v4_cons(j.x, j.y, j.z, 1.0), 1);
	m4_set_column(res, v4_cons(k.x, k.y, k.z, 1.0), 2);
}

void basis_matrix_dir(mat4_t res, vec3_t dir)
{
	vec3_t k = v3_normalize(dir);
	vec3_t j = {0, 1, 0};
	vec3_t i = v3_cross(j, k);
	j = v3_cross(k, i);
	basis_matrix(res, i, j, k);
}
