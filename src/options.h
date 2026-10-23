// SPDX-License-Identifier: GPL-3.0-only
#ifndef INODEYOU_OPTIONS_H
#define INODEYOU_OPTIONS_H
#include <string>

extern const char *const USAGE;

struct Options {
    std::string volume;
    std::string start = "/";
    std::string root = "/";
    std::string color = "auto";
    std::string fs_type = "auto";
    bool have_start = false;
};

Options parse_options(int argc, char *argv[]);

#endif
