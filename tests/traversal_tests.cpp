// SPDX-License-Identifier: GPL-3.0-only
#include "fs/populate_fs_inodes.h"
#include "utils/inode_utils.h"
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <sys/stat.h>
#include <unistd.h>

namespace {

bool has_path(const InodePaths &paths, const std::string &wanted)
{
    for (const auto &item : paths)
        if (item.second.count(wanted))
            return true;
    return false;
}

bool has_prefix(const InodePaths &paths, const std::string &prefix)
{
    for (const auto &item : paths)
        for (const auto &path : item.second)
            if (path.compare(0, prefix.size(), prefix) == 0)
                return true;
    return false;
}

void remove_file(const std::string &path)
{
    unlink(path.c_str());
}

} // namespace

int main()
{
    char template_name[] = "/tmp/inodeyou-traversal-XXXXXX";
    char *created = mkdtemp(template_name);
    if (!created) {
        std::cerr << "mkdtemp failed\n";
        return 1;
    }
    const std::string base(created);
    const std::string directory = base + "/dir";
    const std::string file = directory + "/file";
    const std::string outside = base + "/outside_link";
    const std::string inside_link = base + "/dir_link";
    const std::string loop = base + "/loop";

    int status = 1;
    try {
        if (mkdir(directory.c_str(), 0700) != 0)
            throw std::runtime_error("mkdir failed");
        std::ofstream output(file.c_str());
        if (!output)
            throw std::runtime_error("open fixture failed");
        output << "fixture\n";
        if (symlink("/etc", outside.c_str()) != 0 ||
            symlink(directory.c_str(), inside_link.c_str()) != 0 ||
            symlink(".", loop.c_str()) != 0)
            throw std::runtime_error("symlink failed");

        InodePaths paths;
        populate_fs_inodes(base.c_str(), paths);

        if (!has_path(paths, directory) || !has_path(paths, file) ||
            !has_path(paths, outside) || !has_path(paths, inside_link) ||
            !has_path(paths, loop))
            throw std::runtime_error("expected entry missing");
        if (has_prefix(paths, outside + "/") ||
            has_prefix(paths, inside_link + "/") ||
            has_prefix(paths, loop + "/"))
            throw std::runtime_error("symlink was followed");
        status = 0;
    } catch (const std::exception &error) {
        std::cerr << error.what() << '\n';
    }

    remove_file(loop);
    remove_file(inside_link);
    remove_file(outside);
    remove_file(file);
    rmdir(directory.c_str());
    rmdir(base.c_str());
    return status;
}
