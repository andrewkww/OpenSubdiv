#ifndef OSD_LOCAL_H
#define OSD_LOCAL_H

#include <stdio.h>

#define OSD_STRINGIFY(src) OSD_XSTRINGIFY(src)
#define OSD_XSTRINGIFY(src) #src

#define CHECK_GL_ERROR(...)  \
    if(GLuint err = glGetError()) {   \
    printf("GL error %x :", err); \
    printf(__VA_ARGS__); \
    }

#define OSD_ERROR(...) printf(__VA_ARGS__);

//#define OSD_DEBUG(...) printf(__VA_ARGS__);
#define OSD_DEBUG(...)

#define PRINT_GL_INFO() \
    printf("Renderer: %s\n", glGetString(GL_RENDERER)); \
    printf("Vendor: %s\n", glGetString(GL_VENDOR)); \
    printf("OpenGL version: %s\n", glGetString(GL_VERSION));

#endif // OSD_LOCAL_H
