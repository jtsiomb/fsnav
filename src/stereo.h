#ifndef STEREO_H_
#define STEREO_H_

enum { VIEW_CENTER, VIEW_LEFT, VIEW_RIGHT };

#ifdef __cplusplus
extern "C" {
#endif

void stereo_proj_param(float vfov, float aspect, float near, float far);
void stereo_focus_dist(float d);

void stereo_view_matrix(int eye);
void stereo_proj_matrix(int eye);

#ifdef __cplusplus
}
#endif

#endif	/* STEREO_H_ */
