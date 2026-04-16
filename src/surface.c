// Created by ABai on 2026/3/26.

#include "surface.h"
#include "log.h"
#include "display.h"

static int HC_Surface_init(HC_Surface *self, PyObject *args, PyObject *kwds) {
    HC_debug_output("Initializing a surface.\n");
    int width, height;
    if (!PyArg_ParseTupleAndKeywords(
            args, kwds,
            "ii",
            (char *[]) {"width", "height", NULL},
            &width, &height
    )) {
        return -1;
    }
    SDL_Surface *surface = SDL_CreateSurface(width, height, SDL_PIXELFORMAT_RGBA8888);
    if (!surface) {
        HC_debug_output("Fail to create a SDL_Surface(w: %d h: %d).%s\n", width, height, SDL_GetError());
        return -1;
    }
    self->surface = surface;
    HC_debug_output("Init a surface(w: %d h: %d).\n", width, height);
    return 0;
}

static void HC_Surface_dealloc(HC_Surface *self) {
    HC_debug_output("Deleting a surface.\n");
    if (self->surface) {
        SDL_DestroySurface(self->surface);
    }
    Py_TYPE(self)->tp_free((PyObject *) self);
    HC_debug_output("Delete a surface.\n");
}

PyTypeObject HC_SurfaceType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        .tp_name = "HCGame.__Surface",
        .tp_basicsize = sizeof(HC_Surface),
        .tp_itemsize = 0,
        .tp_flags = Py_TPFLAGS_DEFAULT,
        .tp_new = PyType_GenericNew,      // 创建对象（分配内存），我这里填默认行为，通常不用动
        .tp_init = (initproc) HC_Surface_init, // 初始化 __init__
        .tp_dealloc = (destructor) HC_Surface_dealloc, // 销毁 __del__，无循环引用不需要GC介入
        .tp_doc = "CSurface",
};

PyObject *HC_Surface_load_image_as_surface(PyObject *self, PyObject *args, PyObject *kwds) {
    char *path;
    if (!PyArg_ParseTupleAndKeywords(
            args, kwds,
            "s",
            (char *[]) {"path", NULL},
            &path
    )) {
        return NULL;
    }
    SDL_Surface *surface = SDL_LoadSurface(path);
    if (!surface) {
        HC_debug_output("Fail to SDL_LoadSurface at load_image_as_surface: %s\n", SDL_GetError());
        return NULL;
    }

    if (surface->format != SDL_PIXELFORMAT_RGBA8888) {
        SDL_Surface *temp = SDL_ConvertSurface(surface, SDL_PIXELFORMAT_RGBA8888);
        SDL_DestroySurface(surface);
        if (!temp) {
            HC_debug_output("Fail to SDL_ConvertSurface at load_image_as_surface: %s\n", SDL_GetError());
            return NULL;
        }
        surface = temp;
    }

    HC_Surface *result;
    result = (HC_Surface *) HC_SurfaceType.tp_new(&HC_SurfaceType, NULL, NULL);
    {
        int window_width = 0, window_height = 0;
        HC_Display_get_window_size(&window_width, &window_height);
        PyObject *args2 = PyTuple_Pack(2, PyLong_FromLong(window_width), PyLong_FromLong(window_height));
        HC_SurfaceType.tp_init((PyObject *) result, args2, NULL);
        Py_XDECREF(args2);
    }
    SDL_DestroySurface(result->surface);
    result->surface = surface;
    return (PyObject *) result;
}


SDL_Surface* HC_Surface_get_surface_from_role(HC_Role * role) {
    if (PyList_Size(role->costume_list) <= 0) {
        HC_debug_output("Fail to get_surface_from_role: role must has costume");
        return NULL;
    }
    void * a = PyCapsule_GetPointer(PyList_GetItem(role->costume_list, PyLong_AsLong(role->costume_number)), "SDL_Surface*");
    SDL_Surface * result = SDL_DuplicateSurface(a);

    SDL_ScaleSurface(result,
                     (int)(PyFloat_AsDouble(role->scale) * result->w * PyFloat_AsDouble(role->width_scale)),
                     (int)(PyFloat_AsDouble(role->scale) * result->h * PyFloat_AsDouble(role->height_scale)),
                     SDL_SCALEMODE_PIXELART);
    SDL_FlipMode flipMode = SDL_FLIP_NONE;
    if (role->flip_x) { flipMode = flipMode | SDL_FLIP_HORIZONTAL; }
    if (role->flip_y) { flipMode = flipMode | SDL_FLIP_VERTICAL; }
    SDL_FlipSurface(result, flipMode);
    SDL_RotateSurface(result, (float)PyFloat_AsDouble(role->facing_angle));
    SDL_SetSurfaceAlphaMod(result, (int)(PyFloat_AsDouble(role->alpha) * 255));
    return result;
}