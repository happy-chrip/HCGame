// Created by ABai on 2026/3/24.

#include "display.h"
#include "log.h"
#include "HCGame.h"
#include "role.h"

static int HC_Display_init(HC_Display *self) {
    SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_EVENTS);
    if (!SDL_WasInit(SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
        PyErr_SetString(PyExc_RuntimeError, SDL_GetError());
        HC_debug_output("Fail to SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_EVENTS).\n");
        return 1;
    }
    return 0;
}

static void HC_Display_dealloc(HC_Display *self) {
    // 退出逻辑：释放内存、关闭文件、断开连接、清理资源等
    HC_debug_output("Deleting display...\n");
    if (self->renderer) {
        SDL_DestroyWindow(self->window);
        self->renderer = NULL;
    }
    if (self->window) {
        SDL_DestroyWindow(self->window);
        self->window = NULL;
    }
    SDL_Quit();
    PyObject_Free(self);
    HC_debug_output("Delete display.\n");
}

static PyObject *HC_Display_create_window(HC_Display *self, PyObject *args, PyObject *kwds) {
    int width = 800, height = 600;
    char *title = "Welcome to use HCGame";
    if (!PyArg_ParseTupleAndKeywords(
            args, kwds,
            "|iis",
            (char *[]) {"width", "height", "title", NULL},
            &width, &height, &title
    )) {
        return NULL;
    }

    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_CreateWindowAndRenderer(title, width, height, 0, &window, &renderer);
    if ((!window) || (!renderer)) {
        PyErr_SetString(PyExc_RuntimeError, SDL_GetError());
        HC_debug_output("Fail to SDL_CreateWindowAndRenderer.\n");
        return NULL;
    }
    self->window = window;
    self->renderer = renderer;
    Py_RETURN_TRUE;
}

static PyObject *HC_Display_blits_all(HC_Display *self) {
    if (role_list) {
        SDL_RenderClear(self->renderer);
        SDL_Texture *temp_texture = NULL;
        SDL_FRect fRect = {0, 0, 0, 0};

        for (int i = 0; i < PyList_Size(role_list); i++) {
            bool should_del_temp_texture = true;
            const HC_Role *role = (HC_Role *) PyList_GetItem(role_list, i);
            const Py_ssize_t role_costume_list_length = PyList_Size(role->costume_list);
            long role_costume_number = PyLong_AsLong(role->costume_number);
            if (role_costume_list_length == 0
                || role_costume_number >= role_costume_list_length
                || role_costume_number < 0) { continue; }

            SDL_Surface *now_surface_point = PyCapsule_GetPointer(
                PyList_GetItem(role->costume_list, role_costume_number),
                "SDL_Surface*"
            );
            if (role->buffer
                && role_costume_number < PyList_Size(role->buffer_list)) {
                // 启用了buffer（缓冲）
                PyObject *temp_capsule = PyList_GetItem(role->buffer_list, role_costume_number);
                if (temp_capsule != NULL && PyCapsule_CheckExact(temp_capsule)) {   // 如果存储了Texture
                    temp_texture = PyCapsule_GetPointer(temp_capsule, "SDL_Texture*");
                    should_del_temp_texture = false;    // 此时不需要销毁temp_texture
                } else {    // 没有存储Texture，从Surface转换一个（非常慢）
                    temp_texture = SDL_CreateTextureFromSurface(self->renderer, now_surface_point);
                }
            } else {    // 未启用buffer，转换一个Texture
                temp_texture = SDL_CreateTextureFromSurface(self->renderer, now_surface_point);
            }

            fRect.x = (float) PyFloat_AsDouble(role->position->x);
            fRect.y = (float) PyFloat_AsDouble(role->position->y);
            fRect.w = (float) (now_surface_point->w * PyFloat_AsDouble(role->scale) *
                               PyFloat_AsDouble(role->width_scale));;
            fRect.h = (float) (now_surface_point->h * PyFloat_AsDouble(role->scale) *
                               PyFloat_AsDouble(role->height_scale));
            SDL_FlipMode flipMode = SDL_FLIP_NONE;
            if (role->flip_x) { flipMode = flipMode | SDL_FLIP_HORIZONTAL; }
            if (role->flip_y) { flipMode = flipMode | SDL_FLIP_VERTICAL; }
            SDL_RenderTextureRotated(self->renderer, temp_texture, NULL, &fRect,
                                     PyFloat_AsDouble(role->facing_angle), NULL, flipMode);
            if (should_del_temp_texture) {
                SDL_DestroyTexture(temp_texture);
            }
            temp_texture = NULL;
        }
        Py_RETURN_TRUE;
    }
    return NULL;
}

static PyObject *HC_Display_update(HC_Display *self) {
    if (SDL_RenderPresent(self->renderer)) {
        Py_RETURN_TRUE;
    }
    return NULL;
}

static PyMethodDef HC_DisplayMethodDef[] = {
        {"update",        (PyCFunction) HC_Display_update,        METH_NOARGS, "Update screen"},
        {"blits_all",     (PyCFunction) HC_Display_blits_all,     METH_NOARGS, "blit all to screen"},
        {"create_window", (PyCFunction) HC_Display_create_window, METH_VARARGS |
                                                                  METH_KEYWORDS, "create window and renderer"},
        {NULL, NULL, 0,                                                          NULL}
};

void HC_Display_get_window_size(int *width, int *height) {
    SDL_GetWindowSize(display->window, width, height);
}

PyTypeObject HC_DisplayType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        .tp_name = "HCGame.__Display",
        .tp_basicsize = sizeof(HC_Display),
        .tp_itemsize = 0,
        .tp_flags = Py_TPFLAGS_DEFAULT,
        .tp_new = PyType_GenericNew,
        .tp_init = (initproc) HC_Display_init,
        .tp_dealloc = (destructor) HC_Display_dealloc,
        .tp_doc = "CDisplay",
        .tp_methods = HC_DisplayMethodDef,
};