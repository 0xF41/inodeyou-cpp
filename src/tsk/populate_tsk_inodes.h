// SPDX-License-Identifier: GPL-3.0-only
#ifndef POPULATE_TSK_INODES_H
#define POPULATE_TSK_INODES_H
#include "../utils/inode_utils.h"
#include <string>
#include <tsk/libtsk.h>

void populate_tsk_inodes(const char *volume, const std::string &inside,
                         const std::string &mounted, Inode expected_inode,
                         InodePaths &paths,
                         TSK_FS_TYPE_ENUM fs_type = TSK_FS_TYPE_DETECT);

// Takes a caller-owned descriptor and keeps it open for the duration of the call.
void populate_tsk_inodes(int fd, const std::string &volume_name,
                         const std::string &inside, const std::string &mounted,
                         Inode expected_inode, InodePaths &paths,
                         TSK_FS_TYPE_ENUM fs_type = TSK_FS_TYPE_DETECT);

#endif
