#include "key.h"

#include "log.h"
#include <SDL3/SDL.h>

PyObject* HC_Key_key_pressed(PyObject* self, PyObject* args, PyObject* kwds) {
    const char * key_name;
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "s",
                                     (char*[]){"key_name", NULL}, &key_name)) {
        return NULL;
    }
    SDL_PumpEvents();
    const bool * key_state = SDL_GetKeyboardState(NULL);
    SDL_Keycode key_code = SDL_GetKeyFromName(key_name);
    if (key_code == SDLK_UNKNOWN) {
        HC_debug_output("Can't understand key_name\"%s\" -> key_code(%d).\n", key_name, key_code);
        PyErr_SetString(PyExc_ValueError, "Can't understand key_name\"%s\".");
        return NULL;
    }
    if (key_state[key_code]) {
        Py_RETURN_TRUE;
    } else {
        Py_RETURN_FALSE;
    }
}