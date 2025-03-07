#ifndef WIDGETS_H
#define WIDGETS_H

#include <stdio.h>
#include <math.h>
#include "internal/layout.h"


#if defined(GL_VERSION)
#include <GL/gl.h>
#if defined(__linux__)
#include <GL/glx.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#elif defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#include <GL/gl.h>
#elif defined(__APPLE__)
#include <OpenGL/gl.h>
#include <OpenGL/CGLCurrent.h>
#endif

// Common color definitions
typedef struct {
    float r, g, b, a;
} SColor;

// Common shape properties
typedef struct {
    float x, y;
    float width, height;
    float rotation;
    SColor color;
} SShapeProps;

// Function to create a color
static inline SColor SCreateColor(float r, float g, float b, float a) {
    SColor color = {r, g, b, a}; // Fixed the order: r->r, g->g, b->b
    return color;
}

// Function to create color from RGB array
static inline SColor SCreateColorFromArray(float color[3], float alpha) {
    return SCreateColor(color[0], color[1], color[2], alpha);
}

// Function to create shape properties
static inline SShapeProps SCreateShapeProps(float x, float y, float width, float height, float rotation, SColor color) {
    SShapeProps props = {x, y, width, height, rotation, color};
    return props;
}

// Buffer swap helper function
static inline void SSwapBuffers(SApplication *app) {
#if defined(__linux__)
    glXSwapBuffers(app->display, app->window);
#elif defined(_WIN32) || defined(_WIN64)
    SwapBuffers(app->hdc);
#elif defined(__APPLE__)
    CGLFlushDrawable(CGLGetCurrentContext());
#endif
}

// Drawing functions with consistent parameters
void STriangle(SApplication *app, const SShapeProps *props);
void SRectangle(SApplication *app, const SShapeProps *props);
void SCircle(SApplication *app, const SShapeProps *props);
void SPolygon(SApplication *app, const SShapeProps *props, const float *vertices, int vertexCount);

// Helper functions for direct color array usage
void SDrawTriangle(SApplication *app, float color[3], float posX, float posY);
void SDrawRectangle(SApplication *app, float color[3], float posX, float posY);
void SDrawCircle(SApplication *app, float color[3], float posX, float posY);

// Implementation of drawing functions
void STriangle(SApplication *app, const SShapeProps *props) {
    glPushMatrix();
    
    // Apply transformations
    glTranslatef(props->x, props->y, 0.0f);
    glRotatef(props->rotation, 0.0f, 0.0f, 1.0f);
    glScalef(props->width, props->height, 1.0f);
    
    // Set color with alpha
    glColor4f(props->color.r, props->color.g, props->color.b, props->color.a);
    
    glBegin(GL_TRIANGLES);
        glVertex2f(-0.5f, -0.5f);
        glVertex2f(0.5f, -0.5f);
        glVertex2f(0.0f, 0.5f);
    glEnd();
    
    glPopMatrix();
    
    SSwapBuffers(app);
}

void SRectangle(SApplication *app, const SShapeProps *props) {
    glPushMatrix();
    
    // Apply transformations
    glTranslatef(props->x, props->y, 0.0f);
    glRotatef(props->rotation, 0.0f, 0.0f, 1.0f);
    glScalef(props->width, props->height, 1.0f);
    
    // Set color with alpha
    glColor4f(props->color.r, props->color.g, props->color.b, props->color.a);
    
    glBegin(GL_QUADS);
        glVertex2f(-0.5f, -0.5f);
        glVertex2f(0.5f, -0.5f);
        glVertex2f(0.5f, 0.5f);
        glVertex2f(-0.5f, 0.5f);
    glEnd();
    
    glPopMatrix();
    
    SSwapBuffers(app);
}

void SCircle(SApplication *app, const SShapeProps *props) {
    glPushMatrix();
    
    // Apply transformations
    glTranslatef(props->x, props->y, 0.0f);
    glRotatef(props->rotation, 0.0f, 0.0f, 1.0f);
    glScalef(props->width, props->height, 1.0f);
    
    // Set color with alpha
    glColor4f(props->color.r, props->color.g, props->color.b, props->color.a);
    
    // Draw circle using triangles
    const int segments = 36;
    const float angleIncrement = 2.0f * M_PI / segments;
    
    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(0.0f, 0.0f); // Center point
        
        for (int i = 0; i <= segments; i++) {
            float angle = i * angleIncrement;
            float x = 0.5f * cosf(angle);
            float y = 0.5f * sinf(angle);
            glVertex2f(x, y);
        }
    glEnd();
    
    glPopMatrix();
    
    SSwapBuffers(app);
}

void SPolygon(SApplication *app, const SShapeProps *props, const float *vertices, int vertexCount) {
    if (vertexCount < 3 || vertices == NULL) {
        fprintf(stderr, "Error: Invalid polygon data\n");
        return;
    }
    
    glPushMatrix();
    
    // Apply transformations
    glTranslatef(props->x, props->y, 0.0f);
    glRotatef(props->rotation, 0.0f, 0.0f, 1.0f);
    glScalef(props->width, props->height, 1.0f);
    
    // Set color with alpha
    glColor4f(props->color.r, props->color.g, props->color.b, props->color.a);
    
    glBegin(GL_POLYGON);
        for (int i = 0; i < vertexCount * 2; i += 2) {
            glVertex2f(vertices[i], vertices[i+1]);
        }
    glEnd();
    
    glPopMatrix();
    
    SSwapBuffers(app);
}

void SDrawTriangle(SApplication *app, float color[3], float posX, float posY) {
    SColor c = SCreateColorFromArray(color, 1.0f);
    SShapeProps props = {posX, posY, 1.0f, 1.0f, 0.0f, c};
    STriangle(app, &props);
}

void SDrawRectangle(SApplication *app, float color[3], float posX, float posY) {
    SColor c = SCreateColorFromArray(color, 1.0f);
    SShapeProps props = {posX, posY, 1.0f, 1.0f, 0.0f, c};
    SRectangle(app, &props);
}

void SDrawCircle(SApplication *app, float color[3], float posX, float posY) {
    SColor c = SCreateColorFromArray(color, 1.0f);
    SShapeProps props = {posX, posY, 1.0f, 1.0f, 0.0f, c};
    SCircle(app, &props);
}

#elif defined(VULKAN_VERSION_1_0)

#include <vulkan/vulkan.h>

typedef struct {
    float r, g, b, a;
} SColor;

typedef struct {
    float x, y;
    float width, height;
    float rotation;
    SColor color;
} SShapeProps;

// Function to create a color
static inline SColor SCreateColor(float r, float g, float b, float a) {
    SColor color = {r, g, b, a}; // Fixed order
    return color;
}

// Function to create color from RGB array
static inline SColor SCreateColorFromArray(float color[3], float alpha) {
    return SCreateColor(color[0], color[1], color[2], alpha);
}

void STriangle(SApplication *app, const SShapeProps *props) {
    // TODO: Implement Vulkan rendering
    printf("Vulkan triangle rendering not yet implemented\n");
}

void SRectangle(SApplication *app, const SShapeProps *props) {
    // TODO: Implement Vulkan rendering
    printf("Vulkan rectangle rendering not yet implemented\n");
}

void SCircle(SApplication *app, const SShapeProps *props) {
    // TODO: Implement Vulkan rendering
    printf("Vulkan circle rendering not yet implemented\n");
}

void SDrawTriangle(SApplication *app, float color[3], float posX, float posY) {
    SColor c = SCreateColorFromArray(color, 1.0f);
    SShapeProps props = {posX, posY, 1.0f, 1.0f, 0.0f, c};
    STriangle(app, &props);
}

void SDrawRectangle(SApplication *app, float color[3], float posX, float posY) {
    SColor c = SCreateColorFromArray(color, 1.0f);
    SShapeProps props = {posX, posY, 1.0f, 1.0f, 0.0f, c};
    SRectangle(app, &props);
}

void SDrawCircle(SApplication *app, float color[3], float posX, float posY) {
    SColor c = SCreateColorFromArray(color, 1.0f);
    SShapeProps props = {posX, posY, 1.0f, 1.0f, 0.0f, c};
    SCircle(app, &props);
}

#else

typedef struct {
    float r, g, b, a;
} SColor;

typedef struct {
    float x, y;
    float width, height;
    float rotation;
    SColor color;
} SShapeProps;

// Function to create a color
static inline SColor SCreateColor(float r, float g, float b, float a) {
    SColor color = {r, g, b, a}; // Fixed order
    return color;
}

// Function to create color from RGB array
static inline SColor SCreateColorFromArray(float color[3], float alpha) {
    return SCreateColor(color[0], color[1], color[2], alpha);
}

void STriangle(SApplication *app, const SShapeProps *props) {
    printf("No rendering backend available\n");
}

void SRectangle(SApplication *app, const SShapeProps *props) {
    printf("No rendering backend available\n");
}

void SCircle(SApplication *app, const SShapeProps *props) {
    printf("No rendering backend available\n");
}

void SDrawTriangle(SApplication *app, float color[3], float posX, float posY) {
    SColor c = SCreateColorFromArray(color, 1.0f);
    SShapeProps props = {posX, posY, 1.0f, 1.0f, 0.0f, c};
    STriangle(app, &props);
}

void SDrawRectangle(SApplication *app, float color[3], float posX, float posY) {
    SColor c = SCreateColorFromArray(color, 1.0f);
    SShapeProps props = {posX, posY, 1.0f, 1.0f, 0.0f, c};
    SRectangle(app, &props);
}

void SDrawCircle(SApplication *app, float color[3], float posX, float posY) {
    SColor c = SCreateColorFromArray(color, 1.0f);
    SShapeProps props = {posX, posY, 1.0f, 1.0f, 0.0f, c};
    SCircle(app, &props);
}

#endif // Graphics API checks

#endif // WIDGETS_H
