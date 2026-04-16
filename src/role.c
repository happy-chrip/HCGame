// Created by ABai on 2026/3/24.

#define max(a, b) ((a > b) ? a : b)
#define min(a, b) ((a < b) ? a : b)
#include "role.h"
#include "position.h"
#include "log.h"
#include <SDL3/SDL.h>
#include "surface.h"
#include "HCGame.h"
#include <stdlib.h>
#include <structmember.h>

static unsigned int now_role_count = 0;

static int HC_Role_init(HC_Role *self) {
    self->alpha = PyFloat_FromDouble(1.0);
    self->scale = PyFloat_FromDouble(1.0);
    self->height_scale = PyFloat_FromDouble(1.0);
    self->width_scale = PyFloat_FromDouble(1.0);
    self->costume_list = PyList_New(0);
    self->costume_number = PyLong_FromLong(-1);
    self->buffer_list = PyList_New(0);
    self->buffer = false;
    self->flip_x = false;
    self->flip_y = false;
    self->facing_angle = PyFloat_FromDouble(0);

    // 初始化坐标参数
    {
        // 此处减少作用域污染
        PyObject *args = PyTuple_New(0);
        PyObject *temp = HC_PositionType.tp_new(&HC_PositionType, NULL, NULL);
        if (!temp) {
            Py_DECREF(temp);
            Py_DECREF(args);
            return -1;
        }
        if (HC_PositionType.tp_init(temp, args, NULL) != 0) {
            Py_DECREF(args);
            return -1;
        }
        self->position = (HC_Position *) temp;
        Py_DECREF(args);
    }

    PyList_Append(role_list, (PyObject *) self);

    self->id = now_role_count;
    HC_debug_output("Init a role (ID: %d).\n", self->id);
    now_role_count += 1;
    return 0;
}

static void HC_Role_dealloc(HC_Role *self) {
    HC_debug_output("Deleting a role (ID: %d)...\n", self->id);
    if (self->costume_list) {
        PyObject *temp;
        for (int i = 0; i < PyList_Size(self->costume_list); i++) {
            temp = PyList_GetItem(self->costume_list, i);
            if (temp != NULL && PyCapsule_CheckExact(temp)) {
                SDL_DestroySurface(PyCapsule_GetPointer(temp, "SDL_Surface*"));
            }
        }
    }
    if (self->buffer_list) {
        PyObject *temp;
        for (int i = 0; i < PyList_Size(self->buffer_list); i++) {
            temp = PyList_GetItem(self->buffer_list, i);
            if (!PyLong_Check(temp)) {
                SDL_Texture *temp_texture = PyCapsule_GetPointer(temp, "SDL_Texture*");
                SDL_DestroyTexture(temp_texture);
            }
        }
    }
    Py_XDECREF(self->alpha);
    Py_XDECREF(self->scale);
    Py_XDECREF(self->width_scale);
    Py_XDECREF(self->height_scale);
    Py_XDECREF(self->costume_number);
    Py_XDECREF(self->facing_angle);

    Py_CLEAR(self->costume_list);
    Py_CLEAR(self->buffer_list);

    Py_XDECREF(self->position);

    if (!HCGame_is_exiting && (role_list != NULL)) {
        // 如果是模块级退出，就不需要手动删除role_list中的引用了
        Py_ssize_t index = PySequence_Index(role_list, (PyObject *) self);
        if (index >= 0) {
            PySequence_DelItem(role_list, index);
        }
    }
    HC_debug_output("Delete a role (ID: %d).\n", self->id);
    PyObject_Free(self);
}

static PyObject *HC_Role_load_image(HC_Role *self, PyObject *args, PyObject *kwds) {
    char *path;
    if (!PyArg_ParseTupleAndKeywords(
        args, kwds,
        "s",
        (char *[]){"path", NULL},
        &path
    )) {
        return NULL;
    }
    SDL_Surface *surface = SDL_LoadSurface(path);
    if (!surface) {
        HC_debug_output("Fail to SDL_LoadSurface at Role.load_image: %s\n", SDL_GetError());
        return NULL;
    }

    if (surface->format != SDL_PIXELFORMAT_RGBA8888) {
        SDL_Surface *temp = SDL_ConvertSurface(surface, SDL_PIXELFORMAT_RGBA8888);
        SDL_DestroySurface(surface);
        if (!temp) {
            HC_debug_output("Fail to SDL_ConvertSurface at Role.load_image: %s\n", SDL_GetError());

            return NULL;
        }
        surface = temp;
    }

    PyList_Append(self->costume_list, PyCapsule_New(surface, "SDL_Surface*", NULL));

    if (PyLong_AsLong(self->costume_number) == -1) {
        PyObject *one = PyLong_FromLong(1);
        PyObject *result = PyNumber_Add(self->costume_number, one);
        if (!result) {
            PyErr_SetString(PyExc_RuntimeError, "Fail to add numbers at Role.load_image");
            Py_XDECREF(one);
            Py_XDECREF(result);
            Py_RETURN_FALSE;
        }
        Py_XDECREF(self->costume_number);
        self->costume_number = result;
        Py_XDECREF(one);
        // result所有权交给了self->costume_number，不用再Py_XDECREF(result);了（那会导致self->costume_number变为野指针）
    }

    Py_RETURN_TRUE;
}

static PyObject *HC_Role_load_image_from_surface(HC_Role *self, PyObject *args, PyObject *kwds) {
    PyObject *py_object;
    if (!PyArg_ParseTupleAndKeywords(
        args, kwds,
        "O",
        (char *[]){"surface", NULL},
        &py_object
    )) {
        return NULL;
    }
    if (!PyObject_TypeCheck(py_object, &HC_SurfaceType)) {
        PyErr_SetString(PyExc_TypeError, "Value of Role.load_image_from_surface must be a HC_Surface.");
        return NULL;
    }
    HC_Surface *hc_surface = (HC_Surface *) py_object;
    PyList_Append(self->costume_list, PyCapsule_New(hc_surface->surface, "SDL_Surface*", NULL));

    if (PyLong_AsLong(self->costume_number) == -1) {
        PyObject *one = PyLong_FromLong(1);
        PyObject *result = PyNumber_Add(self->costume_number, one);
        if (!result) {
            PyErr_SetString(PyExc_RuntimeError, "Fail to add numbers at Role.load_image");
            Py_XDECREF(one);
            Py_XDECREF(result);
            Py_RETURN_FALSE;
        }
        Py_XDECREF(self->costume_number);
        self->costume_number = result;
        Py_XDECREF(one);
        // result所有权交给了self->costume_number，不用再Py_XDECREF(result);了（那会导致self->costume_number变为野指针）
    }

    Py_RETURN_TRUE;
}

static PyObject *HC_Role_collide(HC_Role *self, PyObject *args, PyObject *kwds) {
    if (PyList_Size(self->costume_list) <= 0) {
        HC_debug_output("Role of collide should has costume.\n");
        Py_RETURN_FALSE;
    }
    PyObject *temp;
    if (!PyArg_ParseTupleAndKeywords(
        args, kwds,
        "O",
        (char *[]){"target_role", NULL},
        &temp
    )) {
        return NULL;
    }
    if (!PyObject_TypeCheck(temp, &HC_RoleType)) {
        HC_debug_output("Collide target must be a role!");
        return NULL;
    }
    HC_Role *other = (HC_Role *) temp;
    if (PyList_Size(other->costume_list) <= 0) {
        HC_debug_output("Another Role of collide should has costume.\n");
        Py_RETURN_FALSE;
    }
    if (other->id == self->id) {
        HC_debug_output("Collide target should be another role.");
        Py_RETURN_TRUE;
    }

    SDL_Surface *a = HC_Surface_get_surface_from_role(self);
    SDL_Surface *b = HC_Surface_get_surface_from_role(other);
    double x1 = PyFloat_AsDouble(self->position->x);
    double y1 = PyFloat_AsDouble(self->position->y);
    double x2 = PyFloat_AsDouble(other->position->x);
    double y2 = PyFloat_AsDouble(other->position->y);

    // 边界检查
    if ((x1 < x2 && x1 + (double)a->w < x2) || (x1 > x2 && x1 > x2 + (double)b->w)
        || (y1 > y2 && y1 > y2 + (double)b->h) || (y1 < y2 && y1 + (double)a->h < y2)) {
        SDL_DestroySurface(a);
        SDL_DestroySurface(b);
        Py_RETURN_FALSE;
    }


    HC_debug_output("colliding...\n");
    const int left1 = max(0, (int)(x2 - x1)); // 每过一行左侧要加上的字节数
    const int left2 = max(0, (int)(x1 - x2));
    const int width = min(x1+a->w, x2+b->w) - max(x1, x2);

    const char * start1 = (char *) a->pixels + max(0, (int)(y2 - y1)) * a->pitch + left1 * 4;
    const char * start2 = (char *) b->pixels + max(0, (int)(y1 - y2)) * b->pitch + left2 * 4;
    const int height = min(y1 + a->h, y2 + b->h) - max(y1, y2);

    bool result = false;
    Uint32 pixel_a = 0x00000000;
    Uint32 pixel_b = 0x00000000;
    for (int line = 0; line < height; line++) {
        for (int i = 0; i < width; i++) {
            memcpy(&pixel_a,
                   start1 + i * 4,
                   4);
            memcpy(&pixel_b,
                   start2 + i * 4,
                   4);
            // 提取透明通道
            pixel_a = pixel_a & 0x000000ff;
            pixel_b = pixel_b & 0x000000ff;
            if (pixel_a > 0 && pixel_b > 0) {
                result = true;
                goto finish;
            }
        }
        start1 += a->pitch;
        start2 += b->pitch;
    }

finish:
    HC_debug_output("collide.\n");
    SDL_DestroySurface(a);
    SDL_DestroySurface(b);
    if (result) {
        Py_RETURN_TRUE;
    }
    Py_RETURN_FALSE;
}

static PyObject *HC_Role_buffer(HC_Role *self, PyObject *args, PyObject *kwds) {
    if (self->buffer) {
        PyList_SetSlice(self->buffer_list, 0, PyList_Size(self->buffer_list), NULL);
        self->buffer = false;
    }
    PyObject *temp;
    for (int i = 0; i < PyList_Size(self->costume_list); i++) {
        temp = PyList_GetItem(self->costume_list, i);
        if (temp != NULL && PyCapsule_CheckExact(temp)) {
            SDL_Texture *temp_texture = SDL_CreateTextureFromSurface(display->renderer,
                                                                     PyCapsule_GetPointer(temp, "SDL_Surface*"));
            PyList_Append(self->buffer_list, PyCapsule_New(temp_texture, "SDL_Texture*", NULL));
        } else {
            PyList_Append(self->buffer_list, PyLong_FromLong(0));
        }
    }
    self->buffer = true;
    Py_RETURN_NONE;
}

static PyObject *HC_Role_get_flip_x(HC_Role *self, void *closure) {
    if (self->flip_x) {
        Py_RETURN_TRUE;
    } else {
        Py_RETURN_FALSE;
    }
}

static int HC_Role_set_flip_x(HC_Role *self, PyObject *new_value, void *closure) {
    if (!PyBool_Check(new_value)) {
        PyErr_SetString(PyExc_TypeError, "Role.flip_x value must be an bool.");
        return -1;
    }
    if (Py_IsTrue(new_value)) {
        self->flip_x = true;
    } else {
        self->flip_x = false;
    }
    return 0;
}

static PyObject *HC_Role_get_flip_y(HC_Role *self, void *closure) {
    if (self->flip_y) {
        Py_RETURN_TRUE;
    } else {
        Py_RETURN_FALSE;
    }
}

static int HC_Role_set_flip_y(HC_Role *self, PyObject *new_value, void *closure) {
    if (!PyBool_Check(new_value)) {
        PyErr_SetString(PyExc_TypeError, "Role.flip_y value must be an bool.");
        return -1;
    }
    if (Py_IsTrue(new_value)) {
        self->flip_y = true;
    } else {
        self->flip_y = false;
    }
    return 0;
}

static PyObject *HC_Role_get_facing_angle(HC_Role *self, void *closure) {
    return Py_NewRef(self->facing_angle);
}

static int HC_Role_set_facing_angle(HC_Role *self, PyObject *new_value, void *closure) {
    if (PyFloat_Check(new_value) || PyLong_Check(new_value)) {
        Py_XDECREF(self->facing_angle);
        self->facing_angle = PyNumber_Float(new_value);
        return 0;
    }
    PyErr_SetString(PyExc_TypeError, "Role.facing_angle value must be a float or a int.");
    return -1;
}

static PyObject *HC_Role_get_position(HC_Role *self, void *closure) {
    return Py_NewRef(self->position);
}

static int HC_Role_set_position(HC_Role *self, PyObject *new_value, void *closure) {
    if (PyObject_TypeCheck(new_value, &HC_PositionType)) {
        Py_XDECREF(self->position);
        self->position = (HC_Position *) Py_NewRef(new_value);
        return 0;
    }
    PyErr_SetString(PyExc_TypeError, "Role.position value must be a Position.");
    return -1;
}

static PyObject *HC_Role_get_costume_number(HC_Role *self, void *closure) {
    return Py_NewRef(self->costume_number);
}

// 未写完
static int HC_Role_set_costume_number(HC_Role *self, PyObject *new_value, void *closure) {
    if (!PyBool_Check(new_value)) {
        PyErr_SetString(PyExc_TypeError, "Role.flip_y value must be an bool.");
        return -1;
    }
    if (Py_IsTrue(new_value)) {
        self->flip_y = true;
    } else {
        self->flip_y = false;
    }
    return 0;
}

static PyGetSetDef HC_Role_getSetDef[] = {
    {"flip_x", (getter) HC_Role_get_flip_x, (setter) HC_Role_set_flip_x, "set or get Role.flip_x."},
    {"flip_y", (getter) HC_Role_get_flip_y, (setter) HC_Role_set_flip_y, "set or get Role.flip_y."},
    {
        "facing_angle", (getter) HC_Role_get_facing_angle, (setter) HC_Role_set_facing_angle,
        "set or get Role.facing_angle."
    },
    {"position", (getter) HC_Role_get_position, (setter) HC_Role_set_position, "set or get Role.position."},
    {NULL}
};

static PyMemberDef HC_RoleMembers[] = {
    {"costume_list", T_OBJECT, offsetof(HC_Role, costume_list), READONLY, "costume_list of CRole"},
    {"costume_number", T_OBJECT, offsetof(HC_Role, costume_number), READONLY, "costume_number of CRole"},
    {"alpha", T_OBJECT, offsetof(HC_Role, alpha), 0, "alpha of CRole"},
    {"scale", T_OBJECT, offsetof(HC_Role, scale), 0, "scale of CRole"},
    {"width_scale", T_OBJECT, offsetof(HC_Role, width_scale), 0, "width_scale of CRole"},
    {"height_scale", T_OBJECT, offsetof(HC_Role, height_scale), 0, "height_scale of CRole"},
    {NULL, 0, 0, 0, NULL}
};


static PyMethodDef HC_RoleMethodDef[] = {
    {
        "load_image", (PyCFunction) HC_Role_load_image,
        METH_KEYWORDS |
        METH_VARARGS,
        "Load image from path."
    },
    {
        "load_image_from_surface", (PyCFunction) HC_Role_load_image_from_surface, METH_KEYWORDS |
            METH_VARARGS,
        "Load image from a surface."
    },
    {"buffer", (PyCFunction) HC_Role_buffer, METH_NOARGS, "buffer all surface."},
    {
        "collide", (PyCFunction) HC_Role_collide, METH_KEYWORDS |
                                                  METH_VARARGS,
        "collide with another role."
    },
    {NULL, NULL, 0, NULL}
};
PyTypeObject HC_RoleType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "HCGame.Role",
    .tp_basicsize = sizeof(HC_Role),
    .tp_itemsize = 0,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_new = PyType_GenericNew, // 创建对象（分配内存），我这里填默认行为，通常不用动
    .tp_init = (initproc) HC_Role_init, // 初始化 __init__
    .tp_dealloc = (destructor) HC_Role_dealloc, // 销毁 __del__，无循环引用不需要GC介入
    .tp_doc = "CRole",
    .tp_members = HC_RoleMembers,
    .tp_methods = HC_RoleMethodDef,
    .tp_getset = HC_Role_getSetDef
};
