// Created by ABai on 2026/3/24.

#ifndef HCGAME_DISPLAY_H
#define HCGAME_DISPLAY_H

#include <Python.h>
#include <SDL3/SDL.h>

typedef struct {
    PyObject_HEAD  // doc为“这是一个在声明代表无可变长度对象的新类型时所使用的宏”
    PyObject * surface;
    SDL_Window * window;
    SDL_Renderer * renderer;
} HC_Display;


extern PyTypeObject HC_DisplayType;

void HC_Display_get_window_size(int * width, int * height);

#endif //HCGAME_DISPLAY_H
