// Created by ABai on 2026/3/24.

#include "event.h"
#include <SDL3/SDL.h>
PyObject * HC_Event_has_quit_event(PyObject* self, PyObject* args) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_EVENT_QUIT:
                return PyBool_FromLong(1);
        }
    }
    return PyBool_FromLong(0);  // 记得别用Py_True，引用计数会炸
}