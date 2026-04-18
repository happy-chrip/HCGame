// Created by ABai on 2026/3/22.
#include "HCGame.h"
#include "log.h"
#include "role.h"
#include "event.h"
#include "surface.h"
#include "position.h"
#include "key.h"

static PyObject *add(PyObject *self, PyObject *args) {
    int a, b;
    if (!PyArg_ParseTuple(args, "ii", &a, &b)) {
        return NULL;
    }
    return PyLong_FromLong(a + b);
}

// 模块退出清理函数（必须叫 模块名_free）
static void HCGame_free(void *module) {
    HCGame_is_exiting = true;
    // 退出逻辑：释放内存、关闭文件、断开连接、清理资源等
    HC_debug_output("Exiting...\n");
    if (role_list) {
        Py_CLEAR(role_list);
    }
    Py_XDECREF(display);
    SDL_Quit();
    HC_debug_output("Exit.\n");
}

// 方法列表
static PyMethodDef MyModuleMethods[] = {
    {"add", add, METH_VARARGS, NULL},
    {"has_quit_event", HC_Event_has_quit_event, METH_NOARGS, NULL},
    {"load_image_as_surface", (PyCFunction)HC_Surface_load_image_as_surface,
     METH_KEYWORDS | METH_VARARGS, "Load a Surface from image path."},
    {"key_pressed", (PyCFunction)HC_Key_key_pressed,
     METH_KEYWORDS | METH_VARARGS, "Get key state from name of one key."},
    // 结束标记，表示写完了函数声明
    {NULL, NULL, 0, NULL}};

// 模块定义
static struct PyModuleDef moduleDef = {
        .m_base = PyModuleDef_HEAD_INIT,
        .m_name = "HCGame",   // 模块名（import HCGame）
        .m_doc = "C LIB API of HCGame",
        .m_size = -1,
        .m_methods = MyModuleMethods,
        .m_free = HCGame_free
};

HC_Display *display;
PyObject *role_list;
bool HC_DEBUG_MODE = false;
bool HCGame_is_exiting = false;
// 模块初始化函数（必须叫 PyInit_模块名）
PyMODINIT_FUNC PyInit_HCGame(void) {
    PyObject *module;

    // 准备Python元类(类似object)
    if (PyType_Ready(&HC_RoleType) < 0)
        return NULL;

    if (PyType_Ready(&HC_DisplayType) < 0)
        return NULL;

    if (PyType_Ready(&HC_SurfaceType) < 0)
        return NULL;

    if (PyType_Ready(&HC_PositionType) < 0)
        return NULL;

    module = PyModule_Create(&moduleDef);
    if (!module) return NULL;

    if (getenv("HCGame_DEBUG_MODE") != NULL && strcmp(getenv("HCGame_DEBUG_MODE"), "1") == 0) {
        HC_DEBUG_MODE = true;
        HC_debug_output("Debug mode of HCGame is ready.\n");
    }


    role_list = PyList_New(0);

    display = PyObject_New(HC_Display, &HC_DisplayType);

    // 注册Python类(class)，注释掉则Python层不可见这个类，但可以用返回这个类的函数来获取它
    PyModule_AddObjectRef(module, "Role", (PyObject *) &HC_RoleType);

    PyModule_AddObjectRef(module, "Position", (PyObject *) &HC_PositionType);

    PyModule_AddObjectRef(module, "display", (PyObject *) display);
    HC_debug_output("[C] loaded.\n");
    return module;
}
