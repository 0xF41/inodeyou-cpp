#ifndef INODEYOU_OUTPUT_H
#define INODEYOU_OUTPUT_H
#include <cstdlib>
#include <string>
#include <unistd.h>

inline bool use_color(const std::string &mode, int fd)
{
    if (mode == "always") return true;
    if (mode == "never") return false;
    const char *term = std::getenv("TERM");
    return isatty(fd) && !std::getenv("NO_COLOR") &&
           (!term || std::string(term) != "dumb");
}

inline std::string paint(const std::string &text, const char *code, bool enabled)
{
    return enabled ? std::string("\033[") + code + "m" + text + "\033[0m" : text;
}

inline std::string safe_text(const std::string &text)
{
    const char *hex = "0123456789abcdef";
    std::string result;
    for (unsigned char c : text) {
        if (c < 32 || c >= 127) {
            result += "\\x";
            result += hex[c >> 4];
            result += hex[c & 15];
        } else result += c;
    }
    return result;
}
#endif
