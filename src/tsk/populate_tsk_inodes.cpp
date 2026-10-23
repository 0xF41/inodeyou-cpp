// SPDX-License-Identifier: GPL-3.0-only
#include "populate_tsk_inodes.h"
#include "tsk_walk_path.h"
#include "../utils/syscalls.h"
#include "../utils/tsk_error.h"
#include "../utils/unique_fd.h"
#include <fcntl.h>
#include <memory>
#include <stdexcept>
#include <vector>

namespace {

void populate_tsk_fd(int fd, const std::string &volume_name,
                     const std::string &inside, const std::string &mounted,
                     Inode expected_inode, InodePaths &paths,
                     TSK_FS_TYPE_ENUM fs_type)
{
    if (fd < 0)
        throw std::runtime_error("TSK volume descriptor is closed");
    const std::string image = "/proc/self/fd/" + std::to_string(fd);
    std::unique_ptr<TSK_IMG_INFO, decltype(&tsk_img_close)> img(
        tsk_img_open_utf8_sing(image.c_str(), TSK_IMG_TYPE_RAW, 0), tsk_img_close);
    if (!img)
        throw std::runtime_error("TSK could not open volume " + volume_name + ": " +
                                 tsk_error_message());
    std::unique_ptr<TSK_FS_INFO, decltype(&tsk_fs_close)> fs(
        tsk_fs_open_img(img.get(), 0, fs_type), tsk_fs_close);
    if (!fs)
        throw std::runtime_error("TSK could not open filesystem at volume offset zero: " +
                                 tsk_error_message());
    TSK_INUM_T inode = 0;
    std::vector<char> lookup(inside.begin(), inside.end());
    lookup.push_back('\0');
    if (tsk_fs_ifind_path(fs.get(), lookup.data(), &inode) != 0)
        throw std::runtime_error("TSK could not resolve starting directory inside volume: " +
                                 inside + ": " + tsk_error_message());
    if (static_cast<Inode>(inode) != expected_inode)
        throw std::runtime_error("Raw/mounted starting inode mismatch; check volume and --mount-root");
    std::set<Inode> ancestors;
    InventoryBudget budget;
    tsk_walk_path(fs.get(), inode, mounted, paths, ancestors, budget);
}

} // namespace

void populate_tsk_inodes(const char *volume, const std::string &inside,
                         const std::string &mounted, Inode expected_inode,
                         InodePaths &paths, TSK_FS_TYPE_ENUM fs_type)
{
    UniqueFd fd(retry_open(volume, O_RDONLY | O_CLOEXEC | O_NOFOLLOW));
    if (fd.get() < 0)
        throw std::runtime_error(std::string("Cannot open volume: ") + volume);
    populate_tsk_fd(fd.get(), volume, inside, mounted, expected_inode, paths, fs_type);
}

void populate_tsk_inodes(int fd, const std::string &volume_name,
                         const std::string &inside, const std::string &mounted,
                         Inode expected_inode, InodePaths &paths,
                         TSK_FS_TYPE_ENUM fs_type)
{
    populate_tsk_fd(fd, volume_name, inside, mounted, expected_inode, paths, fs_type);
}
