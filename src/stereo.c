#include <math.h>

#ifndef __APPLE__
#include <GL/glut.h>
#else
#include <GLUT/glut.h>
#endif

#include "stereo.h"

static float vfov = 45.0;
static float aspect = 1.0;
static float near = 0.5;
static float far = 500.0;

static float eye_sep = 1.0 / 30.0;
static float focus_dist = 1.0;

void stereo_proj_param(float _vfov, float _aspect, float _near, float _far)
{
	vfov = _vfov;
	aspect = _aspect;
	near = _near;
	far = _far;
}

void stereo_focus_dist(float d)
{
	focus_dist = d;
	eye_sep = d / 30.0;
}

void stereo_view_matrix(int eye)
{
	static const float offs_sign[] = {0.0f, 0.5f, -0.5f};
	glTranslatef(eye_sep * offs_sign[eye], 0, 0);
}

void stereo_proj_matrix(int eye)
{
	float vfov_rad = M_PI * vfov / 180.0;
	float top = near * tan(vfov_rad * 0.5);
	float right = top * aspect;

	static const float offs_sign[] = {0.0f, 1.0, -1.0};	/* center, left, right */
	float frust_shift = offs_sign[eye] * (eye_sep * 0.5 * near / focus_dist);

	glFrustum(-right + frust_shift, right + frust_shift, -top, top, near, far);
}
