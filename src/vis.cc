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
	glLineWidth(link->selected ? 3.0 : 2.0);

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

void draw_file_stats(File *file)
{
}
