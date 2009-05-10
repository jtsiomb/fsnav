#include <iostream>

#if defined(__APPLE__) && defined(__MACH__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "vis.h"
#include "colorman.h"
#include "text.h"

static void draw_cube(float sz);
static const char *mode_str(unsigned int mode);

extern unsigned int fonttt, fontrm, fonttt_sm;

void draw_env()
{
	float col[] = {0.05, 0.3, 0.03, 1.0};

	glPushAttrib(GL_LIGHTING_BIT);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, col);

	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);
	glVertex3f(-500, 0, 500);
	glVertex3f(500, 0, 500);
	glVertex3f(500, 0, -500);
	glVertex3f(-500, 0, -500);
	glEnd();

	glPopAttrib();
}

void draw_node(const FSNode *node)
{
	Vector3 col = get_color(node);
	float glcol[] = {col.x, col.y, col.z, 1.0};
	float zero[] = {0, 0, 0, 0};

	Vector3 pos = node->get_vis_pos();
	Vector3 sz = node->get_vis_size();

	glPushAttrib(GL_LIGHTING_BIT);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, glcol);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, zero);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslatef(pos.x, pos.y, pos.z);
	glScalef(sz.x, sz.y, sz.z);

	draw_cube(1.0);

	glPopMatrix();
	glPopAttrib();
}

void draw_node_text(const FSNode *node)
{
	const char *name = node->get_name();
	if(name) {
		Vector3 tpos = node->get_text_pos();
		float tsize = node->get_text_size();

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glTranslatef(tpos.x, tpos.y + 0.01, tpos.z + get_line_advance() / 2.0);
		glRotatef(-60, 1, 0, 0);

		bind_font(fontrm);
		set_text_mode(TEXT_MODE_3D);
		set_text_size(tsize);
		set_text_pos(0.5 - get_text_width(name) * 0.5, 0.5);

		glDepthMask(0);
		print_string(name);
		glDepthMask(1);

		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
	}
}

void draw_link(const Link *link)
{
	Vector3 start = link->from->get_vis_pos();
	Vector3 end = link->to->get_vis_pos();

	glPushAttrib(GL_ENABLE_BIT | GL_LINE_BIT);
	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);
	glLineWidth(2.0);//link->selected ? 2.0 : 1.0);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslatef(0.0, 0.0, 0.0);

	glBegin(GL_LINES);
	glColor3f(0.1, link->selected ? 1.0 : 0.75, 0.2);
	glVertex3f(start.x, start.y, start.z);
	glVertex3f(end.x, end.y, end.z);
	glEnd();

	glPopMatrix();
	glPopAttrib();
}

static void draw_cube(float sz)
{
	static int dlist;

	if(dlist) {
		glCallList(dlist);
		return;
	}

	dlist = glGenLists(1);
	glNewList(dlist, GL_COMPILE_AND_EXECUTE);

	float hsz = sz * 0.5f;

	glBegin(GL_QUADS);
	/* far face (-Z) */
	glNormal3f(0, 0, -1);
	glTexCoord2f(1, 1);	glVertex3f(hsz, -hsz, -hsz);
	glTexCoord2f(0, 1);	glVertex3f(-hsz, -hsz, -hsz);
	glTexCoord2f(0, 0);	glVertex3f(-hsz, hsz, -hsz);
	glTexCoord2f(1, 0);	glVertex3f(hsz, hsz, -hsz);
	/* top face (+Y) */
	glNormal3f(0, 1, 0);
	glTexCoord2f(0, 1); glVertex3f(-hsz, hsz, hsz);
	glTexCoord2f(1, 1); glVertex3f(hsz, hsz, hsz);
	glTexCoord2f(1, 0); glVertex3f(hsz, hsz, -hsz);
	glTexCoord2f(0, 0); glVertex3f(-hsz, hsz, -hsz);
	/* bottom face (-Y) */
	glNormal3f(0, -1, 0);
	glTexCoord2f(0, 1); glVertex3f(-hsz, -hsz, -hsz);
	glTexCoord2f(1, 1); glVertex3f(hsz, -hsz, -hsz);
	glTexCoord2f(1, 0); glVertex3f(hsz, -hsz, hsz);
	glTexCoord2f(0, 0); glVertex3f(-hsz, -hsz, hsz);
	/* right face (+X) */
	glNormal3f(1, 0, 0);
	glTexCoord2f(0, 1); glVertex3f(hsz, -hsz, hsz);
	glTexCoord2f(1, 1); glVertex3f(hsz, -hsz, -hsz);
	glTexCoord2f(1, 0); glVertex3f(hsz, hsz, -hsz);
	glTexCoord2f(0, 0); glVertex3f(hsz, hsz, hsz);
	/* left face (-X) */
	glNormal3f(-1, 0, 0);
	glTexCoord2f(0, 1); glVertex3f(-hsz, -hsz, -hsz);
	glTexCoord2f(1, 1); glVertex3f(-hsz, -hsz, hsz);
	glTexCoord2f(1, 0); glVertex3f(-hsz, hsz, hsz);
	glTexCoord2f(0, 0); glVertex3f(-hsz, hsz, -hsz);
	/* near face (+Z) */
	glNormal3f(0, 0, 1);
	glTexCoord2f(0, 1); glVertex3f(-hsz, -hsz, hsz);
	glTexCoord2f(1, 1); glVertex3f(hsz, -hsz, hsz);
	glTexCoord2f(1, 0); glVertex3f(hsz, hsz, hsz);
	glTexCoord2f(0, 0); glVertex3f(-hsz, hsz, hsz);
	glEnd();

	glEndList();
}

void draw_file_stats(const File *file)
{
	double mvmat[16], proj[16];
	int viewport[4];
	double x, y, z;

	glGetDoublev(GL_MODELVIEW_MATRIX, mvmat);
	glGetDoublev(GL_PROJECTION_MATRIX, proj);
	glGetIntegerv(GL_VIEWPORT, viewport);

	Vector3 pos = file->get_vis_pos();

	gluProject(pos.x, pos.y, pos.z, mvmat, proj, viewport, &x, &y, &z);

	draw_file_stats(file, x / viewport[2], 1.0 - y / viewport[3]);
}


#define newline()	\
	set_text_pos(mx + xoffs, get_text_pos().y);	\
	text_line_advance(1)

void draw_file_stats(const File *file, float mx, float my)
{
	extern unsigned int scope_tex;

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	float x = 2.0 * mx - 1.0;
	float y = 1.0 - 2.0 * my;

	glTranslatef(x + 0.28, y + 0.12, 0);
	glScalef(0.4, 0.25, 1);

	glPushAttrib(GL_ENABLE_BIT);
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, scope_tex);

	glBegin(GL_QUADS);
	glTexCoord2f(0, 1);
	glVertex2f(-1, -1);
	glTexCoord2f(1, 1);
	glVertex2f(1, -1);
	glTexCoord2f(1, 0);
	glVertex2f(1, 1);
	glTexCoord2f(0, 0);
	glVertex2f(-1, 1);
	glEnd();

	glPopAttrib();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();


	float xoffs = 0.15;
	float yoffs = -0.19;

	bind_font(fonttt);

	glDisable(GL_DEPTH_TEST);
	set_text_mode(TEXT_MODE_2D);
	set_text_pos(mx + xoffs, my + yoffs);
	set_text_size(1.0);

	print_string(file->get_name());

	bind_font(fonttt_sm);
	xoffs = 0.085;
	set_text_pos(mx + xoffs, my + yoffs + 0.04);
	set_text_size(1.0);

	char buf[512];

	print_string("    size: ");
	size_t filesize = file->get_size();

	if(filesize < 1024) {
		sprintf(buf, "%d bytes", (int)filesize);
	} else if(filesize < SQ(1024)) {
		sprintf(buf, "%.1f kb", (float)filesize / 1024.0);
	} else if(filesize < SQ(1024) * 1024) {
		sprintf(buf, "%.1f mb", (float)filesize / SQ(1024.0));
	} else {
		sprintf(buf, "%.1f gb", (float)filesize / (SQ(1024.0) * 1024.0));
	}
	print_string(buf);
	newline();

	print_string("    perm: ");
	print_string(mode_str(file->get_mode()));
	newline();

	print_string("    user: ");
	sprintf(buf, "%s (%d)\n", file->get_user(), file->get_uid());
	print_string(buf);
	newline();

	print_string("   group: ");
	sprintf(buf, "%s (%d)\n", file->get_group(), file->get_gid());
	print_string(buf);
	newline();

	print_string("accessed: ");
	time_t atime = file->get_time(ATIME);
	sprintf(buf, "%s\n", asctime(localtime(&atime)));
	print_string(buf);
	newline();

	print_string("modified: ");
	time_t mtime = file->get_time(MTIME);
	sprintf(buf, "%s\n", asctime(localtime(&mtime)));
	print_string(buf);
	newline();

	print_string(" created: ");
	time_t ctime = file->get_time(CTIME);
	sprintf(buf, "%s\n", asctime(localtime(&ctime)));
	print_string(buf);
	newline();
	
	glEnable(GL_DEPTH_TEST);
}

static const char *mode_str(unsigned int mode)
{
	static char str[10];
	char *ptr = str + 8;

	int bit = 0;
	for(int i=0; i<3; i++) {
		*ptr-- = (mode & (1 << bit++)) ? 'x' : '-';
		*ptr-- = (mode & (1 << bit++)) ? 'w' : '-';
		*ptr-- = (mode & (1 << bit++)) ? 'r' : '-';
	}
	return str;
}

