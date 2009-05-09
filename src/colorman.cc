#include "colorman.h"
#include "fstree.h"

static const Vector3 dir_color[] = {
	Vector3(0.263, 0.396, 0.647),
	Vector3(0.3, 0.5, 1.0)
};
static const Vector3 file_color[] = {
	Vector3(0.278, 0.023, 0.023),
	Vector3(0.4, 0.2, 0.1)
};

Vector3 get_color(const FSNode *node)
{
	if(dynamic_cast<const Dir*>(node)) {
		return dir_color[node->selected ? 1 : 0];
	}
	return file_color[node->selected ? 1 : 0];
}
