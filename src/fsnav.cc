#include <stdio.h>
#include "fstree.h"

#if defined(__APPLE__) && defined(__MACH__)
#include <GLUT/glut.h>
#include <OpenGL/glu.h>
#else
#include <GL/glut.h>
#include <GL/glu.h>
#endif

#include "fstree.h"
#include "text.h"
#include "vis.h"

void disp();
Ray calc_mouse_ray(int x, int y);
void reshape(int x, int y);
void keyb(unsigned char key, int x, int y);
void mouse(int bn, int state, int x, int y);
void motion(int x, int y);
void passive_motion(int x, int y);
void double_click(int x, int y);

static float cam_theta = 0, cam_phi = 25, cam_dist = 5;
static float cam_y = 0;
static Vector3 cam_from, cam_targ;
static unsigned int cam_motion_start;

static Ray mouse_ray;

static Dir *root;
static unsigned int font;

static int xsz, ysz;

int main(int argc, char **argv)
{
	char *root_dirname = ".";

	glutInitWindowSize(800, 600);
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
	glutCreateWindow("Filesystem Visualizer");

	glutDisplayFunc(disp);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyb);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutPassiveMotionFunc(passive_motion);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	if(argc > 1) {
		root_dirname = argv[1];
	}

	set_layout_param(LP_FILE_SIZE, 0.5);
	set_layout_param(LP_FILE_SPACING, 0.1);
	set_layout_param(LP_FILE_HEIGHT, 0.1);
	
	set_layout_param(LP_DIR_SIZE, 0.5 + 0.2);
	set_layout_param(LP_DIR_SPACING, 0.5);
	set_layout_param(LP_DIR_HEIGHT, 0.1);
	set_layout_param(LP_DIR_DIST, 5.0);

	root = new Dir;
	if(!build_tree(root, root_dirname)) {
		return 1;
	}
	root->layout();

	if(!(font = create_font("data/kerkis.pfb", 32))) {
		return 1;
	}
	bind_font(font);
	set_text_color(1.0, 1.0, 1.0, 1.0);
	set_text_mode(TEXT_MODE_3D);

	glEnable(GL_NORMALIZE);

	glutMainLoop();
	return 0;
}

#define TRANS_TIME	0.8
void disp()
{
	unsigned int msec = glutGet(GLUT_ELAPSED_TIME);
	float lpos[] = {-0.5, 1, 0.5, 0};

	float t = (msec - cam_motion_start) / 1000.0 / TRANS_TIME;
	if(t > 1.0) {
		t = 1.0;
	}
	Vector3 cam_pos = lerp(cam_from, cam_targ, t);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0, -cam_y, -cam_dist);
	glRotatef(cam_phi, 1, 0, 0);
	glRotatef(cam_theta, 0, 1, 0);
	glTranslatef(-cam_pos.x, -cam_pos.y, -cam_pos.z);

	glLightfv(GL_LIGHT0, GL_POSITION, lpos);

	draw_env();
	root->draw();

	/*
	glPushAttrib(GL_ENABLE_BIT);
	glDisable(GL_LIGHTING);
	glBegin(GL_LINES);
	Vector3 v0 = mouse_ray.origin;
	Vector3 v1 = mouse_ray.origin + mouse_ray.dir * 0.1;

	glColor3f(1, 0, 0);
	glVertex3f(v0.x, v0.y, v0.z);
	glVertex3f(v1.x, v1.y, v1.z);
	glEnd();
	glPopAttrib();
	*/

	glutSwapBuffers();
	assert(glGetError() == GL_NO_ERROR);

	if(t < 1.0) {
		glutPostRedisplay();
	}
}

Ray calc_mouse_ray(int x, int y)
{
	Ray ray;

	double mvmat[16], proj[16];
	int viewport[4];

	glGetDoublev(GL_MODELVIEW_MATRIX, mvmat);
	glGetDoublev(GL_PROJECTION_MATRIX, proj);
	glGetIntegerv(GL_VIEWPORT, viewport);

	double res_x, res_y, res_z;
	gluUnProject(x, y, 0.0, mvmat, proj, viewport, &res_x, &res_y, &res_z);

	ray.origin = Vector3(res_x, res_y, res_z);

	gluUnProject(x, y, 1.0, mvmat, proj, viewport, &res_x, &res_y, &res_z);
	ray.dir = Vector3(res_x, res_y, res_z) - ray.origin;

	return ray;
}

void reshape(int x, int y)
{
	xsz = x;
	ysz = y;
	glViewport(0, 0, x, y);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(50.0, (float)x / (float)y, 1.0, 1000.0);
}

void keyb(unsigned char key, int x, int y)
{
	switch(key) {
	case 27:
		exit(0);

	default:
		break;
	}
}

#define DOUBLE_CLICK_INTERVAL	400
static int bnstate[16];

static int prev_x = -1, prev_y;
void mouse(int bn, int state, int x, int y)
{
	static unsigned int prev_left_click;
	static int prev_left_x, prev_left_y;

	bnstate[bn] = state == GLUT_DOWN ? 1 : 0;
	if(state == GLUT_DOWN) {
		if(bn == GLUT_LEFT_BUTTON) {
			unsigned int msec = glutGet(GLUT_ELAPSED_TIME);
			int dx = abs(x - prev_left_x);
			int dy = abs(y - prev_left_y);

			if(msec - prev_left_click < DOUBLE_CLICK_INTERVAL && dx < 3 && dy < 3) {
				double_click(x, y);
				prev_left_click = 0;
			} else {
				prev_left_click = msec;
				prev_left_x = x;
				prev_left_y = y;
			}
		}

		if(bn == 3) {
			cam_dist -= 0.5;
			glutPostRedisplay();
			if(cam_dist < 0) cam_dist = 0;
		} else if(bn == 4) {
			cam_dist += 0.5;
			glutPostRedisplay();
		} else {
			prev_x = x;
			prev_y = y;
		}
	} else {
		prev_x = -1;
	}
}

void motion(int x, int y)
{
	if(bnstate[0]) {
		cam_theta += (x - prev_x) * 0.5;
		cam_phi += (y - prev_y) * 0.5;

		if(cam_phi < -90) cam_phi = -90;
		if(cam_phi > 90) cam_phi = 90;

		glutPostRedisplay();
	}

	if(bnstate[1]) {
		cam_y += (prev_y - y) * 0.1;
		glutPostRedisplay();
	}

	if(bnstate[2]) {
		cam_dist += (y - prev_y) * 0.1;
		glutPostRedisplay();
	}

	prev_x = x;
	prev_y = y;
}

void passive_motion(int x, int y)
{
	mouse_ray = calc_mouse_ray(x, ysz - y);
	if(root->pick(mouse_ray)) {
		glutPostRedisplay();
	}
}

void double_click(int x, int y)
{
	FSNode *selnode = get_selection();

	if(selnode) {
		cam_from = cam_targ;
		cam_targ = selnode->get_vis_pos();
		cam_motion_start = glutGet(GLUT_ELAPSED_TIME);
		glutPostRedisplay();
	}
}
