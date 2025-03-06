#ifndef WIDGETS_H
#define WIDGETS_H
#include <stdio.h>
#if defined(GL_VERSION)
#include <GL/gl.h>
#if defined(__linux__)
#include <GL/glx.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#elif defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#include <GL/gl.h>
#endif

void STriangle(SApplication *app, float color[3], float posX, float posY) {
    glClear(GL_COLOR_BUFFER_BIT);
    glBegin(GL_TRIANGLES);
        glColor3f(color[0], color[1], color[2]);
  
        glVertex2f(-0.5f + posX, -0.5f + posY); 
        glVertex2f(0.5f + posX, -0.5f + posY);  
        glVertex2f(0.0f + posX, 0.5f + posY);   
    glEnd();
    
    #if defined(__linux__)
    glXSwapBuffers(app->display, app->window);
    #elif defined(_WIN32) || defined(_WIN64)
    SwapBuffers(app->hdc);
    #endif
}


void SRectangle(SApplication *app, float color[3], float posX, float posY) {
    glBegin(GL_QUADS);
        glColor3f(color[0], color[1], color[2]);
      
        glVertex2f(-0.5f + posX, -0.5f + posY); 
        glVertex2f(0.5f + posX, -0.5f + posY);  
        glVertex2f(0.5f + posX, 0.5f + posY);   
        glVertex2f(-0.5f + posX, 0.5f + posY);  
    glEnd();
    
    #if defined(__linux__)
    glXSwapBuffers(app->display, app->window);
    #elif defined(_WIN32) || defined(_WIN64)
    SwapBuffers(app->hdc);
    #endif
}


void STriangle(SApplication *app, float color[3]) {
    STriangle(app, color, 0.0f, 0.0f);
}

void SRectangle(SApplication *app, float color[3]) {
    SRectangle(app, color, 0.0f, 0.0f);
}

void SCircle(SApplication *app, float *color) {
    //TODO: Add circle support.
    printf("I'm sorry! This feature is not implemented... Check back in updated versions.");
}
#elif defined(VULKAN_VERSION_1_0)
#else 
#endif //VULKAN
#endif //WIDGETS_H
