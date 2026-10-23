// SPDX-License-Identifier: GPL-3.0-only
#ifndef TSK_WALK_PATH_H
#define TSK_WALK_PATH_H
#include <tsk/libtsk.h>
#include "../utils/inode_utils.h"
void tsk_walk_path(TSK_FS_INFO *fs, TSK_INUM_T inode, const std::string &path,
                   InodePaths &paths, std::set<Inode> &ancestors, unsigned depth = 0);
#endif
