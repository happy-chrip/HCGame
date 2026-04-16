// Created by ABai on 2026/3/24.
#include <string.h>

const char * HC_Path_get_file_extName(const char *path) {
    const char *dot = strrchr(path, '.');    // 找最后一个点
    const char *sep = strrchr(path, '/');    // 找最后一个路径分隔符(unix)
    if (!sep) sep = strrchr(path, '\\');     // 兼容Windows路径

    // 点必须在文件名中才有效
    if (dot && (!sep || dot > sep)) {
        return dot + 1;
    }
    return "\0";
}