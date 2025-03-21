// Global stuff declared here.
#include <stdio.h>
#include <stdbool.h>

#if defined(__linux__)
    #pragma comment(lib, "X11")
    #pragma comment(lib, "GL")
#elif defined(_WIN32) || defined(_WIN64)
    #pragma comment(lib, "opengl32.lib")
    #pragma comment(lib, "gdi32.lib")
    #pragma comment(lib, "user32.lib")
#endif

#if defined(__linux__)

#define GL_VERSION_3_3

#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <GL/glx.h>
#include <GL/glxext.h>
#include <GL/gl.h>


typedef struct {
    Display *display;
    int screen;
    Window window;
    XEvent event;
    GLXContext glx_context;  
    Colormap colormap;
    float mouseX;
    float mouseY;
    int mouseDown;
} SApplication;

// Forward declarations from other files (widget.h and image.h)
bool initText(const char* fontPath);



#ifdef IMAGE_H
extern ImageRenderer* imageRenderer;
void renderImage(ImageRenderer* renderer);
#endif


//Implementation of funcs.
int SDisplayOpen(SApplication *app) {
    if (app == NULL) {
        return 0;
    }
    app->display = XOpenDisplay(NULL);
    if (app->display == NULL) {
        fprintf(stderr, "ERROR: Unable to open X11 display.\n");
        return 0;
    }
    #ifdef STDUI_VERBAL_DEBUG
    printf("STATUS: Opened Display with code 0. \n");
    #endif
    
    app->screen = DefaultScreen(app->display);
    return 1;
}

int SWindowCreate(SApplication *app, const char *title, int x, int y, int width, int height) {
    if (app == NULL) {
        return 0;
    }
    
    int context_attribs[] = {
        GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
        GLX_CONTEXT_MINOR_VERSION_ARB, 3,
        GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
        None
    };
    
    // For framebuffer configuration (Copied from WIKI)
    static int visual_attribs[] = {
        GLX_X_RENDERABLE    , True,
        GLX_DRAWABLE_TYPE   , GLX_WINDOW_BIT,
        GLX_RENDER_TYPE     , GLX_RGBA_BIT,
        GLX_X_VISUAL_TYPE   , GLX_TRUE_COLOR,
        GLX_RED_SIZE        , 8,
        GLX_GREEN_SIZE      , 8,
        GLX_BLUE_SIZE       , 8,
        GLX_ALPHA_SIZE      , 8,
        GLX_DEPTH_SIZE      , 24,
        GLX_STENCIL_SIZE    , 8,
        GLX_DOUBLEBUFFER    , True,
        None
    };
    
    // Get framebuffer configs that match our criteria Note: No idea what this is.
    int fbcount;
    GLXFBConfig* fbc = glXChooseFBConfig(app->display, app->screen, visual_attribs, &fbcount);
    if (!fbc) {
        fprintf(stderr, "ERROR: Failed to retrieve framebuffer config\n");
        return 0;
    }
    
    // Find the best config.
    int best_fbc = -1, worst_fbc = -1, best_num_samp = -1, worst_num_samp = 999;
    for (int i = 0; i < fbcount; ++i) {
        XVisualInfo *vi = glXGetVisualFromFBConfig(app->display, fbc[i]);
        if (vi) {
            int samp_buf, samples;
            glXGetFBConfigAttrib(app->display, fbc[i], GLX_SAMPLE_BUFFERS, &samp_buf);
            glXGetFBConfigAttrib(app->display, fbc[i], GLX_SAMPLES, &samples);
            
            if (best_fbc < 0 || (samp_buf && samples > best_num_samp)) {
                best_fbc = i;
                best_num_samp = samples;
            }
            if (worst_fbc < 0 || !samp_buf || samples < worst_num_samp) {
                worst_fbc = i;
                worst_num_samp = samples;
            }
            XFree(vi);
        }
    }
    
    GLXFBConfig bestFbc = fbc[best_fbc];
    XFree(fbc);
    
    // Get a visual.
    XVisualInfo *vi = glXGetVisualFromFBConfig(app->display, bestFbc);
    
    // Create colormap.
    app->colormap = XCreateColormap(app->display, RootWindow(app->display, app->screen), 
                                    vi->visual, AllocNone);
    
    // Window attribs.
    XSetWindowAttributes swa;
    swa.colormap = app->colormap;
    swa.event_mask = ExposureMask | KeyPressMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask; //KeyPress, Mouse and Exposure.
    
    // Create window.
    app->window = XCreateWindow(app->display, RootWindow(app->display, app->screen), 
                                x, y, width, height, 0, vi->depth, InputOutput, 
                                vi->visual, CWColormap | CWEventMask, &swa);
    
    // Set window title.
    XStoreName(app->display, app->window, title);
    
    // Get the function to create OpenGL 3.3 context. Note: Who made these names? They are stupid.
    PFNGLXCREATECONTEXTATTRIBSARBPROC glXCreateContextAttribsARB = (PFNGLXCREATECONTEXTATTRIBSARBPROC) glXGetProcAddress((const GLubyte*)"glXCreateContextAttribsARB");
    
    if (!glXCreateContextAttribsARB) {
        fprintf(stderr, "ERROR: glXCreateContextAttribsARB() not found\n");
        app->glx_context = glXCreateNewContext(app->display, bestFbc, GLX_RGBA_TYPE, NULL, True);
    } else {
        app->glx_context = glXCreateContextAttribsARB(app->display, bestFbc, NULL, True, context_attribs);
    }
    
    // Verify context.
    if (!app->glx_context) {
        fprintf(stderr, "ERROR: Failed to create OpenGL context.\n");
        XFree(vi);
        return 0;
    }
    
    // Make context current.
    if (!glXMakeCurrent(app->display, app->window, app->glx_context)) {
        fprintf(stderr, "ERROR: Failed to make context current.\n");
        XFree(vi);
        return 0;
    }
    
    // Map window.
    XMapWindow(app->display, app->window);
    
    
    const char* version = (const char*)glGetString(GL_VERSION);
    const char* shaderVersion = (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION);

    // Enable blending for text.
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Initialize text rendering.
    if (!initText("stdui/internal/courier_new.ttf")) {
        fprintf(stderr, "ERROR: Failed to initialize text rendering.\n");
        XFree(vi);
        return 0;
    }

    #ifdef STDUI_VERBAL_DEBUG
    printf("STATUS: Window created with code 0: \n OpenGL version: %s\n GLSL version: %s \n", version, shaderVersion);
    #endif
    
    XFree(vi);
    XFlush(app->display);
    return 1;
}


void SGetMouseState(SApplication *app) {
    if (app == NULL) {
        return;
    }
    
    Window root_return, child_return;
    int root_x_return, root_y_return;
    int win_x_return, win_y_return;
    unsigned int mask_return;
    
    XQueryPointer(app->display, app->window, 
                 &root_return, &child_return,
                 &root_x_return, &root_y_return,
                 &win_x_return, &win_y_return,
                 &mask_return);
    
    // Update mouse position.
    app->mouseX = (float)win_x_return;
    app->mouseY = (float)win_y_return;
    
    // Update mouse button state.
    app->mouseDown = (mask_return & Button1Mask) ? 1 : 0;
}


int SEventProcess(SApplication *app) {
    if (app == NULL) {
        return 0;
    }

    #ifdef STDUI_VERBAL_DEBUG
    printf("STATUS: Started processing events.");
    #endif
    
    if (XPending(app->display) > 0) {
        XNextEvent(app->display, &app->event);
        switch (app->event.type) {
            case Expose:
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                break;
            case KeyPress:
                if (XLookupKeysym(&app->event.xkey, 0) == XK_Escape) {
                    return 0;
                }
                break;
            case ButtonPress: //Button down. (mouse)
                if (app->event.xbutton.button == Button1) {
                    app->mouseDown = 1;
                    app->mouseX = (float)app->event.xbutton.x;
                    app->mouseY = (float)app->event.xbutton.y;
                }
                break;
            case ButtonRelease: //Button up.  (mouse)
                if (app->event.xbutton.button == Button1) {
                    app->mouseDown = 0;
                }
                break;
            case MotionNotify:
                app->mouseX = (float)app->event.xmotion.x;
                app->mouseY = (float)app->event.xmotion.y;
                break;
        }
    }
    
    return 1;
}


void SDisplayClose(SApplication *app) {
    if (app == NULL) {
        return;
    }
    
 
    if (app->glx_context) {
        glXMakeCurrent(app->display, None, NULL);
        glXDestroyContext(app->display, app->glx_context);
    }
    

    if (app->colormap) {
        XFreeColormap(app->display, app->colormap);
    }
    

    XDestroyWindow(app->display, app->window);
    XCloseDisplay(app->display);
}

static inline void SClearScreen(SApplication *app, float r, float g, float b) {
    glClearColor(r, g, b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Reset view matrix.
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}


static inline int SGetCurrentWindowWidth(SApplication *app) {
    if (app == NULL || app->display == NULL) {
        return -1;
    }
    
    int x, y; // Unused right now.        
    unsigned int width, height, borderWidth, depth;
    Window root_return;
    
    if (XGetGeometry(app->display, app->window, &root_return, &x, &y, &width, &height, &borderWidth, &depth)) { //Notice: Have had problems with this before.
        return width; 
    } else {
        fprintf(stderr, "Error: Could not retrieve window geometry.\n");
        return -1; 
    }
}


static inline int SGetCurrentWindowHeight(SApplication *app) {
    if (app == NULL || app->display == NULL) {
        return -1;
    }
    
    int x, y;                
    unsigned int width, height, borderWidth, depth;
    Window root_return;

    if (XGetGeometry(app->display, app->window, &root_return, &x, &y, &width, &height, &borderWidth, &depth)) { //Notice: Have had problems with this before.
        return height;
    } else {
        fprintf(stderr, "Error: Could not retrieve window geometry.\n");
        return -1; 
    }
}

void SUpdateViewport(SApplication *app, int width, int height) {
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, height, 0, -1, 1); // Origin at top-left.
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

static inline void SBeginFrame(SApplication *app) {

    #ifdef STDUI_VERBAL_DEBUG
    printf("STATUS: Entering Rendering Loop \n");
    #endif
    
    int width = SGetCurrentWindowWidth(app);
    int height = SGetCurrentWindowHeight(app);
    
    if (width > 0 && height > 0) {
        SUpdateViewport(app, width, height);
    }
    
    SGetMouseState(app);
        
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    #ifdef IMAGE_H
    if (imageRenderer) {
        renderImage(imageRenderer);
    }
    #endif
}

static inline void SEndFrame(SApplication *app) {
    if (app == NULL || app->display == NULL) {
        return;
    }
    
    glXSwapBuffers(app->display, app->window);
}

#elif defined(_WIN32) || defined(_WIN64) 
#include <windows.h>
#include <GL/gl.h>
//THIS windows code was written with an LLM. No idea what it does. (I hate WIN32 API.)

// Window procedure function prototype
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

typedef struct {
    HINSTANCE hinstance;
    HWND hwnd;
    HDC hdc;
    HGLRC hglrc;
    MSG msg;
    float mouseX;
    float mouseY;
    int mouseDown;
} SApplication;

// Forward declarations
bool initText(const char* fontPath);
void SUpdateViewport(SApplication *app, int width, int height);

#ifdef IMAGE_H
extern ImageRenderer* imageRenderer;
void renderImage(ImageRenderer* renderer);
#endif

int SDisplayOpen(SApplication *app) {
    if (app == NULL) {
        return 0;
    }
    
    app->hinstance = GetModuleHandle(NULL);
    
    // Register the window class
    const char CLASS_NAME[] = "OpenGLWindowClass";
    
    WNDCLASS wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = app->hinstance;
    wc.lpszClassName = CLASS_NAME;
    wc.style = CS_OWNDC;
    
    if (!RegisterClass(&wc)) {
        fprintf(stderr, "Failed to register window class.\n");
        return 0;
    }
    
    return 1;
}

int SWindowCreate(SApplication *app, const char *title, int x, int y, int width, int height) {
    if (app == NULL) {
        return 0;
    }
    
    // Initialize mouse state
    app->mouseX = 0.0f;
    app->mouseY = 0.0f;
    app->mouseDown = 0;

    app->hwnd = CreateWindowEx(
        0,                          // Optional window styles
        "OpenGLWindowClass",        // Window class
        title,                      // Window text
        WS_OVERLAPPEDWINDOW,        // Window style
        x, y,                       // Position
        width, height,              // Size
        NULL,                       // Parent window    
        NULL,                       // Menu
        app->hinstance,             // Instance handle
        app                         // Additional application data - pass app pointer
    );
    
    if (app->hwnd == NULL) {
        fprintf(stderr, "Failed to create window.\n");
        return 0;
    }
    

    app->hdc = GetDC(app->hwnd);
    

    PIXELFORMATDESCRIPTOR pfd = {
        sizeof(PIXELFORMATDESCRIPTOR),
        1,                              // Version
        PFD_DRAW_TO_WINDOW |           // Support window
        PFD_SUPPORT_OPENGL |           // Support OpenGL
        PFD_DOUBLEBUFFER,              // Double buffered
        PFD_TYPE_RGBA,                 // RGBA type
        24,                            // 24-bit color depth
        0, 0, 0, 0, 0, 0,              // Color bits ignored
        0,                             // No alpha buffer
        0,                             // Shift bit ignored
        0,                             // No accumulation buffer
        0, 0, 0, 0,                    // Accumulation bits ignored
        16,                            // 16-bit z-buffer
        0,                             // No stencil buffer
        0,                             // No auxiliary buffer
        PFD_MAIN_PLANE,                // Main drawing layer
        0,                             // Reserved
        0, 0, 0                        // Layer masks ignored
    };
    
    int pixelFormat = ChoosePixelFormat(app->hdc, &pfd);
    if (pixelFormat == 0) {
        fprintf(stderr, "Failed to choose pixel format.\n");
        return 0;
    }
    
    if (!SetPixelFormat(app->hdc, pixelFormat, &pfd)) {
        fprintf(stderr, "Failed to set pixel format.\n");
        return 0;
    }
    

    app->hglrc = wglCreateContext(app->hdc);
    if (app->hglrc == NULL) {
        fprintf(stderr, "Failed to create OpenGL context.\n");
        return 0;
    }
    

    if (!wglMakeCurrent(app->hdc, app->hglrc)) {
        fprintf(stderr, "Failed to make OpenGL context current.\n");
        return 0;
    }
    
    // Enable blending for text
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Initialize text rendering
    if (!initText("stdui/internal/courier_new.ttf")) {
        fprintf(stderr, "ERROR: Failed to initialize text rendering.\n");
        return 0;
    }

    ShowWindow(app->hwnd, SW_SHOW);
    UpdateWindow(app->hwnd);
    
    return 1;
}

void SGetMouseState(SApplication *app) {
    if (app == NULL) {
        return;
    }
    
    POINT point;
    if (GetCursorPos(&point) && ScreenToClient(app->hwnd, &point)) {
        app->mouseX = (float)point.x;
        app->mouseY = (float)point.y;
    }
    
    app->mouseDown = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) ? 1 : 0;
}

int SEventProcess(SApplication *app) {
    if (app == NULL) {
        return 0;
    }
    

    if (PeekMessage(&app->msg, NULL, 0, 0, PM_REMOVE)) {
 
        if (app->msg.message == WM_QUIT) {
            return 0;
        }
        
        TranslateMessage(&app->msg);
        DispatchMessage(&app->msg);
    }
    
    return 1;
}

void SDisplayClose(SApplication *app) {
    if (app == NULL) {
        return;
    }
    

    if (app->hglrc) {
        wglMakeCurrent(NULL, NULL);
        wglDeleteContext(app->hglrc);
    }
    

    if (app->hwnd && app->hdc) {
        ReleaseDC(app->hwnd, app->hdc);
    }
    

    if (app->hwnd) {
        DestroyWindow(app->hwnd);
    }
    
    UnregisterClass("OpenGLWindowClass", app->hinstance);
}

static inline int SGetCurrentWindowWidth(SApplication *app) {
    if (app == NULL || app->hwnd == NULL) {
        return -1;
    }
    
    RECT rect;
    if (GetClientRect(app->hwnd, &rect)) {
        return rect.right - rect.left;
    } else {
        fprintf(stderr, "Error: Could not retrieve window client area.\n");
        return -1;
    }
}

static inline int SGetCurrentWindowHeight(SApplication *app) {
    if (app == NULL || app->hwnd == NULL) {
        return -1;
    }
    
    RECT rect;
    if (GetClientRect(app->hwnd, &rect)) {
        return rect.bottom - rect.top;
    } else {
        fprintf(stderr, "Error: Could not retrieve window client area.\n");
        return -1;
    }
}

void SUpdateViewport(SApplication *app, int width, int height) {
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, height, 0, -1, 1); // Origin at top-left
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

static inline void SClearScreen(SApplication *app, float r, float g, float b) {
    glClearColor(r, g, b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Reset model-view matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

static inline void SBeginFrame(SApplication *app) {
    int width = SGetCurrentWindowWidth(app);
    int height = SGetCurrentWindowHeight(app);
    
    if (width > 0 && height > 0) {
        SUpdateViewport(app, width, height);
    }
    
    SGetMouseState(app);
        
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    #ifdef IMAGE_H
    if (imageRenderer) {
        renderImage(imageRenderer);
    }
    #endif
}

static inline void SEndFrame(SApplication *app) {
    if (app == NULL || app->hdc == NULL) {
        return;
    }
    
    SwapBuffers(app->hdc);
}

// Store a pointer to the application instance to access in WindowProc
SApplication* g_app = NULL;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    // Try to get the app instance from window user data
    SApplication* app = (SApplication*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    
    switch (uMsg) {
        case WM_CREATE: {
            // Save the app pointer passed in CreateWindow
            CREATESTRUCT* createStruct = (CREATESTRUCT*)lParam;
            app = (SApplication*)createStruct->lpCreateParams;
            if (app) {
                SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)app);
                g_app = app; // Backup global pointer
            }
            return 0;
        }
        
        case WM_CLOSE:
            PostQuitMessage(0);
            return 0;
            
        case WM_KEYDOWN:
            if (wParam == VK_ESCAPE) {
                PostQuitMessage(0);
                return 0;
            }
            break;
            
        case WM_LBUTTONDOWN:
            if (app) {
                app->mouseDown = 1;
                app->mouseX = (float)LOWORD(lParam);
                app->mouseY = (float)HIWORD(lParam);
            } else if (g_app) {
                g_app->mouseDown = 1;
                g_app->mouseX = (float)LOWORD(lParam);
                g_app->mouseY = (float)HIWORD(lParam);
            }
            break;
            
        case WM_LBUTTONUP:
            if (app) {
                app->mouseDown = 0;
            } else if (g_app) {
                g_app->mouseDown = 0;
            }
            break;
            
        case WM_MOUSEMOVE:
            if (app) {
                app->mouseX = (float)LOWORD(lParam);
                app->mouseY = (float)HIWORD(lParam);
            } else if (g_app) {
                g_app->mouseX = (float)LOWORD(lParam);
                g_app->mouseY = (float)HIWORD(lParam);
            }
            break;
            
        case WM_PAINT:
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            break;
    }
    
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

#elif defined(__APPLE__) && defined(__MACH__)
#include <ApplicationServices/ApplicationServices.h>
// MacOS implementation would go here
#endif
