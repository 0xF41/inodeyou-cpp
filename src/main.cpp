// SPDX-License-Identifier: GPL-3.0-only
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cerrno>
#include <cstring>
#include <string>
#include "tsk/populate_tsk_inodes.h"
#include "fs/populate_fs_inodes.h"
#include "utils/inode_utils.h"
#include "utils/output.h"
#include "utils/syscalls.h"
#include "utils/unique_fd.h"
#include "options.h"

static std::string canonical(const char *path)
{
    char *resolved = realpath(path, nullptr);
    if (!resolved)
        throw std::runtime_error(std::string("Cannot resolve path: ") + path);
    std::string result(resolved);
    free(resolved);
    return result;
}

static void sync_target(int fd, const std::string &path)
{
    if (retry_syncfs(fd) != 0)
        throw std::runtime_error("Cannot flush target filesystem: " + path + ": " +
                                 std::strerror(errno));
}

static TSK_FS_TYPE_ENUM filesystem_type(const std::string &name)
{
    if (name == "auto") return TSK_FS_TYPE_DETECT;
    if (name == "ext4") return TSK_FS_TYPE_EXT4;
    if (name == "ext3") return TSK_FS_TYPE_EXT3;
    if (name == "ext2") return TSK_FS_TYPE_EXT2;
    if (name == "ntfs") return TSK_FS_TYPE_NTFS;
    if (name == "fat") return TSK_FS_TYPE_FAT_DETECT;
    if (name == "exfat") return TSK_FS_TYPE_EXFAT;
    if (name == "hfs") return TSK_FS_TYPE_HFS_DETECT;
    if (name == "ufs") return TSK_FS_TYPE_FFS_DETECT;
    if (name == "yaffs2") return TSK_FS_TYPE_YAFFS2;
    if (name == "apfs") return TSK_FS_TYPE_APFS;
    throw std::runtime_error("Unsupported --fs-type: " + name);
}

int main(int argc, char *argv[])
{
    if (argc == 2 && std::string(argv[1]) == "--help") {
        std::cout << USAGE;
        return 0;
    }
    std::string color_mode = "auto";
    try {
        const Options options = parse_options(argc, argv);
        color_mode = options.color;
        if (geteuid() != 0)
            throw std::runtime_error("Run as root to read the volume");

        const auto volume = canonical(options.volume.c_str());
        const auto root = canonical(options.root.c_str());
        const auto start = canonical((options.have_start ? options.start : options.root).c_str());
        const auto inside = volume_path(start, root);

        struct stat root_stat, start_stat;
        if (stat(root.c_str(), &root_stat) || stat(start.c_str(), &start_stat))
            throw std::runtime_error("Cannot inspect mount root or starting directory");
        if (!S_ISDIR(root_stat.st_mode) || !S_ISDIR(start_stat.st_mode))
            throw std::runtime_error("Mount root and starting path must be directories");
        if (root_stat.st_dev != start_stat.st_dev)
            throw std::runtime_error("Starting directory is on a different device from --mount-root");

        UniqueFd volume_fd(retry_open(volume.c_str(), O_RDONLY | O_CLOEXEC | O_NOFOLLOW));
        if (volume_fd.get() < 0)
            throw std::runtime_error("Cannot open volume: " + volume);
        struct stat volume_stat;
        if (retry_fstat(volume_fd.get(), &volume_stat) != 0 || !S_ISBLK(volume_stat.st_mode) ||
            volume_stat.st_rdev != root_stat.st_dev)
            throw std::runtime_error("Use the block device backing the scan directory; specify --mount-root for non-root mounts");

        UniqueFd start_fd(retry_open(start.c_str(), O_RDONLY | O_DIRECTORY | O_CLOEXEC | O_NOFOLLOW));
        if (start_fd.get() < 0)
            throw std::runtime_error("Cannot open starting directory: " + start);
        struct stat opened;
        if (retry_fstat(start_fd.get(), &opened) != 0)
            throw std::runtime_error("Cannot inspect starting directory: " + start);
        if (!S_ISDIR(opened.st_mode) || opened.st_dev != start_stat.st_dev ||
            opened.st_ino != start_stat.st_ino)
            throw std::runtime_error("Starting directory identity changed: " + start);

        InodePaths raw, mounted;
        // Flush only the target filesystem; do not disturb unrelated mounts.
        sync_target(start_fd.get(), start);
        const auto fs_type = filesystem_type(options.fs_type);
        populate_tsk_inodes(volume_fd.get(), volume, inside, start,
                            static_cast<Inode>(start_stat.st_ino), raw, fs_type);
        populate_fs_inodes(start_fd.release(), start, start_stat.st_dev,
                           start_stat.st_ino, mounted);

        const bool color = use_color(color_mode, STDOUT_FILENO);
        std::cout << paint("[*] TSK inventory:", "36", color) << " "
                  << paint(std::to_string(raw.size()), "1;36", color) << " inodes.\n"
                  << paint("[*] Mounted inventory:", "36", color) << " "
                  << paint(std::to_string(mounted.size()), "1;36", color) << " inodes.\n";
        const bool different = print_differences(raw, mounted, std::cout, color);
        if (different)
            std::cout << paint("[*] Differences are candidates, not proof of malware; live filesystem changes can also cause them.", "2", color) << '\n';
        return different ? 1 : 0;
    } catch (const std::exception &error) {
        std::cerr << paint("[!] INCOMPLETE:", "1;33", use_color(color_mode, STDERR_FILENO))
                  << " " << safe_text(error.what()) << '\n';
        return 2;
    }
}
