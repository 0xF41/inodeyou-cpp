#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <unistd.h>
#include <sys/stat.h>
#include "tsk/populate_tsk_inodes.h"
#include "fs/populate_fs_inodes.h"
#include "utils/inode_utils.h"
#include "utils/output.h"

static std::string canonical(const char *path)
{
    char *resolved = realpath(path, nullptr);
    if (!resolved) throw std::runtime_error(std::string("Cannot resolve path: ") + path);
    std::string result(resolved);
    free(resolved);
    return result;
}

int main(int argc, char *argv[])
{
    if (argc == 2 && std::string(argv[1]) == "--help") {
        std::cout << "Usage: inodeyou <volume> [directory] [--mount-root <mountpoint>] [--color auto|always|never]\n"
                     "Exit: 0=no differences, 1=differences, 2=incomplete/error.\n";
        return 0;
    }
    std::string color_mode = "auto";
    try {
        if (argc < 2 || argc > 8) throw std::runtime_error("Usage: inodeyou <volume> [directory] [--mount-root <mountpoint>] [--color auto|always|never]");
        std::string start = "/", root = "/";
        bool have_start = false, have_root = false, have_color = false;
        for (int i = 2; i < argc; ++i) {
            if (std::string(argv[i]) == "--mount-root" && !have_root && i + 1 < argc) {
                root = argv[++i]; have_root = true;
            } else if (std::string(argv[i]) == "--color" && !have_color && i + 1 < argc) {
                color_mode = argv[++i]; have_color = true;
                if (color_mode != "auto" && color_mode != "always" && color_mode != "never")
                    throw std::runtime_error("--color must be auto, always, or never");
            } else if (!have_start && std::string(argv[i]).compare(0, 2, "--") != 0) {
                start = argv[i]; have_start = true;
            } else throw std::runtime_error("Invalid or duplicate argument");
        }
        if (geteuid() != 0) throw std::runtime_error("Run as root to read the volume");
        const auto volume = canonical(argv[1]);
        root = canonical(root.c_str());
        start = canonical((have_start ? start : root).c_str());
        const auto inside = volume_path(start, root);
        struct stat vol_stat, root_stat, start_stat;
        if (stat(volume.c_str(), &vol_stat) || stat(root.c_str(), &root_stat) || stat(start.c_str(), &start_stat))
            throw std::runtime_error("Cannot inspect volume/directory identity");
        if (!S_ISDIR(root_stat.st_mode) || !S_ISDIR(start_stat.st_mode))
            throw std::runtime_error("Mount root and starting path must be directories");
        if (!S_ISBLK(vol_stat.st_mode) || vol_stat.st_rdev != root_stat.st_dev || root_stat.st_dev != start_stat.st_dev)
            throw std::runtime_error("Use the block device backing the scan directory; specify --mount-root for non-root mounts");
        InodePaths raw, mounted;
        // Flush pending changes, but do not globally drop the machine's caches.
        sync();
        populate_tsk_inodes(volume.c_str(), inside, start, static_cast<Inode>(start_stat.st_ino), raw);
        populate_fs_inodes(start.c_str(), mounted);
        const bool color = use_color(color_mode, STDOUT_FILENO);
        std::cout << paint("[*] TSK inventory:", "36", color) << " "
                  << paint(std::to_string(raw.size()), "1;36", color) << " inodes.\n"
                  << paint("[*] Mounted inventory:", "36", color) << " "
                  << paint(std::to_string(mounted.size()), "1;36", color) << " inodes.\n";
        bool different = print_differences(raw, mounted, std::cout, color);
        if (different) std::cout << paint("[*] Differences are candidates, not proof of malware; live filesystem changes can also cause them.", "2", color) << '\n';
        return different ? 1 : 0;
    } catch (const std::exception &error) {
        std::cerr << paint("[!] INCOMPLETE:", "1;33", use_color(color_mode, STDERR_FILENO))
                  << " " << safe_text(error.what()) << '\n';
        return 2;
    }
}
