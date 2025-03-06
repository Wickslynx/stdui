#ifndef WIDGETS_H
#define WIDGETS_H

#include <stdio.h>

#include "internal/utils.h"

#if defined(GL_VERSION)
#include <GL/gl.h>

#if defined(__linux__)
#include <GL/glx.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#elif defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#endif


void STriangle(SApplication *app, float color[3]) {
    glClear(GL_COLOR_BUFFER_BIT);

    glBegin(GL_TRIANGLES);
        glColor3f(color[0], color[1], color[2]);
        glVertex2f(-0.5f, -0.5f); 
        glVertex2f(0.5f, -0.5f);  
        glVertex2f(0.0f, 0.5f);   
    glEnd();

    glXSwapBuffers(app->display, app->window);
}

void SRectangle(SApplication *app, float color[3]) {
    glBegin(GL_QUADS);
        glColor3f(color[0], color[1], color[2]);
        glVertex2f(-0.5f, -0.5f); 
        glVertex2f(0.5f, -0.5f);  
        glVertex2f(0.5f, 0.5f);   
        glVertex2f(-0.5f, 0.5f);  
    glEnd();

    glXSwapBuffers(app->display, app->window);
}

void SCircle(SApplication *app, float *color) {
    //TODO: Add circle support.
    printf("I'm sorry! This feature is not implemented... Check back in updated versions.");
}

#elif defined(VULKAN_VERSION_1_0)

#else 

#endif //VULKAN

#endif //WIDGETS_H
