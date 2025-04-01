#ifndef WIDGETS_H
#define WIDGETS_H

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>
#include "internal/layout.h"
#include "internal/font.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "internal/stb_truetype.h"

// Color structure definition
typedef struct {
    float r, g, b, a;
} SColor;

// Shape properties structure
typedef struct {
    float x, y;
    float width, height;
    float rotation;
    SColor color;
} SShapeProps;

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


typedef struct {
    // Programs and shaders
    GLuint basicProgram;
    GLuint textProgram;
    
    // Shape meshes
    GLuint rectVAO, rectVBO, rectEBO;
    GLuint triangleVAO, triangleVBO;
    GLuint circleVAO, circleVBO, circleEBO;
    
    // Text rendering resources
    GLuint fontTexture;
    GLuint textVAO, textVBO;
    
    // Uniform locations
    GLint modelLoc;
    GLint projectionLoc;
    GLint colorLoc;
} SRenderer;
SRenderer renderer;

// Initialize the renderer
bool SInitializeRenderer();

// Create a color
static inline SColor SCreateColor(float r, float g, float b, float a) {
    SColor color = {r, g, b, a}; 
    return color;
}

// Create color from RGB array
static inline SColor SCreateColorFromArray(float color[3], float alpha) {
    return SCreateColor(color[0], color[1], color[2], alpha);
}

// Create shape properties
static inline SShapeProps SCreateShapeProps(float x, float y, float width, float height, float rotation, SColor color) {
    SShapeProps props = {x, y, width, height, rotation, color};
    return props;
}

// Swap buffers (platform-specific)
static inline void SSwapBuffers(SApplication *app) {
#if defined(__linux__)
    glXSwapBuffers(app->display, app->window);
#elif defined(_WIN32) || defined(_WIN64)
    SwapBuffers(app->hdc);
#elif defined(__APPLE__)
    CGLFlushDrawable(CGLGetCurrentContext());
#endif
}

// Drawing functions
void STriangle(SApplication *app, const SShapeProps *props);
void SRectangle(SApplication *app, const SShapeProps *props);
void SCircle(SApplication *app, const SShapeProps *props);
void SPolygon(SApplication *app, const SShapeProps *props, const float *vertices, int vertexCount);

// Helper functions for direct color array usage
void SDrawTriangle(SApplication *app, float color[3], float posX, float posY, float size);
void SDrawRectangle(SApplication *app, float color[3], float posX, float posY, float width, float height);
void SDrawCircle(SApplication *app, float color[3], float posX, float posY, float radius);

// Clean up renderer resources
void SCleanupRenderer();

static GLuint compileShader(const char* source, GLenum type) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);
    
    // Check for compilation errors
    GLint success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        fprintf(stderr, "ERROR::SHADER::COMPILATION_FAILED\n%s\n", infoLog);
    }
    
    return shader;
}

static GLuint createShaderProgram(const char* vertexSource, const char* fragmentSource) {
    GLuint vertexShader = compileShader(vertexSource, GL_VERTEX_SHADER);
    GLuint fragmentShader = compileShader(fragmentSource, GL_FRAGMENT_SHADER);
    
    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    
    // Check for linking errors
    GLint success;
    char infoLog[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        fprintf(stderr, "ERROR::SHADER::PROGRAM::LINKING_FAILED\n%s\n", infoLog);
    }
    
    // Clean up shaders
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    return program;
}


// Matrix utility functions
static void identityMatrix(float* matrix) {
    memset(matrix, 0, 16 * sizeof(float));
    matrix[0] = 1.0f;
    matrix[5] = 1.0f;
    matrix[10] = 1.0f;
    matrix[15] = 1.0f;
}

static void translateMatrix(float* matrix, float x, float y, float z) {
    matrix[12] = x;
    matrix[13] = y;
    matrix[14] = z;
}

static void scaleMatrix(float* matrix, float x, float y, float z) {
    matrix[0] = x;
    matrix[5] = y;
    matrix[10] = z;
}

static void rotateMatrix(float* matrix, float angle) {
    float rad = angle * M_PI / 180.0f;
    float c = cosf(rad);
    float s = sinf(rad);
    
    // Create a temporary matrix with just rotation
    float rotMat[16] = {
        c,   -s,   0.0f, 0.0f,
        s,    c,   0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    
    // Note: In a production environment, you would implement proper
    // matrix multiplication here. This is simplified.
    memcpy(matrix, rotMat, 16 * sizeof(float));
}

static void createTransformMatrix(float* matrix, float x, float y, float width, float height, float rotation) {
    identityMatrix(matrix);
    
    // Order: scale, rotate, translate
    scaleMatrix(matrix, width, height, 1.0f);
    
    if (rotation != 0.0f) {
        float rotMat[16];
        identityMatrix(rotMat);
        rotateMatrix(rotMat, rotation);
        
        // Apply rotation (simplified)
        // In production code, use proper matrix multiplication
        matrix[0] = rotMat[0] * width;
        matrix[1] = rotMat[1] * width;
        matrix[4] = rotMat[4] * height;
        matrix[5] = rotMat[5] * height;
    }
    
    translateMatrix(matrix, x, y, 0.0f);
}

// Implementation of the renderer initialization
bool SInitializeRenderer() {
    // Basic shader for shapes
    const char* vertexShaderSource = 
        "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "uniform mat4 model;\n"
        "uniform mat4 projection;\n"
        "void main()\n"
        "{\n"
        "   gl_Position = projection * model * vec4(aPos, 1.0);\n"
        "}\0";
    
    const char* fragmentShaderSource = 
        "#version 330 core\n"
        "out vec4 FragColor;\n"
        "uniform vec4 color;\n"
        "void main()\n"
        "{\n"
        "   FragColor = color;\n"
        "}\0";
    
    // Create shader programs
    renderer.basicProgram = createShaderProgram(vertexShaderSource, fragmentShaderSource);
    
    // Get uniform locations
    renderer.modelLoc = glGetUniformLocation(renderer.basicProgram, "model");
    renderer.projectionLoc = glGetUniformLocation(renderer.basicProgram, "projection");
    renderer.colorLoc = glGetUniformLocation(renderer.basicProgram, "color");
    
    // Create rectangle mesh
    float rectangleVertices[] = {
        -0.5f, -0.5f, 0.0f,  // bottom left
         0.5f, -0.5f, 0.0f,  // bottom right
         0.5f,  0.5f, 0.0f,  // top right
        -0.5f,  0.5f, 0.0f   // top left
    };
    
    unsigned int rectangleIndices[] = {
        0, 1, 2,  // first triangle
        2, 3, 0   // second triangle
    };
    
    glGenVertexArrays(1, &renderer.rectVAO);
    glGenBuffers(1, &renderer.rectVBO);
    glGenBuffers(1, &renderer.rectEBO);
    
    glBindVertexArray(renderer.rectVAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, renderer.rectVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVertices), rectangleVertices, GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer.rectEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(rectangleIndices), rectangleIndices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Create triangle mesh
    float triangleVertices[] = {
        -0.5f, -0.5f, 0.0f,  // bottom left
         0.5f, -0.5f, 0.0f,  // bottom right
         0.0f,  0.5f, 0.0f   // top
    };
    
    glGenVertexArrays(1, &renderer.triangleVAO);
    glGenBuffers(1, &renderer.triangleVBO);
    
    glBindVertexArray(renderer.triangleVAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, renderer.triangleVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Create circle mesh
    const int segments = 36;
    const float angleIncrement = 2.0f * M_PI / segments;
    
    // Allocate memory for vertices (center + segments + 1 duplicate vertex)
    float* circleVertices = (float*)malloc((segments + 2) * 3 * sizeof(float));
    unsigned int* circleIndices = (unsigned int*)malloc(segments * 3 * sizeof(unsigned int));
    
    // Center vertex
    circleVertices[0] = 0.0f;
    circleVertices[1] = 0.0f;
    circleVertices[2] = 0.0f;
    
    // Perimeter vertices
    for (int i = 0; i <= segments; i++) {
        float angle = i * angleIncrement;
        circleVertices[(i + 1) * 3 + 0] = 0.5f * cosf(angle);
        circleVertices[(i + 1) * 3 + 1] = 0.5f * sinf(angle);
        circleVertices[(i + 1) * 3 + 2] = 0.0f;
        
        if (i < segments) {
            // Triangle fan: center, current, next
            circleIndices[i * 3 + 0] = 0;                  // Center
            circleIndices[i * 3 + 1] = i + 1;              // Current
            circleIndices[i * 3 + 2] = (i + 1) % segments + 1; // Next (wrap around)
        }
    }
    
    glGenVertexArrays(1, &renderer.circleVAO);
    glGenBuffers(1, &renderer.circleVBO);
    glGenBuffers(1, &renderer.circleEBO);
    
    glBindVertexArray(renderer.circleVAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, renderer.circleVBO);
    glBufferData(GL_ARRAY_BUFFER, (segments + 2) * 3 * sizeof(float), circleVertices, GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer.circleEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, segments * 3 * sizeof(unsigned int), circleIndices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Cleanup
    free(circleVertices);
    free(circleIndices);
    
    // Unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
    return true;
}

// Implementation of drawing functions
void STriangle(SApplication *app, const SShapeProps *props) {
    // Create model matrix
    float modelMatrix[16];
    createTransformMatrix(modelMatrix, props->x, props->y, props->width, props->height, props->rotation);
    
    // Use shader
    glUseProgram(renderer.basicProgram);
    
    // Set uniforms
    glUniformMatrix4fv(renderer.modelLoc, 1, GL_FALSE, modelMatrix);
    // Note: Projection matrix should be set elsewhere, like in the app update
    
    // Set color
    glUniform4f(renderer.colorLoc, props->color.r, props->color.g, props->color.b, props->color.a);
    
    // Draw triangle
    glBindVertexArray(renderer.triangleVAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);
}

void SRectangle(SApplication *app, const SShapeProps *props) {
    // Create model matrix
    float modelMatrix[16];
    createTransformMatrix(modelMatrix, props->x, props->y, props->width, props->height, props->rotation);
    
    // Use shader
    glUseProgram(renderer.basicProgram);
    
    // Set uniforms
    glUniformMatrix4fv(renderer.modelLoc, 1, GL_FALSE, modelMatrix);
    
    // Set color
    glUniform4f(renderer.colorLoc, props->color.r, props->color.g, props->color.b, props->color.a);
    
    // Draw rectangle
    glBindVertexArray(renderer.rectVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void SCircle(SApplication *app, const SShapeProps *props) {
    // Create model matrix
    float modelMatrix[16];
    createTransformMatrix(modelMatrix, props->x, props->y, props->width, props->height, props->rotation);
    
    // Use shader
    glUseProgram(renderer.basicProgram);
    
    // Set uniforms
    glUniformMatrix4fv(renderer.modelLoc, 1, GL_FALSE, modelMatrix);
    
    // Set color
    glUniform4f(renderer.colorLoc, props->color.r, props->color.g, props->color.b, props->color.a);
    
    // Draw circle
    glBindVertexArray(renderer.circleVAO);
    glDrawElements(GL_TRIANGLES, 36 * 3, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void SPolygon(SApplication *app, const SShapeProps *props, const float *vertices, int vertexCount) {
    if (vertexCount < 3 || vertices == NULL) {
        fprintf(stderr, "Error: Invalid polygon data\n");
        return;
    }
    
    // For polygons, we need to create a custom VAO each time
    // (since the vertices are dynamic)
    GLuint polyVAO, polyVBO;
    glGenVertexArrays(1, &polyVAO);
    glGenBuffers(1, &polyVBO);
    
    glBindVertexArray(polyVAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, polyVBO);
    glBufferData(GL_ARRAY_BUFFER, vertexCount * 2 * sizeof(float), vertices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Create model matrix
    float modelMatrix[16];
    createTransformMatrix(modelMatrix, props->x, props->y, props->width, props->height, props->rotation);
    
    // Use shader
    glUseProgram(renderer.basicProgram);
    
    // Set uniforms
    glUniformMatrix4fv(renderer.modelLoc, 1, GL_FALSE, modelMatrix);
    
    // Set color
    glUniform4f(renderer.colorLoc, props->color.r, props->color.g, props->color.b, props->color.a);
    
    // Draw polygon
    glDrawArrays(GL_TRIANGLE_FAN, 0, vertexCount);
    
    // Clean up temporary resources
    glDeleteVertexArrays(1, &polyVAO);
    glDeleteBuffers(1, &polyVBO);
}

// Helper functions
void SDrawTriangle(SApplication *app, float color[3], float posX, float posY, float size) {
    SColor sColor = SCreateColorFromArray(color, 1.0f);
    SShapeProps props = SCreateShapeProps(posX, posY, size, size, 0.0f, sColor);
    STriangle(app, &props);
}

void SDrawRectangle(SApplication *app, float color[3], float posX, float posY, float width, float height) {
    SColor sColor = SCreateColorFromArray(color, 1.0f);
    SShapeProps props = SCreateShapeProps(posX, posY, width, height, 0.0f, sColor);
    SRectangle(app, &props);
}

void SDrawCircle(SApplication *app, float color[3], float posX, float posY, float radius) {
    SColor sColor = SCreateColorFromArray(color, 1.0f);
    SShapeProps props = SCreateShapeProps(posX, posY, radius * 2.0f, radius * 2.0f, 0.0f, sColor);
    SCircle(app, &props);
}

// Clean up renderer resources
void SCleanupRenderer() {
    // Delete shader programs
    glDeleteProgram(renderer.basicProgram);
    
    // Delete VAOs and VBOs
    glDeleteVertexArrays(1, &renderer.rectVAO);
    glDeleteBuffers(1, &renderer.rectVBO);
    glDeleteBuffers(1, &renderer.rectEBO);
    
    glDeleteVertexArrays(1, &renderer.triangleVAO);
    glDeleteBuffers(1, &renderer.triangleVBO);
    
    glDeleteVertexArrays(1, &renderer.circleVAO);
    glDeleteBuffers(1, &renderer.circleVBO);
    glDeleteBuffers(1, &renderer.circleEBO);
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
    SColor color = {r, g, b, a}; 
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
    SColor color = {r, g, b, a}; 
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

}

#endif // Graphics API checks

//GLOBAL stuff that dosen't care about version.


void checkGLSLVersion() { //Easy stuff to prevent a random error.
    const GLubyte* versionStr = glGetString(GL_SHADING_LANGUAGE_VERSION);
    if (versionStr == NULL) {
        fprintf(stderr, "ERROR: Unable to retrieve GLSL version.\n");
        exit(EXIT_FAILURE);
    }

    int major = 0, minor = 0;
    if (sscanf((const char*)versionStr, "%d.%d", &major, &minor) != 2) {
        fprintf(stderr, "ERROR: Unable to parse GLSL version.\n");
        exit(EXIT_FAILURE);
    }

    // Check if version < 3.3
    if (major < 3 || (major == 3 && minor < 3)) {
        fprintf(stderr, "ERROR: GLSL version 3.3 is required. Detected version: %d.%d\n", major, minor);
        exit(EXIT_FAILURE);
    }
}
void SGL_Error(const char* operation) {
    GLenum error;
    while ((error = glGetError()) != GL_NO_ERROR) {
        fprintf(stderr, "OpenGL error after %s: 0x%x\n", operation, error);
    }
}


// Global variables for text rendering
GLuint fontTexture;
GLuint textVAO, textVBO;
GLuint textShader;
stbtt_bakedchar charData[96]; // ASCII 32..126 is 95 glyphs
float fontTexWidth = 512;
float fontTexHeight = 512;

// Generate font texture using stb_truetype
bool generateFontTexture(const char* fontPath) {
    // Load font file
    FILE* fontFile = fopen(fontPath, "rb");
    if (!fontFile) {
        fprintf(stderr, "ERROR: Failed to open font file: %s\n", fontPath);
        return false;
    }
    
    // Get file size
    fseek(fontFile, 0, SEEK_END);
    long fileSize = ftell(fontFile);
    fseek(fontFile, 0, SEEK_SET);
    
    // Read font file data
    unsigned char* fontFileData = (unsigned char*)malloc(fileSize);
    if (!fontFileData) {
        fprintf(stderr, "ERROR: Failed to allocate memory for font file\n");
        fclose(fontFile);
        return false;
    }
    
    fread(fontFileData, 1, fileSize, fontFile);
    fclose(fontFile);
    
    // Create bitmap for font texture atlas
    unsigned char* bitmapData = (unsigned char*)malloc(fontTexWidth * fontTexHeight);
    if (!bitmapData) {
        fprintf(stderr, "ERROR: Failed to allocate memory for font bitmap\n");
        free(fontFileData);
        return false;
    }
    
    // Bake font to bitmap
    // Parameters: font data, font index, font height in pixels, bitmap data, width, height, first char, num chars, output char data
    int result = stbtt_BakeFontBitmap(fontFileData, 0, 24.0f, bitmapData, fontTexWidth, fontTexHeight, 32, 96, charData);
    
    if (result <= 0) {
        fprintf(stderr, "ERROR: Failed to bake font bitmap\n");
        free(fontFileData);
        free(bitmapData);
        return false;
    }
    
    // Generate OpenGL texture
    glGenTextures(1, &fontTexture);
    glBindTexture(GL_TEXTURE_2D, fontTexture);
    
    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    // Upload texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, fontTexWidth, fontTexHeight, 0, GL_RED, GL_UNSIGNED_BYTE, bitmapData);
    
    // Clean up
    free(fontFileData);
    free(bitmapData);
    
    return true;
}

bool initText(const char* fontPath) {

    checkGLSLVersion();

    // Save previous OpenGL state
    GLint prevVAO, prevArrayBuffer, prevProgram;
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &prevVAO);
    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &prevArrayBuffer);
    glGetIntegerv(GL_CURRENT_PROGRAM, &prevProgram);
    
    // Generate font texture
    if (!generateFontTexture(fontPath)) {
        fprintf(stderr, "ERROR: Failed to generate font texture\n");
        return false;
    }
    
    // Create VBO and VAO for text rendering
    glGenVertexArrays(1, &textVAO);
    glGenBuffers(1, &textVBO);
    glBindVertexArray(textVAO);
    glBindBuffer(GL_ARRAY_BUFFER, textVBO);
    
    // Pre-allocate buffer data
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    
    // Setup vertex attributes
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
    // Create shader program
    const char* vertexSource = 
        "#version 330 core\n"
        "layout (location = 0) in vec4 vertex;\n"
        "out vec2 TexCoords;\n"
        "uniform mat4 projection;\n"
        "void main() {\n"
        "   gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);\n"
        "   TexCoords = vertex.zw;\n"
        "}\0";
    
    const char* fragmentSource = 
        "#version 330 core\n"
        "in vec2 TexCoords;\n"
        "out vec4 color;\n"
        "uniform sampler2D text;\n"
        "uniform vec3 textColor;\n"
        "void main() {\n"
        "   float alpha = texture(text, TexCoords).r;\n"
        "   color = vec4(textColor, alpha);\n"
        "}\0";
    
    // Compile vertex shader
    GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertexSource, NULL);
    glCompileShader(vertex);
    
    // Check for shader compilation errors
    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        printf("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n%s\n", infoLog);
    }
    
    // Compile fragment shader
    GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragmentSource, NULL);
    glCompileShader(fragment);
    
    // Check for shader compilation errors
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment, 512, NULL, infoLog);
        printf("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n%s\n", infoLog);
    }
    
    // Link shader program
    textShader = glCreateProgram();
    glAttachShader(textShader, vertex);
    glAttachShader(textShader, fragment);
    glLinkProgram(textShader);
    
    // Check for linking errors
    glGetProgramiv(textShader, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(textShader, 512, NULL, infoLog);
        printf("ERROR::SHADER::PROGRAM::LINKING_FAILED\n%s\n", infoLog);
    }
    
    // Clean up shaders
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    
    // Restore previous OpenGL state
    glBindVertexArray(prevVAO);
    glBindBuffer(GL_ARRAY_BUFFER, prevArrayBuffer);
    glUseProgram(prevProgram);

    // Enable blending for transparent text
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    return true;
}

void SDrawText(SApplication *app, const char* text, float x, float y, float scale, float r, float g, float b) {
    // Save current OpenGL state
    GLint prevProgram;
    glGetIntegerv(GL_CURRENT_PROGRAM, &prevProgram);
    
    GLint prevTexture;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &prevTexture);
    
    GLint prevVAO;
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &prevVAO);
    
    GLint prevBuffer;
    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &prevBuffer);
    
    // Use text shader program
    glUseProgram(textShader);
    glUniform3f(glGetUniformLocation(textShader, "textColor"), r, g, b);

    // Get window dimensions for projection matrix
    float windowWidth = (float)SGetCurrentWindowWidth(app);
    float windowHeight = (float)SGetCurrentWindowHeight(app);
    
    // Setup orthographic projection matrix (screen space)
    GLfloat projection[16] = {
        2.0f/windowWidth, 0.0f, 0.0f, 0.0f,
        0.0f, -2.0f/windowHeight, 0.0f, 0.0f,
        0.0f, 0.0f, -1.0f, 0.0f,
        -1.0f, 1.0f, 0.0f, 1.0f
    };
    glUniformMatrix4fv(glGetUniformLocation(textShader, "projection"), 1, GL_FALSE, projection);
    
    // Activate texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, fontTexture);
    
    // Bind VAO
    glBindVertexArray(textVAO);
    
    // Starting position
    float startX = x;
    float startY = y;
    
    // Render each character
    while (*text) {
        char c = *text++;
        
        // Handle newlines
        if (c == '\n') {
            y += (charData[0].y1 - charData[0].y0) * 1.25f * scale;
            x = startX;
            continue;
        }
        
        // Skip non-printable characters
        if (c < 32 || c > 127) {
            continue;
        }
        
        // Get character data from the baked font
        stbtt_aligned_quad q;
        stbtt_GetBakedQuad(charData, fontTexWidth, fontTexHeight, c - 32, &x, &y, &q, 1);
        
        // Scale the quad
        float x0 = q.x0 * scale;
        float y0 = q.y0 * scale;
        float x1 = q.x1 * scale;
        float y1 = q.y1 * scale;
        
        // Adjust Y position (stb_truetype assumes y increases upwards, OpenGL usually has y increasing downwards)
        float yOffset = startY - y0;
        y0 += yOffset;
        y1 += yOffset;
        
        // Create vertices for this character
        float vertices[6][4] = {
            { x0, y0, q.s0, q.t0 },
            { x0, y1, q.s0, q.t1 },
            { x1, y1, q.s1, q.t1 },
            
            { x0, y0, q.s0, q.t0 },
            { x1, y1, q.s1, q.t1 },
            { x1, y0, q.s1, q.t0 }
        };
        
        // Update VBO
        glBindBuffer(GL_ARRAY_BUFFER, textVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        
        // Draw character
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
    
    // Restore previous OpenGL state
    glBindVertexArray(prevVAO);
    glBindBuffer(GL_ARRAY_BUFFER, prevBuffer);
    glBindTexture(GL_TEXTURE_2D, prevTexture);
    glUseProgram(prevProgram);
}

#endif 
