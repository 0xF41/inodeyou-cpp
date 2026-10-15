#include "inode_utils.h"
#include "output.h"
#include "populate_fs_inodes.h"
#include "populate_tsk_inodes.h"
#include <sstream>
#include <stdexcept>
#include <iostream>

static void require(bool ok) { if (!ok) throw std::runtime_error("Test assertion failed"); }
static Inode find_path(const InodePaths &paths, const std::string &path) {
    for (const auto &entry : paths) if (entry.second.count(path)) return entry.first;
    throw std::runtime_error("Missing path: " + path);
}
int main(int argc, char **argv) {
    try {
        require(argc == 3);
        require(volume_path("/mnt/data/demo", "/mnt/data") == "/demo");
        require(volume_path("/mnt/data", "/mnt/data") == "/");
        bool rejected = false;
        try { volume_path("/mnt/database/demo", "/mnt/data"); } catch (...) { rejected = true; }
        require(rejected);
        Inode big = (Inode(1) << 40) + 5;
        InodePaths raw{{big, {"/a", "/hardlink"}}, {7, {"/raw-only"}}};
        InodePaths mounted{{big, {"/a"}}, {8, {"/mounted-only"}}};
        std::ostringstream out;
        require(print_differences(raw, mounted, out));
        for (const char *text : {"1099511627781", "NAME-ONLY IN RAW VIEW", "RAW-ONLY", "MOUNTED-ONLY", "/hardlink"})
            require(out.str().find(text) != std::string::npos);
        std::ostringstream clean;
        require(!print_differences(raw, raw, clean));
        require(clean.str().find('\033') == std::string::npos);
        std::ostringstream colored;
        print_differences(raw, mounted, colored, true);
        require(colored.str().find("\033[1;31m") != std::string::npos);
        require(colored.str().find("\033[1;33m") != std::string::npos);
        std::ostringstream green;
        print_differences(raw, raw, green, true);
        require(green.str().find("\033[1;32m") != std::string::npos);
        setenv("NO_COLOR", "1", 1);
        require(!use_color("auto", STDOUT_FILENO));
        require(use_color("always", STDOUT_FILENO));
        require(!use_color("never", STDOUT_FILENO));
        require(!use_color("auto", -1));
        require(safe_text("bad\033\n") == "bad\\x1b\\x0a");
        std::ostringstream escaped;
        print_differences(InodePaths{{1, {"/bad\033\n"}}}, {}, escaped);
        require(escaped.str().find("\\x1b\\x0a") != std::string::npos);
        InodePaths disk, live;
        std::string base(argv[2]);
        populate_tsk_inodes(argv[1], "/", base, 2, disk);
        populate_fs_inodes(base.c_str(), live);
        for (const auto *inventory : {&disk, &live}) {
            require(find_path(*inventory, base + "/nested/file") == find_path(*inventory, base + "/alias"));
            find_path(*inventory, base + "/link");
            find_path(*inventory, base + "/nested");
        }
        InodePaths subtree;
        populate_tsk_inodes(argv[1], "/nested", "/mnt/example/nested",
                            find_path(disk, base + "/nested"), subtree);
        require(subtree.size() == 1);
        find_path(subtree, "/mnt/example/nested/file");
        rejected = false;
        try { InodePaths bad; populate_tsk_inodes(argv[1], "/", base, 999999, bad); }
        catch (...) { rejected = true; }
        require(rejected);
        rejected = false;
        try { InodePaths bad; populate_tsk_inodes(argv[1], "/absent", base, 2, bad); }
        catch (...) { rejected = true; }
        require(rejected);
        std::cout << "Wide inodes, directional differences, hardlinks, escaping, path mapping, mounted walk and ext4 TSK walk passed.\n";
        return 0;
    } catch (const std::exception &e) { std::cerr << e.what() << '\n'; return 1; }
}
