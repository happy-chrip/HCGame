// Created by ABai on 2026/3/27.

#ifndef HCGAME_POSITION_H
#define HCGAME_POSITION_H

#include <Python.h>
typedef struct HC_Position {
    PyObject_HEAD
    PyObject * x, * y;
} HC_Position;

extern PyTypeObject HC_PositionType;

#endif //HCGAME_POSITION_H
