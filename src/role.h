// Created by ABai on 2026/3/24.

#ifndef HCGAME_ROLE_H
#define HCGAME_ROLE_H

#include <Python.h>
#include <stdbool.h>
#include "position.h"

typedef struct {
    PyObject_HEAD  // doc为“这是一个在声明代表无可变长度对象的新类型时所使用的宏”
    PyObject *scale, *width_scale, *height_scale, *alpha, *facing_angle; // 浮点数类型数据（Python），其实写一行也行
    PyObject *costume_list;    // 造型列表（Python）
    PyObject *costume_number;  // 当前造型（Python）
    PyObject *buffer_list;
    struct HC_Position * position;
    bool buffer;
    bool flip_x, flip_y;
    unsigned int id;
} HC_Role;

extern PyTypeObject HC_RoleType;

#endif // HCGAME_ROLE_H
