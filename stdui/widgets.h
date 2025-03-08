#ifndef WIDGETS_H
#define WIDGETS_H

#include <stdio.h>
#include <math.h>
#include "internal/layout.h"
#include "internal/widgets_ext.h"


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
    float r, g, b, a;
} SColor;


typedef struct {
    float x, y;
    float width, height;
    float rotation;
    SColor color;
} SShapeProps;

float mouseX = 0.0f, mouseY = 0.0f;
bool mouseDown = false;

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
void SDrawRectangle(SApplication *app, float color[3], float posX, float posY, float size);
void SDrawCircle(SApplication *app, float color[3], float posX, float posY, float size);


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
    
    // TODO: Draw text (requires text rendering function)
    // For now, we're just rendering the button background
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
    
    // TODO: Draw text label (requires text rendering function)
}

// Draw a menu bar widget
void SDrawMenuBar(SApplication* app, SMenuBar* menuBar) {
    // Draw the menu bar background
    SRectangle(app, &menuBar->base);
    
    // Draw menu items
    if (menuBar->items != NULL) {
        float itemWidth = menuBar->base.width / menuBar->itemCount;
        float startX = menuBar->base.x - (menuBar->base.width / 2) + (itemWidth / 2);
        
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
            
            // TODO: Draw menu text (requires text rendering function)
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
                    
                    // Highlight hovered subitem (would need additional state tracking)
                    SRectangle(app, &subItemShape);
                    
                    // TODO: Draw submenu text
                }
            }
        }
    }
}

// Draw a progress bar widget
void SDrawProgressBar(SApplication* app, SProgressBar* progressBar) {
    // Draw the background
    SRectangle(app, &progressBar->base);
    
    // Draw the fill
    SRectangle(app, &progressBar->fill);
    
    // TODO: Draw percentage text if showText is true (requires text rendering)
}

// Draw a text field widget
void SDrawTextField(SApplication* app, STextField* textField) {
    // Draw the text field background
    SRectangle(app, &textField->base);
    
    // TODO: Draw text and cursor (requires text rendering function)
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
