// Created by ABai on 2026/3/27.

#include "position.h"
#include "log.h"

static PyObject *HC_Position_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
    HC_debug_output("Creating a HC_Position...\n");
    HC_Position *self = (HC_Position*)PyObject_New(HC_Position, &HC_PositionType);
    if (self != NULL) {
        HC_debug_output("Create a HC_Position.\n"); // 添加调试输出
        // 初始化指针成员为NULL是个好习惯，但这是静态初始化
        self->x = PyFloat_FromDouble(0);
        self->y = PyFloat_FromDouble(0);
    }
    return (PyObject*)self;
}

static int HC_Position_init(HC_Position *self, PyObject *args, PyObject *kwds) {
    PyObject *x = PyFloat_FromDouble(0), *y = PyFloat_FromDouble(0);
    if (!PyArg_ParseTupleAndKeywords(
            args, kwds,
            "|OO",
            (char *[]) {"x", "y", NULL},
            &x, &y
    )) {
        return -1;
    }
    if (PyLong_Check(x) || PyFloat_Check(x)) {
        self->x = PyNumber_Float(x);
    } else { goto position_value_error; }

    if (PyLong_Check(y) || PyFloat_Check(y)) {
        self->y = PyNumber_Float(y);
    } else {
        goto position_value_error;
    }

    HC_debug_output("Init a position(x=%.2f y=%.2f).\n", PyFloat_AsDouble(self->x), PyFloat_AsDouble(self->y));
    return 0;

    // 发生错误
    position_value_error:
    Py_XDECREF(self->x);
    Py_XDECREF(self->y);
    PyErr_SetString(PyExc_TypeError, "Value of Position must be int or float.");
    return -1;
}

static void HC_Position_dealloc(HC_Position *self) {
    HC_debug_output("deleting a position...\n");
    Py_XDECREF(self->x);
    Py_XDECREF(self->y);
    PyObject_Free(self);
    HC_debug_output("delete a position.\n");
}

static PyObject *HC_Position_get_x(HC_Position *self, void *closure) {
    return Py_NewRef(self->x);
}

static int HC_Position_set_x(HC_Position *self, PyObject *new_value, void *closure) {
    if (PyFloat_Check(new_value) || PyLong_Check(new_value)) {
        Py_XDECREF(self->x);
        self->x = PyNumber_Float(new_value);
        return 0;
    }
    PyErr_SetString(PyExc_TypeError, "Position.x value must be a float or a int.");
    return -1;
}

static PyObject *HC_Position_get_y(HC_Position *self, void *closure) {
    return Py_NewRef(self->y);
}

static int HC_Position_set_y(HC_Position *self, PyObject *new_value, void *closure) {
    if (PyFloat_Check(new_value) || PyLong_Check(new_value)) {
        Py_XDECREF(self->y);
        self->y = PyNumber_Float(new_value);
        return 0;
    }
    PyErr_SetString(PyExc_TypeError, "Position.y value must be a float or a int.");
    return -1;
}


static PyGetSetDef HC_Position_getSetDef[] = {
        {"x", (getter) HC_Position_get_x, (setter) HC_Position_set_x, "set or get Position.x."},
        {"y", (getter) HC_Position_get_y, (setter) HC_Position_set_y, "set or get Position.y."},
        {NULL}
};
//static struct PyMemberDef HC_PositionMembers[] = {
//        {NULL, 0, 0, 0, NULL}
//};

PyTypeObject HC_PositionType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        .tp_name = "HCGame.Position",
        .tp_basicsize = sizeof(HC_Position),
        .tp_itemsize = 0,
        .tp_flags = Py_TPFLAGS_DEFAULT,
        .tp_new = HC_Position_new,      // 创建对象（分配内存）
        .tp_init = (initproc) HC_Position_init, // 初始化 __init__
        .tp_dealloc = (destructor) HC_Position_dealloc, // 销毁 __del__，无循环引用不需要GC介入
        .tp_doc = "CPosition",
//        .tp_members = HC_PositionMembers,
        .tp_getset = HC_Position_getSetDef
};