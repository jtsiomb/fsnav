#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <float.h>
#include <assert.h>
#include <unistd.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <sys/stat.h>
#include "fstree.h"
#include "vis.h"
#include "text.h"

using namespace std;

static Vector2 calc_dir_size(int num_files);


static float params[NUM_LAYOUT_PARAMS];
static FSNode *selnode;


void set_layout_param(LayoutParameter which, float val)
{
	params[which] = val;
}

float get_layout_param(LayoutParameter which)
{
	return params[which];
}

FSNode *get_selection()
{
	return selnode;
}

#ifndef PATH_MAX
#define PATH_MAX	1024
#endif

bool build_tree(Dir *tree, const char *dirname)
{
	DIR *dir;
	struct dirent *dent;
	char cur_dir[PATH_MAX];

	tree->set_name(dirname);

	if(!(dir = opendir(dirname))) {
		fprintf(stderr, "failed to open dir: %s: %s\n", dirname, strerror(errno));
		return false;
	}

	getcwd(cur_dir, PATH_MAX);
	if(chdir(dirname) == -1) {
		fprintf(stderr, "chdir(\"%s\") failed: %s\n", dirname, strerror(errno));
		closedir(dir);
		return false;
	}

	while((dent = readdir(dir))) {
		struct stat st;

		if(stat(dent->d_name, &st) == -1) {
			fprintf(stderr, "%s: stat failed: %s\n", dent->d_name, strerror(errno));
			continue;
		}

		if(st.st_mode & S_IFDIR) {
			if(strcmp(dent->d_name, ".") == 0 || strcmp(dent->d_name, "..") == 0) {
				continue;
			}

			printf("found dir: %s\n", dent->d_name);

			Dir *node = new Dir;
			tree->add_subdir(node);

			build_tree(node, dent->d_name);
		} else {
			File *file = new File;
			file->set_name(dent->d_name);
			file->set_size(st.st_size);
			file->set_mode(st.st_mode);
			file->set_uid(st.st_uid);
			file->set_gid(st.st_gid);
			file->set_time(ATIME, st.st_atime);
			file->set_time(MTIME, st.st_mtime);
			file->set_time(CTIME, st.st_ctime);
			tree->add_file(file);
		}
	}
	closedir(dir);

	chdir(cur_dir);
	return true;
}



// --- link between directories ---

Link::Link(Dir *from, Dir *to)
{
	this->from = from;
	this->to = to;
	selected = false;
}

void Link::draw() const
{
	draw_link(this);
}

bool Link::intersect(const Ray &ray, float *pt) const
{
	return false;	// TODO implement
}

// --- abstract base class FSNode ---

FSNode::FSNode()
{
	name = 0;
	size = 0;
	parent = 0;
	selected = false;
}

FSNode::~FSNode()
{
	delete [] name;
}

void FSNode::set_name(const char *name)
{
	delete [] this->name;
	this->name = new char[strlen(name) + 1];
	strcpy(this->name, name);
}

const char *FSNode::get_name() const
{
	return name;
}

void FSNode::set_size(size_t sz)
{
	size = sz;
}

size_t FSNode::get_size() const
{
	return size;
}

void FSNode::set_vis_pos(const Vector3 &vpos)
{
	vis_pos = vpos;
}

const Vector3 &FSNode::get_vis_pos() const
{
	return vis_pos;
}

void FSNode::set_vis_size(const Vector3 &vsize)
{
	vis_size = vsize;
}

const Vector3 &FSNode::get_vis_size() const
{
	return vis_size;
}

void FSNode::set_parent(FSNode *p)
{
	parent = p;
}

const FSNode *FSNode::get_parent() const
{
	return parent;
}

void FSNode::draw() const
{
	draw_node(this);
}

enum {
	NEG_Z = 1,
	POS_X = 2,
	POS_Z = 4,
	NEG_X = 8,
	POS_Y = 16,
	NEG_Y = 32
};

bool FSNode::intersect(const Ray &ray, float *pt) const
{
	Vector3 min = vis_pos - vis_size, max = vis_pos + vis_size;

	static const Vector3 pnorm[] = {
		Vector3(0, 0, -1), Vector3(1, 0, 0), Vector3(0, 0, 1),
		Vector3(-1, 0, 0), Vector3(0, 1, 0), Vector3(0, -1, 0)
	};
	const Vector3 *vptr[] = { &min, &max, &max, &min, &max, &min };

	Vector3 nearest_pos;
	int nearest_idx = -1;
	double nearest_t = DBL_MAX;

	for(int i=0; i<6; i++) {
		double n_dot_dir = dot_product(pnorm[i], ray.dir);
		if(fabs(n_dot_dir) < ERROR_MARGIN) {
			continue;	// doesn't intersect this side (parallel)
		}

		Vector3 vo_vec = ray.origin - *vptr[i];
		double t = -dot_product(pnorm[i], vo_vec) / n_dot_dir;
		if(t < ERROR_MARGIN) {
			continue;	// side in the opposite subspace
		}

		// intersection point
		Vector3 pos = ray.origin + ray.dir * t;

		int bit = 1 << i;
		if((bit & (NEG_Z | POS_Z | POS_Y | NEG_Y)) && (pos.x < min.x || pos.x >= max.x)) {
			continue;
		}
		if((bit & (POS_X | NEG_X | POS_Y | NEG_Y)) && (pos.z < min.z || pos.z >= max.z)) {
			continue;
		}
		if((bit & (POS_X | NEG_X | POS_Z | NEG_Z)) && (pos.y < min.y || pos.y >= max.y)) {
			continue;
		}

		if(t < nearest_t) {
			nearest_t = t;
			nearest_idx = i;
			nearest_pos = pos;
		}
	}

	if(nearest_idx == -1) {
		return false;
	}

	if(pt) {
		*pt = nearest_t;
	}
	return true;
}

// --- File class ---

File::File()
{
	num_links = 0;
	mode = 0;
	uid = gid = 0;
	time[0] = time[1] = time[2] = 0;
}

File::~File() {}

void File::set_links(int nlinks)
{
	num_links = nlinks;
}

int File::get_links() const
{
	return num_links;
}

void File::set_mode(int mode)
{
	this->mode = mode;
}

int File::get_mode() const
{
	return mode;
}

void File::set_uid(int uid)
{
	this->uid = uid;
}

int File::get_uid() const
{
	return uid;
}

const char *File::get_user() const
{
	struct passwd *pw = getpwuid(uid);
	if(pw) {
		return pw->pw_name;
	}
	return "unknown";
}

void File::set_gid(int gid)
{
	this->gid = gid;
}

int File::get_gid() const
{
	return gid;
}

const char *File::get_group() const
{
	struct group *gr = getgrgid(gid);
	if(gr) {
		return gr->gr_name;
	}
	return "unknown";
}

void File::set_time(int which, time_t t)
{
	time[which] = t;
}

time_t File::get_time(int which) const
{
	return time[which];
}

Vector3 File::get_text_pos() const
{
	return vis_pos + Vector3(0, vis_size.y, 0);
}

float File::get_text_size() const
{
	return 1.0;
}

// --- directories ---

Dir::Dir()
{
}

Dir::~Dir() {}

void Dir::add_subdir(Dir *dir)
{
	subdirs.push_back(dir);
	dir->set_parent(this);

	Link link(this, dir);
	links.push_back(link);
}

void Dir::add_file(File *file)
{
	files.push_back(file);
	file->set_parent(this);
}

Dir *Dir::get_subdirs() const
{
	return subdirs.empty() ? 0 : (Dir*)&subdirs[0];
}

int Dir::get_num_subdirs() const
{
	return (int)subdirs.size();
}

File *Dir::get_files() const
{
	return files.empty() ? 0 : (File*)&files[0];
}

int Dir::get_num_files() const
{
	return (int)files.size();
}

Link *Dir::get_links() const
{
	return links.empty() ? 0 : (Link*)&links[0];
}

int Dir::get_num_links() const
{
	return (int)links.size();
}

void Dir::layout()
{
	calc_bounds();
	place(Vector3(0, params[LP_DIR_HEIGHT] / 2.0, 0));
}

void Dir::calc_bounds()
{
	Vector2 dir_size = calc_dir_size(files.size());
	vis_size = Vector3(dir_size.x, params[LP_DIR_HEIGHT], dir_size.y);

	float child_width = 0.0;
	for(size_t i=0; i<subdirs.size(); i++) {
		subdirs[i]->calc_bounds();
		child_width += subdirs[i]->max_x - subdirs[i]->min_x;
	}

	float width = MAX(dir_size.x, child_width);

	min_x = -(width + params[LP_DIR_SPACING]) / 2.0;
	max_x = (width + params[LP_DIR_SPACING]) / 2.0;
}

void Dir::place(const Vector3 &pos)
{
	Vector3 child_pos;

	vis_pos = pos;

	float x = min_x - params[LP_DIR_SPACING] / 2.0;
	for(size_t i=0; i<subdirs.size(); i++) {
		float width = subdirs[i]->max_x - subdirs[i]->min_x;

		child_pos.x = pos.x + x + width / 2.0;
		child_pos.y = pos.y;
		child_pos.z = pos.z - (vis_size.z / 2.0 + params[LP_DIR_DIST]);

		subdirs[i]->place(child_pos);

		x += width + params[LP_DIR_SPACING];
	}

	// -- place files --
	int num_files = files.size();
	int side_files = (int)ceil(sqrt(num_files));
	float fsize = params[LP_FILE_SIZE];
	float fspace = params[LP_FILE_SPACING];
	float fheight = params[LP_FILE_HEIGHT];

	float frow_width = side_files * fsize + (side_files - 1) * fspace;

	float offs = fsize / 2.0 + fspace;
	Vector3 fstart = vis_pos - vis_size / 2.0 + Vector3(offs, vis_size.y + fheight / 2.0, offs);
	Vector3 fpos = fstart;

	for(size_t i=0; i<files.size(); i++) {
		files[i]->set_vis_pos(fpos);
		files[i]->set_vis_size(Vector3(fsize, fheight, fsize));

		fpos.x += fsize + fspace;
		if(fpos.x - fstart.x > frow_width) {
			fpos.x = fstart.x;
			fpos.z += fsize + fspace;
		}
	}

}

/* the post-order drawing is a nice trick to avoid deferring and sorting
 * transparent text labels :) They're drawn back-to-front this way when
 * the users looks down the hierarchy (otherwise they're not visible anyway)
 */
void Dir::draw() const
{
	assert(links.size() == subdirs.size());
	for(size_t i=0; i<subdirs.size(); i++) {
		subdirs[i]->draw();
		links[i].draw();
	}
	
	for(size_t i=0; i<files.size(); i++) {
		files[i]->draw();
	}
	draw_node(this);

	for(size_t i=0; i<files.size(); i++) {
		draw_node_text(files[i]);
	}
	draw_node_text(this);
}

Vector3 Dir::get_text_pos() const
{
	float zoffs = vis_size.z / 2.0 + get_line_advance() * get_text_size();
	return vis_pos + Vector3(0, 0, zoffs);
}

float Dir::get_text_size() const
{
	return 5.0;
}

FSNode *Dir::find_intersection(const Ray &ray, float *pt)
{
	float nearest_t = FLT_MAX;
	FSNode *nearest_node = 0;

	float t;
	if(intersect(ray, &t) && t < nearest_t) {
		nearest_t = t;
		nearest_node = this;
	}

	for(size_t i=0; i<subdirs.size(); i++) {
		FSNode *node = subdirs[i]->find_intersection(ray, &t);
		if(node && t < nearest_t) {
			nearest_node = node;
			nearest_t = t;
		}
	}

	for(size_t i=0; i<files.size(); i++) {
		if(files[i]->intersect(ray, &t) && t < nearest_t) {
			nearest_node = files[i];
			nearest_t = t;
		}
	}

	if(pt) {
		*pt = nearest_t;
	}
	return nearest_node;
}

bool Dir::pick(const Ray &ray)
{
	FSNode *node = find_intersection(ray, 0);

	bool chng = selnode != node;
	
	if(selnode) {
		selnode->selected = false;
	}
	if(node) {
		selnode = node;
		node->selected = true;
	} else {
		selnode = 0;
	}

	return chng;
}

static Vector2 calc_dir_size(int num_files)
{
	int files_x = (int)ceil(sqrt((float)num_files));
	int files_y = (int)ceil((float)num_files / (float)files_x);

	float xsz = files_x * params[LP_FILE_SIZE] + (files_x + 1) * params[LP_FILE_SPACING];
	float ysz = files_y * params[LP_FILE_SIZE] + (files_y + 1) * params[LP_FILE_SPACING];

	float min_dir_sz = params[LP_DIR_SIZE];
	return Vector2(MAX(xsz, min_dir_sz), MAX(ysz, min_dir_sz));
}

