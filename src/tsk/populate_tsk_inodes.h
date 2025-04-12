#ifndef POPULATE_TSK_INODES_H
#define POPULATE_TSK_INODES_H

#include <unordered_set>
#include <tsk/libtsk.h>

void populate_tsk_inodes(const char *vol, const char *dir, std::unordered_set<int> &tsk_inode_set);

#endif // POPULATE_TSK_INODES_H