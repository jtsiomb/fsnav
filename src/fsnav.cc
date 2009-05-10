#include <stdio.h>
#include <assert.h>
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
#include "image.h"

#ifndef GL_BGRA
#define GL_BGRA		0x80e1
#endif

#ifndef PREFIX
#define PREFIX	"/usr/local"
#endif

const char *find_data_file(const char *fname);
void disp();
Ray calc_mouse_ray(int x, int y);
void reshape(int x, int y);
void keyb(unsigned char key, int x, int y);
void keyb_up(unsigned char key, int x, int y);
void mouse(int bn, int state, int x, int y);
void motion(int x, int y);
void passive_motion(int x, int y);
void double_click(int x, int y);
unsigned int load_texture(const char *fname);

static float cam_theta = 0, cam_phi = 25, cam_dist = 5;
static float cam_y = 0;
static Vector3 cam_from, cam_targ;
static unsigned int cam_motion_start;

static float mouse_x, mouse_y;
static Ray mouse_ray;

static Dir *root;
static int xsz, ysz;

static FSNode *clicked_node;
static bool hover_file_info;

unsigned int fontrm, fonttt, fonttt_sm;
unsigned int scope_tex;

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
	glutKeyboardUpFunc(keyb_up);
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

	if(!(fontrm = create_font(find_data_file("kerkis.pfb"), 32))) {
		return 1;
	}
	bind_font(fontrm);
	set_text_color(0.95, 0.9, 0.8, 1.0);

	if(!(fonttt = create_font(find_data_file("courbd.ttf"), 16))) {
		return 1;
	}
	bind_font(fonttt);
	set_text_color(1.0, 1.0, 1.0, 1.0);

	if(!(fonttt_sm = create_font(find_data_file("courbd.ttf"), 14))) {
		return 1;
	}
	bind_font(fonttt_sm);
	set_text_color(1.0, 1.0, 1.0, 1.0);

	unsigned int font_texid = get_font_texture();
	glBindTexture(GL_TEXTURE_2D, font_texid);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	if(!(scope_tex = load_texture(find_data_file("scope.png")))) {
		return 1;
	}

	glEnable(GL_NORMALIZE);
	glEnable(GL_LINE_SMOOTH);

	glutMainLoop();
	return 0;
}

const char *find_data_file(const char *fname)
{
	static char buf[2048];
	const char *dirs[] = {
		PREFIX "/share/fsnav",
		"/usr/local/share/fsnav",
		"/usr/share/fsnav",
		"data",
		0
	};

	for(int i=0; dirs[i]; i++) {
		FILE *fp;
		sprintf(buf, "%s/%s", dirs[i], fname);
		if((fp = fopen(buf, "rb"))) {
			fclose(fp);
			return buf;
		}
	}
	return fname;
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
	glTranslatef(0, 0, -cam_dist);
	glRotatef(cam_phi, 1, 0, 0);
	glRotatef(cam_theta, 0, 1, 0);
	glTranslatef(-cam_pos.x, -cam_pos.y, -cam_pos.z);

	glLightfv(GL_LIGHT0, GL_POSITION, lpos);

	draw_env();
	root->draw();

	FSNode *sel = get_selection();
	if((sel && hover_file_info) || clicked_node) {
		if(!hover_file_info) {
			sel = clicked_node;
		}

		if(dynamic_cast<File*>(sel)) {
			draw_file_stats((File*)sel);
		}
	}

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
	gluPerspective(50.0, (float)x / (float)y, 0.5, 500.0);
}

void keyb(unsigned char key, int x, int y)
{
	switch(key) {
	case 27:
		exit(0);

	case ' ':
		hover_file_info = true;
		clicked_node = 0;
		glutPostRedisplay();
		break;

	default:
		break;
	}
}

void keyb_up(unsigned char key, int x, int y)
{
	if(key == ' ') {
		hover_file_info = false;
		glutPostRedisplay();
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
		if(bn == GLUT_LEFT_BUTTON) {
			if(x == prev_left_x && y == prev_left_y) {
				clicked_node = get_selection();
				glutPostRedisplay();
			}
		}

		prev_x = -1;
	}
}

void motion(int x, int y)
{
	if(bnstate[0]) {
		cam_theta += (x - prev_x) * 0.5;
		cam_phi += (y - prev_y) * 0.5;

		if(cam_phi < 5) cam_phi = 5;
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
	mouse_x = (float)x / (float)xsz;
	mouse_y = (float)y / (float)ysz;

	mouse_ray = calc_mouse_ray(x, ysz - y);
	if(root->pick(mouse_ray)) {
		glutPostRedisplay();
	}

	if(hover_file_info) {
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

unsigned int load_texture(const char *fname)
{
	void *img;
	int width, height;
	unsigned int tex;

	if(!(img = load_image(fname, &width, &height))) {
		fprintf(stderr, "failed to load image: %s\n", fname);
		return 0;
	}

	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, img);

	free_image(img);
	return tex;
}
