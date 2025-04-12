#ifndef TSK_WALK_PATH_H
#define TSK_WALK_PATH_H

#include <unordered_set>
#include <tsk/libtsk.h>

// Function prototypes
void tsk_walk_path(TSK_FS_INFO *fs, TSK_INUM_T dir_inode_num, std::unordered_set<int> &tsk_inode_set);

#endif // TSK_WALK_PATH_H