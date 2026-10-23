// SPDX-License-Identifier: GPL-3.0-only
#ifndef POPULATE_TSK_INODES_H
#define POPULATE_TSK_INODES_H
#include "../utils/inode_utils.h"
void populate_tsk_inodes(const char *volume, const std::string &inside,
                         const std::string &mounted, Inode expected_inode, InodePaths &paths);
#endif
