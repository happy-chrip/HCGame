// Created by ABai on 2026/3/26.

#ifndef HCGAME_SURFACE_H
#define HCGAME_SURFACE_H

#include <Python.h>
#include <SDL3/SDL.h>
#include "role.h"

typedef struct {
    PyObject_HEAD
    SDL_Surface *surface;
} HC_Surface;

PyObject *HC_Surface_load_image_as_surface(PyObject *self, PyObject *args, PyObject *kwds);

SDL_Surface *HC_Surface_get_surface_from_role(HC_Role *role);

extern PyTypeObject HC_SurfaceType;

#endif //HCGAME_SURFACE_H
