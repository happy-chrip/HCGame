// Created by ABai on 2026/4/5.

#include "mouse.h"

#include <Python.h>
#include <SDL3/SDL.h>

#include "log.h"

PyObject* HC_Mouse_mouse_get_pos(PyObject* self) {
    float x, y;
    SDL_GetMouseState(&x, &y);
    return Py_BuildValue("(ff)", x, y);
}

PyObject* HC_Mouse_mouse_get_pressed(PyObject* self, PyObject* args, PyObject* kwds) {
    int check_target;
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "|i", (char*[]){"x", NULL},
                                     &check_target)) {
        return -1;
    }
    bool result = true;
    SDL_MouseButtonFlags mouse_state = SDL_GetMouseState(NULL, NULL);
    if (check_target & 0b100) {
        result = result && SDL_BUTTON_MASK(SDL_BUTTON_LEFT);
    }
    if (check_target & 0b010) {
        result = result && SDL_BUTTON_MASK(SDL_BUTTON_MIDDLE);
    }
    if (check_target & 0b001) {
        result = result && SDL_BUTTON_MASK(SDL_BUTTON_RIGHT);
    }
    if (result) {
        Py_RETURN_TRUE;
    }
    Py_RETURN_FALSE;
}