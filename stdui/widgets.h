#ifndef WIDGETS_H
#define WIDGETS_H

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>
#include "internal/layout.h"
#include "internal/font.h"


typedef struct {
    float r, g, b, a;
} SColor;


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



// Function to create a color
static inline SColor SCreateColor(float r, float g, float b, float a) {
    SColor color = {r, g, b, a}; 
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
    
}

GLuint fontTexture;
GLuint textVAO, textVBO;
GLuint textShader;


void SCircle(SApplication *app, const SShapeProps *props) {
    glPushMatrix();
    
    // Apply transformations
    glTranslatef(props->x, props->y, 0.0f);
    glRotatef(props->rotation, 0.0f, 0.0f, 1.0f);
    glScalef(props->width, props->height, 1.0f);
    
    // Set color with alpha
    glColor4f(props->color.r, props->color.g, props->color.b, props->color.a);
    
    // Draw circle using triangles (Who made this up? This is stupid.)
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

void SDrawTriangle(SApplication *app, float color[3], float posX, float posY, float size) {
    SColor c = SCreateColorFromArray(color, 1.0f);
    SShapeProps props = {posX, posY, size, size, 0.0f, c};
    STriangle(app, &props);
}

void SDrawRectangle(SApplication *app, float color[3], float posX, float posY, float width, float height) {
    SColor c = SCreateColorFromArray(color, 1.0f);
    SShapeProps props = {posX, posY, width, height, 0.0f, c};
    SRectangle(app, &props);
}

void SDrawCircle(SApplication *app, float color[3], float posX, float posY, float radius) {
    SColor c = SCreateColorFromArray(color, 1.0f);
    SShapeProps props = {posX, posY, radius*2, radius*2, 0.0f, c};
    SCircle(app, &props);
}


// Generate font texture from embedded bitmap data
void generateFontTexture() {
    // Create a texture with all font characters (8x8 pixels each, 16x6 grid)
    const int fontTexWidth = 128;  // 16 chars per row, 8 pixels each
    const int fontTexHeight = 48;  // 6 rows, 8 pixels each
    
    // Create empty texture data (all black)
    unsigned char* textureData = (unsigned char*)malloc(fontTexWidth * fontTexHeight);
    memset(textureData, 0, fontTexWidth * fontTexHeight);
    
    // Fill in the texture data with font bitmap
    for (int charIndex = 0; charIndex < 96; charIndex++) {
        int gridX = (charIndex % 16) * 8;
        int gridY = (charIndex / 16) * 8;
        
        // Copy character bitmap to texture
        for (int x = 0; x < 8; x++) {
            unsigned char column = fontData[charIndex][x];
            for (int y = 0; y < 8; y++) {
                if (column & (1 << y)) {
                    int texX = gridX + x;
                    int texY = gridY + y;
                    textureData[texY * fontTexWidth + texX] = 255; // White pixel
                }
            }
        }
    }

    
    // Generate OpenGL texture
    glGenTextures(1, &fontTexture);
    glBindTexture(GL_TEXTURE_2D, fontTexture);
    
    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    // Upload texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, fontTexWidth, fontTexHeight, 0, GL_RED, GL_UNSIGNED_BYTE, textureData);
    
    // Clean up
    free(textureData);
}


bool initText() {
    GLint prevVAO, prevArrayBuffer, prevProgram;
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &prevVAO);
    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &prevArrayBuffer);
    glGetIntegerv(GL_CURRENT_PROGRAM, &prevProgram);
    
    generateFontTexture();
    
    glGenVertexArrays(1, &textVAO);
    glGenBuffers(1, &textVBO);
    glBindVertexArray(textVAO);
    glBindBuffer(GL_ARRAY_BUFFER, textVBO);
    
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
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
    
    GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragmentSource, NULL);
    glCompileShader(fragment);
    
    // Check for shader compilation errors
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment, 512, NULL, infoLog);
        printf("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n%s\n", infoLog);
    }
    
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
    
    // Clean up
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    
    // Restore previous state
    glBindVertexArray(prevVAO);
    glBindBuffer(GL_ARRAY_BUFFER, prevArrayBuffer);
    glUseProgram(prevProgram);
    
    return true;
}


void SDrawText(SApplication *app, const char* text, float x, float y, float scale, float r, float g, float b) { //Why was this so anoying to make.
    // Save current shader program
    GLint prevProgram;
    glGetIntegerv(GL_CURRENT_PROGRAM, &prevProgram);
    
    // Save current texture binding
    GLint prevTexture;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &prevTexture);
    
    // Save current VAO
    GLint prevVAO;
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &prevVAO);
    
    // Save current buffer binding
    GLint prevBuffer;
    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &prevBuffer);
    
    // Use text shader program
    glUseProgram(textShader);
    glUniform3f(glGetUniformLocation(textShader, "textColor"), r, g, b);

    float windowWidth = (float)SGetCurrentWindowWidth(app->display, app->window);
    float windowHeight = (float)SGetCurrentWindowHeight(app->display, app->window);
    
    
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
    
    // Render text
    float xpos = x;
    while (*text) {
        char c = *text++;
        
        if (c < 32 || c > 127) {
            xpos += 8 * scale;
            continue;
        }
        
        int charIndex = c - 32;
        float s = (charIndex % 16) * 8.0f / 128.0f;
        float t = (charIndex / 16) * 8.0f / 48.0f;
        
        float w = 8 * scale;
        float h = 8 * scale;
        
        float vertices[6][4] = {
            { xpos,     y + h,   s,            t            },
            { xpos,     y,       s,            t + 8.0f/48.0f },
            { xpos + w, y,       s + 8.0f/128.0f, t + 8.0f/48.0f },
            
            { xpos,     y + h,   s,            t            },
            { xpos + w, y,       s + 8.0f/128.0f, t + 8.0f/48.0f },
            { xpos + w, y + h,   s + 8.0f/128.0f, t            }
        };
        
        glBindBuffer(GL_ARRAY_BUFFER, textVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        xpos += w;
    }
    
    // Restore previous state
    glBindVertexArray(prevVAO);
    glBindBuffer(GL_ARRAY_BUFFER, prevBuffer);
    glBindTexture(GL_TEXTURE_2D, prevTexture);
    glUseProgram(prevProgram);
}


// Widget state structure
typedef struct {
    bool active;
    bool hover;
    bool focused;
    bool clicked;
} SWidgetState;

// Text properties
typedef struct {
    const char* text;
    float fontSize;
    SColor color;
} STextProps;


typedef void (*SCallback)(void* userData);


typedef struct {
    SShapeProps base;
    SShapeProps handle;
    float min;
    float max;
    float value;
    float* valuePtr;  // Pointer to store value
    bool vertical;
    bool dragging;
    SCallback callback;
    void* userData;
} SSlider;

// Button widget
typedef struct {
    SShapeProps base;
    STextProps text;
    SWidgetState state;
    SCallback callback;
    void* userData;
} SButton;

// Toggle/Checkbox widget
typedef struct {
    SShapeProps base;
    STextProps text;
    bool checked;
    bool* checkedPtr;  // Pointer to store checked state
    SWidgetState state;
    SCallback callback;
    void* userData;
} SToggle;

// Menu item
typedef struct SMenuItem {
    const char* text;
    SCallback callback;
    void* userData;
    struct SMenuItem* subItems;
    int subItemCount;
    bool enabled;
} SMenuItem;

// Menu bar
typedef struct {
    SShapeProps base;
    SMenuItem* items;
    int itemCount;
    int activeMenu;
    int hoveredItem;
    bool expanded;
} SMenuBar;

// Progress bar
typedef struct {
    SShapeProps base;
    SShapeProps fill;
    float min;
    float max;
    float value;
    float* valuePtr;  // Pointer to store value
    bool showText;
} SProgressBar;

// Input text field
typedef struct {
    SShapeProps base;
    char* buffer;
    int bufferSize;
    int cursorPos;
    int selectionStart;
    int selectionEnd;
    SWidgetState state;
    SCallback callback;
    void* userData;
} STextField;

// Init functions
SSlider SCreateSlider(float x, float y, float width, float height, float min, float max, float value);
SButton SCreateButton(float x, float y, float width, float height, const char* text);
SToggle SCreateToggle(float x, float y, float width, float height, const char* text, bool checked);
SMenuBar SCreateMenuBar(float x, float y, float width, float height);
SProgressBar SCreateProgressBar(float x, float y, float width, float height, float min, float max, float value);
STextField SCreateTextField(float x, float y, float width, float height, char* buffer, int bufferSize);

// Helper functions
SWidgetState SCreateWidgetState(void);
STextProps SCreateTextProps(const char* text, float fontSize, SColor color);
SMenuItem SCreateMenuItem(const char* text, SCallback callback, void* userData);

// Draw functions
void SDrawSlider(SApplication* app, SSlider* slider);
void SDrawButton(SApplication* app, SButton* button);
void SDrawToggle(SApplication* app, SToggle* toggle);
void SDrawMenuBar(SApplication* app, SMenuBar* menuBar);
void SDrawProgressBar(SApplication* app, SProgressBar* progressBar);
void SDrawTextField(SApplication* app, STextField* textField);

// Update functions (for processing input)
bool SUpdateSlider(SApplication* app, SSlider* slider, float mouseX, float mouseY, bool mouseDown);
bool SUpdateButton(SApplication* app, SButton* button, float mouseX, float mouseY, bool mouseDown);
bool SUpdateToggle(SApplication* app, SToggle* toggle, float mouseX, float mouseY, bool mouseDown);
bool SUpdateMenuBar(SApplication* app, SMenuBar* menuBar, float mouseX, float mouseY, bool mouseDown);
bool SUpdateTextField(SApplication* app, STextField* textField, float mouseX, float mouseY, bool mouseDown, int key);

// Implementation

// Helper function to check if a point is inside a rectangle
static bool SPointInRect(float x, float y, float rectX, float rectY, float rectWidth, float rectHeight) {
    return (x >= rectX - rectWidth/2) && (x <= rectX + rectWidth/2) &&
           (y >= rectY - rectHeight/2) && (y <= rectY + rectHeight/2);
}

// Create a default widget state
SWidgetState SCreateWidgetState(void) {
    SWidgetState state = {false, false, false, false};
    return state;
}

// Create text properties
STextProps SCreateTextProps(const char* text, float fontSize, SColor color) {
    STextProps props = {text, fontSize, color};
    return props;
}

// Create a menu item
SMenuItem SCreateMenuItem(const char* text, SCallback callback, void* userData) {
    SMenuItem item = {text, callback, userData, NULL, 0, true};
    return item;
}

// Create a slider widget
SSlider SCreateSlider(float x, float y, float width, float height, float min, float max, float value) {
    SSlider slider;
    
    // Set base properties
    slider.base.x = x;
    slider.base.y = y;
    slider.base.width = width;
    slider.base.height = height;
    slider.base.rotation = 0.0f;
    slider.base.color = SCreateColor(0.2f, 0.2f, 0.2f, 1.0f); // Dark gray
    
    // Set handle properties
    slider.handle.width = height;
    slider.handle.height = height;
    slider.handle.rotation = 0.0f;
    slider.handle.color = SCreateColor(0.8f, 0.8f, 0.8f, 1.0f); // Light gray
    
    // Calculate the handle position based on value
    float ratio = (value - min) / (max - min);
    float handleX = x - (width / 2) + (ratio * width);
    slider.handle.x = handleX;
    slider.handle.y = y;
    
    // Set other properties
    slider.min = min;
    slider.max = max;
    slider.value = value;
    slider.valuePtr = NULL;
    slider.vertical = false;
    slider.dragging = false;
    slider.callback = NULL;
    slider.userData = NULL;
    
    return slider;
}

// Create a button widget
SButton SCreateButton(float x, float y, float width, float height, const char* text) {
    SButton button;
    
    // Set base properties
    button.base.x = x;
    button.base.y = y;
    button.base.width = width;
    button.base.height = height;
    button.base.rotation = 0.0f;
    button.base.color = SCreateColor(0.3f, 0.3f, 0.6f, 1.0f); // Blue-ish
    
    // Set text properties
    button.text.text = text;
    button.text.fontSize = height * 0.6f;
    button.text.color = SCreateColor(1.0f, 1.0f, 1.0f, 1.0f); // White
    
    // Initialize state
    button.state = SCreateWidgetState();
    button.callback = NULL;
    button.userData = NULL;
    
    return button;
}

// Create a toggle/checkbox widget
SToggle SCreateToggle(float x, float y, float width, float height, const char* text, bool checked) {
    SToggle toggle;
    
    // Set base properties
    toggle.base.x = x;
    toggle.base.y = y;
    toggle.base.width = width;
    toggle.base.height = height;
    toggle.base.rotation = 0.0f;
    toggle.base.color = SCreateColor(0.2f, 0.2f, 0.2f, 1.0f); // Dark gray
    
    // Set text properties
    toggle.text.text = text;
    toggle.text.fontSize = height * 0.6f;
    toggle.text.color = SCreateColor(1.0f, 1.0f, 1.0f, 1.0f); // White
    
    // Set toggle state
    toggle.checked = checked;
    toggle.checkedPtr = NULL;
    
    // Initialize state
    toggle.state = SCreateWidgetState();
    toggle.callback = NULL;
    toggle.userData = NULL;
    
    return toggle;
}

// Create a menu bar widget
SMenuBar SCreateMenuBar(float x, float y, float width, float height) {
    SMenuBar menuBar;
    
    // Set base properties
    menuBar.base.x = x;
    menuBar.base.y = y;
    menuBar.base.width = width;
    menuBar.base.height = height;
    menuBar.base.rotation = 0.0f;
    menuBar.base.color = SCreateColor(0.3f, 0.3f, 0.3f, 1.0f); // Dark gray
    
    // Initialize menu items
    menuBar.items = NULL;
    menuBar.itemCount = 0;
    menuBar.activeMenu = -1;
    menuBar.hoveredItem = -1;
    menuBar.expanded = false;
    
    return menuBar;
}

// Create a progress bar widget
SProgressBar SCreateProgressBar(float x, float y, float width, float height, float min, float max, float value) {
    SProgressBar progressBar;
    
    // Set base properties
    progressBar.base.x = x;
    progressBar.base.y = y;
    progressBar.base.width = width;
    progressBar.base.height = height;
    progressBar.base.rotation = 0.0f;
    progressBar.base.color = SCreateColor(0.2f, 0.2f, 0.2f, 1.0f); // Dark gray
    
    // Set fill properties
    progressBar.fill.y = y;
    progressBar.fill.height = height;
    progressBar.fill.rotation = 0.0f;
    progressBar.fill.color = SCreateColor(0.2f, 0.6f, 0.2f, 1.0f); // Green
    
    // Calculate the fill width based on value
    float ratio = (value - min) / (max - min);
    float fillWidth = ratio * width;
    progressBar.fill.width = fillWidth;
    
    // Calculate the fill x position
    float fillX = x - (width / 2) + (fillWidth / 2);
    progressBar.fill.x = fillX;
    
    // Set other properties
    progressBar.min = min;
    progressBar.max = max;
    progressBar.value = value;
    progressBar.valuePtr = NULL;
    progressBar.showText = true;
    
    return progressBar;
}

// Create a text field widget
STextField SCreateTextField(float x, float y, float width, float height, char* buffer, int bufferSize) {
    STextField textField;
    
    // Set base properties
    textField.base.x = x;
    textField.base.y = y;
    textField.base.width = width;
    textField.base.height = height;
    textField.base.rotation = 0.0f;
    textField.base.color = SCreateColor(1.0f, 1.0f, 1.0f, 1.0f); // White
    
    // Set text properties
    textField.buffer = buffer;
    textField.bufferSize = bufferSize;
    textField.cursorPos = 0;
    textField.selectionStart = -1;
    textField.selectionEnd = -1;
    
    // Initialize state
    textField.state = SCreateWidgetState();
    textField.callback = NULL;
    textField.userData = NULL;
    
    return textField;
}


// Draw a slider widget
void SDrawSlider(SApplication* app, SSlider* slider) {
    // Draw the track
    SRectangle(app, &slider->base);
    
    // Draw the handle
    SRectangle(app, &slider->handle);
}

// Draw a button widget
void SDrawButton(SApplication* app, SButton* button) {
    // Adjust color based on state
    SColor originalColor = button->base.color;
    
    if (button->state.hover) {
        // Lighten color when hovered
        button->base.color.r = fminf(originalColor.r * 1.2f, 1.0f);
        button->base.color.g = fminf(originalColor.g * 1.2f, 1.0f);
        button->base.color.b = fminf(originalColor.b * 1.2f, 1.0f);
    }
    
    if (button->state.clicked) {
        // Darken color when clicked
        button->base.color.r = originalColor.r * 0.8f;
        button->base.color.g = originalColor.g * 0.8f;
        button->base.color.b = originalColor.b * 0.8f;
    }
    
    // Draw the button background
    SRectangle(app, &button->base);
    
    // Restore original color
    button->base.color = originalColor;
    
     if (button->text.text != NULL) {
        // Calculate text position (centered in the button)
        // Get approximate text width (assuming monospace font where each char is 8 pixels wide)
        float textWidth = strlen(button->text.text) * button->text.fontSize;
        
        // Center text horizontally and vertically
        float textX = button->base.x - textWidth/2;
        float textY = button->base.y - button->text.fontSize/2;
        
        // Draw the text
        SDrawText(app, button->text.text, textX, textY, button->text.fontSize/8.0f, 
                 button->text.color.r, button->text.color.g, button->text.color.b);
    }
}

    
}

// Draw a toggle/checkbox widget
void SDrawToggle(SApplication* app, SToggle* toggle) {
    // Draw the toggle background
    SRectangle(app, &toggle->base);
    
    // Draw the checkmark if checked
    if (toggle->checked) {
        // Create a smaller rectangle for the checkmark
        SShapeProps checkmark = toggle->base;
        checkmark.width = toggle->base.height * 0.6f;
        checkmark.height = toggle->base.height * 0.6f;
        checkmark.color = SCreateColor(1.0f, 1.0f, 1.0f, 1.0f); // White
        
        SRectangle(app, &checkmark);
    }
    
    if (toggle->text.text != NULL) {
        // Position text to the right of the toggle box
        float textX = toggle->base.x + toggle->base.width/2 + toggle->text.fontSize/2;
        float textY = toggle->base.y - toggle->text.fontSize/2;
        
        // Draw the text
        SDrawText(app, toggle->text.text, textX, textY, toggle->text.fontSize/8.0f,
                 toggle->text.color.r, toggle->text.color.g, toggle->text.color.b);
    }
    
}


void SDrawMenuBar(SApplication* app, SMenuBar* menuBar) {
    // Draw the menu bar background
    SRectangle(app, &menuBar->base);
    
    // Draw menu items
    if (menuBar->items != NULL) {
        float itemWidth = menuBar->base.width / menuBar->itemCount;
        float startX = menuBar->base.x - (menuBar->base.width / 2) + (itemWidth / 2);
        float fontSize = menuBar->base.height * 0.6f;
        
        for (int i = 0; i < menuBar->itemCount; i++) {
            // Create a shape for the menu item
            SShapeProps itemShape = {
                startX + (i * itemWidth),
                menuBar->base.y,
                itemWidth,
                menuBar->base.height,
                0.0f,
                menuBar->base.color
            };
            
            // Highlight active or hovered item
            if (i == menuBar->activeMenu || i == menuBar->hoveredItem) {
                itemShape.color.r += 0.1f;
                itemShape.color.g += 0.1f;
                itemShape.color.b += 0.1f;
            }
            
            SRectangle(app, &itemShape);
            
            // Draw menu item text
            if (menuBar->items[i].text != NULL) {
                float textX = startX + (i * itemWidth) - (strlen(menuBar->items[i].text) * fontSize)/2;
                float textY = menuBar->base.y - fontSize/2;
                
                SDrawText(app, menuBar->items[i].text, textX, textY, fontSize/8.0f, 
                         1.0f, 1.0f, 1.0f); // White text
            }
        }
        
        // Draw dropdown menu if active
        if (menuBar->expanded && menuBar->activeMenu >= 0 && menuBar->activeMenu < menuBar->itemCount) {
            SMenuItem* activeItem = &menuBar->items[menuBar->activeMenu];
            
            if (activeItem->subItemCount > 0) {
                float dropdownX = startX + (menuBar->activeMenu * itemWidth);
                float dropdownY = menuBar->base.y + menuBar->base.height / 2 + activeItem->subItemCount * menuBar->base.height / 2;
                
                // Draw dropdown background
                SShapeProps dropdownShape = {
                    dropdownX,
                    dropdownY,
                    itemWidth,
                    activeItem->subItemCount * menuBar->base.height,
                    0.0f,
                    SCreateColor(0.25f, 0.25f, 0.25f, 1.0f) // Slightly darker
                };
                
                SRectangle(app, &dropdownShape);
                
                // Draw submenu items
                for (int j = 0; j < activeItem->subItemCount; j++) {
                    SShapeProps subItemShape = {
                        dropdownX,
                        menuBar->base.y + menuBar->base.height + (j * menuBar->base.height) + menuBar->base.height / 2,
                        itemWidth,
                        menuBar->base.height,
                        0.0f,
                        dropdownShape.color
                    };
                    
                    SRectangle(app, &subItemShape);
                    
                    // Draw submenu text
                    if (activeItem->subItems[j].text != NULL) {
                        float subTextX = dropdownX - (strlen(activeItem->subItems[j].text) * fontSize)/2;
                        float subTextY = menuBar->base.y + menuBar->base.height + (j * menuBar->base.height) + menuBar->base.height/2 - fontSize/2;
                        
                        SDrawText(app, activeItem->subItems[j].text, subTextX, subTextY, fontSize/8.0f,
                                 1.0f, 1.0f, 1.0f); // White text
                    }
                }
            }
        }
    }
}

void SDrawProgressBar(SApplication* app, SProgressBar* progressBar) {
    // Draw the progress bar background
    SRectangle(app, &progressBar->base);
    
    // Draw the fill
    SRectangle(app, &progressBar->fill);
    
    // Draw progress text if enabled
    if (progressBar->showText) {
        // Create percentage text
        char percentText[8];
        int percentage = (int)((progressBar->value - progressBar->min) / 
                               (progressBar->max - progressBar->min) * 100);
        sprintf(percentText, "%d%%", percentage);
        
        float fontSize = progressBar->base.height * 0.7f;
        float textX = progressBar->base.x - (strlen(percentText) * fontSize)/2;
        float textY = progressBar->base.y - fontSize/2;
        
        // Draw text
        SDrawText(app, percentText, textX, textY, fontSize/8.0f,
                 1.0f, 1.0f, 1.0f); // White text
    }
}

// Modified text field drawing function to show text and cursor
void SDrawTextField(SApplication* app, STextField* textField) {
    // Draw text field background
    SRectangle(app, &textField->base);
    
    // Draw text
    if (textField->buffer != NULL) {
        float fontSize = textField->base.height * 0.7f;
        float textX = textField->base.x - textField->base.width/2 + fontSize/2; // Left-aligned with margin
        float textY = textField->base.y - fontSize/2;
        
        // Draw text content
        SDrawText(app, textField->buffer, textX, textY, fontSize/8.0f,
                 0.0f, 0.0f, 0.0f); // Black text
        
        // Draw cursor if text field is focused
        if (textField->state.focused) {
            // Calculate cursor position
            float cursorX = textX + textField->cursorPos * fontSize;
            
            // Draw cursor line
            float color[3] = {0.0f, 0.0f, 0.0f}; // Black
            SDrawRectangle(app, color, cursorX, textField->base.y, 
                          1.0f, textField->base.height * 0.8f);
        }
    }
}


// Update a slider widget
bool SUpdateSlider(SApplication* app, SSlider* slider, float mouseX, float mouseY, bool mouseDown) {
    bool valueChanged = false;
    
    // Check if mouse is over the handle or track
    bool mouseOverHandle = SPointInRect(mouseX, mouseY, slider->handle.x, slider->handle.y, 
                                       slider->handle.width, slider->handle.height);
    bool mouseOverTrack = SPointInRect(mouseX, mouseY, slider->base.x, slider->base.y, 
                                      slider->base.width, slider->base.height);
    
    // Handle dragging
    if (slider->dragging) {
        if (mouseDown) {
            // Continue dragging
            float trackLeft = slider->base.x - (slider->base.width / 2);
            float trackRight = slider->base.x + (slider->base.width / 2);
            float handleX = mouseX;
            
            // Constrain to track bounds
            if (handleX < trackLeft) handleX = trackLeft;
            if (handleX > trackRight) handleX = trackRight;
            
            // Calculate new value based on handle position
            float ratio = (handleX - trackLeft) / slider->base.width;
            float newValue = slider->min + ratio * (slider->max - slider->min);
            
            // Update value if it changed
            if (newValue != slider->value) {
                slider->value = newValue;
                valueChanged = true;
                
                // Update handle position
                slider->handle.x = handleX;
                
                // Update external value if pointer provided
                if (slider->valuePtr != NULL) {
                    *slider->valuePtr = newValue;
                }
                
                // Call callback if provided
                if (slider->callback != NULL) {
                    slider->callback(slider->userData);
                }
            }
        } else {
            // Stop dragging when mouse released
            slider->dragging = false;
        }
    } else if (mouseDown && (mouseOverHandle || mouseOverTrack)) {
        // Start dragging
        slider->dragging = true;
        
        // If clicked on track, jump handle to that position
        if (mouseOverTrack && !mouseOverHandle) {
            float trackLeft = slider->base.x - (slider->base.width / 2);
            float ratio = (mouseX - trackLeft) / slider->base.width;
            float newValue = slider->min + ratio * (slider->max - slider->min);
            
            // Update value
            slider->value = newValue;
            valueChanged = true;
            
            // Update handle position
            slider->handle.x = mouseX;
            
            // Update external value if pointer provided
            if (slider->valuePtr != NULL) {
                *slider->valuePtr = newValue;
            }
            
            // Call callback if provided
            if (slider->callback != NULL) {
                slider->callback(slider->userData);
            }
        }
    }
    
    return valueChanged;
}

// Update a button widget
bool SUpdateButton(SApplication* app, SButton* button, float mouseX, float mouseY, bool mouseDown) {
    bool clicked = false;
    
    // Check if mouse is over the button
    bool mouseOver = SPointInRect(mouseX, mouseY, button->base.x, button->base.y, 
                                 button->base.width, button->base.height);
    
    // Update hover state
    button->state.hover = mouseOver;
    
    // Handle click
    if (mouseOver && mouseDown) {
        button->state.clicked = true;
    } else if (button->state.clicked && !mouseDown) {
        // Button was released while hovering - trigger callback
        if (mouseOver) {
            clicked = true;
            
            // Call callback if provided
            if (button->callback != NULL) {
                button->callback(button->userData);
            }
        }
        
        button->state.clicked = false;
    }
    
    return clicked;
}

// Update a toggle/checkbox widget
bool SUpdateToggle(SApplication* app, SToggle* toggle, float mouseX, float mouseY, bool mouseDown) {
    bool changed = false;
    
    // Check if mouse is over the toggle
    bool mouseOver = SPointInRect(mouseX, mouseY, toggle->base.x, toggle->base.y, 
                                 toggle->base.width, toggle->base.height);
    
    // Update hover state
    toggle->state.hover = mouseOver;
    
    // Handle click
    if (mouseOver && mouseDown) {
        toggle->state.clicked = true;
    } else if (toggle->state.clicked && !mouseDown) {
        // Toggle was released while hovering - change state
        if (mouseOver) {
            // Toggle checked state
            toggle->checked = !toggle->checked;
            changed = true;
            
            // Update external state if pointer provided
            if (toggle->checkedPtr != NULL) {
                *toggle->checkedPtr = toggle->checked;
            }
            
            // Call callback if provided
            if (toggle->callback != NULL) {
                toggle->callback(toggle->userData);
            }
        }
        
        toggle->state.clicked = false;
    }
    
    return changed;
}

// Update a menu bar widget
bool SUpdateMenuBar(SApplication* app, SMenuBar* menuBar, float mouseX, float mouseY, bool mouseDown) {
    bool changed = false;
    
    // Check if mouse is over the menu bar
    bool mouseOverBar = SPointInRect(mouseX, mouseY, menuBar->base.x, menuBar->base.y, 
                                    menuBar->base.width, menuBar->base.height);
    
    // Handle menu item hovering and clicks
    if (menuBar->items != NULL && menuBar->itemCount > 0) {
        float itemWidth = menuBar->base.width / menuBar->itemCount;
        float startX = menuBar->base.x - (menuBar->base.width / 2);
        
        // Determine which item is hovered
        int hoveredItem = -1;
        
        if (mouseOverBar) {
            float relativeX = mouseX - startX;
            hoveredItem = (int)(relativeX / itemWidth);
            
            // Check bounds
            if (hoveredItem < 0 || hoveredItem >= menuBar->itemCount) {
                hoveredItem = -1;
            }
        }
        
        // Update hovered item
        if (menuBar->hoveredItem != hoveredItem) {
            menuBar->hoveredItem = hoveredItem;
            changed = true;
        }
        
        // Handle clicks on menu items
        if (mouseDown && mouseOverBar && hoveredItem >= 0) {
            if (menuBar->activeMenu == hoveredItem && menuBar->expanded) {
                // Clicking the same menu item again - close it
                menuBar->activeMenu = -1;
                menuBar->expanded = false;
            } else {
                // Open a new menu item
                menuBar->activeMenu = hoveredItem;
                menuBar->expanded = true;
            }
            changed = true;
        } else if (mouseDown && !mouseOverBar) {
            // Clicking outside the menu bar - close any open menus
            if (menuBar->expanded) {
                // TODO: Check if clicking on a submenu item and handle accordingly
                
                // For now, just close the menu
                menuBar->activeMenu = -1;
                menuBar->expanded = false;
                changed = true;
            }
        }
    }
    
    return changed;
}

// Update a text field widget
bool SUpdateTextField(SApplication* app, STextField* textField, float mouseX, float mouseY, bool mouseDown, int key) {
    bool changed = false;
    
    // Check if mouse is over the text field
    bool mouseOver = SPointInRect(mouseX, mouseY, textField->base.x, textField->base.y, 
                                 textField->base.width, textField->base.height);
    
    // Update hover state
    textField->state.hover = mouseOver;
    
    // Handle click
    if (mouseOver && mouseDown) {
        // Set focus to this text field
        textField->state.focused = true;
        
        // TODO: Update cursor position based on click position
        // This would require text measurement functions
        
        changed = true;
    } else if (mouseDown && !mouseOver) {
        // Clicking outside - lose focus
        if (textField->state.focused) {
            textField->state.focused = false;
            changed = true;
        }
    }
    
    // Handle key input when focused
    if (textField->state.focused && key != 0) {
        // Handle special keys
        switch (key) {
            case '\b': // Backspace
                if (textField->cursorPos > 0) {
                    // Remove character before cursor
                    for (int i = textField->cursorPos - 1; i < strlen(textField->buffer); i++) {
                        textField->buffer[i] = textField->buffer[i + 1];
                    }
                    textField->cursorPos--;
                    changed = true;
                }
                break;
                
            case 127: // Delete
                if (textField->cursorPos < strlen(textField->buffer)) {
                    // Remove character at cursor
                    for (int i = textField->cursorPos; i < strlen(textField->buffer); i++) {
                        textField->buffer[i] = textField->buffer[i + 1];
                    }
                    changed = true;
                }
                break;
                
            default:
                if (key >= 32 && key <= 126) { // Printable ASCII characters
                    int length = strlen(textField->buffer);
                    

                    if (length + 1 < textField->bufferSize) {
      
                        for (int i = length; i > textField->cursorPos; i--) {
                            textField->buffer[i] = textField->buffer[i - 1];
                        }
                        
            
                        textField->buffer[textField->cursorPos] = (char)key;
                        textField->cursorPos++;
                        changed = true;
                    }
                }
                break;
        }
        

        if (changed && textField->callback != NULL) {
            textField->callback(textField->userData);
        }
    }
    
    return changed;
}


#endif // WIDGETS_H
