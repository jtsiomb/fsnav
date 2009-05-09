#ifndef FSTREE_H_
#define FSTREE_H_

#include <time.h>
#include <vector>
#include <vmath.h>

class Dir;
class FSNode;

enum LayoutParameter {
	LP_FILE_SIZE,
	LP_FILE_SPACING,
	LP_FILE_HEIGHT,

	LP_DIR_SIZE,
	LP_DIR_SPACING,
	LP_DIR_HEIGHT,
	LP_DIR_DIST,

	NUM_LAYOUT_PARAMS
};

void set_layout_param(LayoutParameter param, float val);
float get_layout_param(LayoutParameter param);

FSNode *get_selection();

// scans the filesystem, builds the tree
bool build_tree(Dir *tree, const char *dirname);

class Link {
public:
	Dir *from, *to;
	bool selected;

	Link(Dir *from, Dir *to);

	void draw() const;
	bool intersect(const Ray &ray, float *pt) const;
};


class FSNode {
protected:
	char *name;
	size_t size;

	Vector3 vis_pos, vis_size;

	FSNode *parent;

public:
	bool selected;

	FSNode();
	virtual ~FSNode();

	void set_name(const char *name);
	const char *get_name() const;

	void set_size(size_t sz);
	size_t get_size() const;

	void set_vis_pos(const Vector3 &vpos);
	const Vector3 &get_vis_pos() const;

	void set_vis_size(const Vector3 &vsize);
	const Vector3 &get_vis_size() const;

	void set_parent(FSNode *p);
	const FSNode *get_parent() const;

	virtual Vector3 get_text_pos() const = 0;
	virtual float get_text_size() const = 0;

	virtual void draw() const;
	virtual bool intersect(const Ray &ray, float *pt) const;
};

enum { ATIME, MTIME, CTIME };

class File : public FSNode {
protected:
	int num_links;
	int mode, uid, gid;
	time_t time[3];

public:
	File();
	virtual ~File();

	void set_links(int nlinks);
	int get_links() const;

	void set_mode(int mode);
	int get_mode() const;

	void set_uid(int uid);
	int get_uid() const;

	void set_gid(int gid);
	int get_gid() const;

	void set_time(int which, time_t t);
	time_t get_time(int which) const;

	virtual Vector3 get_text_pos() const;
	virtual float get_text_size() const;
};

class Dir : public FSNode {
protected:
	std::vector<Dir*> subdirs;
	std::vector<File*> files;
	std::vector<Link> links;

	float min_x, max_x;

	void calc_bounds();
	void place(const Vector3 &pos);

	FSNode *find_intersection(const Ray &ray, float *pt);

public:
	Dir();
	virtual ~Dir();

	void add_subdir(Dir *dir);
	void add_file(File* file);

	Dir *get_subdirs() const;
	int get_num_subdirs() const;

	File *get_files() const;
	int get_num_files() const;

	Link *get_links() const;
	int get_num_links() const;

	void layout();

	virtual void draw() const;

	virtual Vector3 get_text_pos() const;
	virtual float get_text_size() const;

	virtual bool pick(const Ray &ray);
};

#endif	// FSTREE_H_
