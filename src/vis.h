#ifndef VIS_H_
#define VIS_H_

#include "fstree.h"

void draw_env();
void draw_node(const FSNode *node);
void draw_node_text(const FSNode *node);
void draw_link(const Link *link);
void draw_file_stats(const File *file);
void draw_file_stats(const File *file, float mx, float my);

#endif	// VIS_H_
