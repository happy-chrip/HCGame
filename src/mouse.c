// Created by ABai on 2026/4/5.

#include "mouse.h"
#include <SDL3/SDL.h>
#include <Python.h>
#include "log.h"
// 未完成，别动



PyObject * HC_Mouse_get_pos(PyObject *self) {
    float x, y;
    SDL_GetMouseState(&x, &y);
    return Py_BuildValue("(ff)", x, y);
}

PyObject * HC_Mouse_get_pressed(PyObject *self, PyObject *args, PyObject * kwds) {

}