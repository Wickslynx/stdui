// Global stuff declared here.
#include <stdio.h>


#if defined(__linux__)
    #pragma comment(lib, "X11")
    #pragma comment(lib, "GL")
#elif defined(_WIN32) || defined(_WIN64)
    #pragma comment(lib, "opengl32.lib")
    #pragma comment(lib, "gdi32.lib")
    #pragma comment(lib, "user32.lib")
#endif




#if defined(__linux__)
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <GL/gl.h>
#include <GL/glx.h>  

typedef struct {
    Display *display;
    int screen;
    Window window;
    XEvent event;
    GLXContext glx_context;  
    Colormap colormap;       
} SApplication;

typedef struct {
    float mouseX;
    float mouseY;
    int mouseDown;
} SMouseState;

int SDisplayOpen(SApplication *app) {
    if (app == NULL) {
        return 0;
    }
    app->display = XOpenDisplay(NULL);
    if (app->display == NULL) {
        fprintf(stderr, "Unable to open X11 display.\n");
        return 0;
    }
    app->screen = DefaultScreen(app->display);
    return 1;
}

int SWindowCreate(SApplication *app, const char *title, int x, int y, int width, int height) {
    if (app == NULL) {
        return 0;
    }
    

    static int visual_attribs[] = {
        GLX_RGBA,
        GLX_DEPTH_SIZE, 24,
        GLX_DOUBLEBUFFER,
        None
    };
    
   
    XVisualInfo *vi = glXChooseVisual(app->display, app->screen, visual_attribs);
    if (vi == NULL) {
        fprintf(stderr, "No appropriate visual found for OpenGL.\n");
        return 0;
    }
    

    app->colormap = XCreateColormap(app->display, 
                                   RootWindow(app->display, app->screen),
                                   vi->visual, AllocNone);
    

    XSetWindowAttributes swa;
    swa.colormap = app->colormap;
    swa.event_mask = ExposureMask | KeyPressMask;
    

    app->window = XCreateWindow(
        app->display,
        RootWindow(app->display, app->screen),
        x, y,
        width, height,
        0,                 // border width
        vi->depth,         // depth from the visual
        InputOutput,       // class
        vi->visual,        // visual
        CWColormap | CWEventMask, // value mask
        &swa               // attributes
    );
    

    XStoreName(app->display, app->window, title);
    

    app->glx_context = glXCreateContext(app->display, vi, NULL, GL_TRUE);
    if (app->glx_context == NULL) {
        fprintf(stderr, "Failed to create GLX context.\n");
        XFree(vi);
        return 0;
    }
    
    // Show the window
    XMapWindow(app->display, app->window);    

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    if (!glXMakeCurrent(app->display, app->window, app->glx_context)) {
        fprintf(stderr, "Failed to make GLX context current.\n");
        XFree(vi);
        return 0;
    }
    
    XFree(vi);
    XFlush(app->display);
    return 1;
}

void SGetMouseState(SApplication *app, SMouseState *mouseState) {
    if (app == NULL || mouseState == NULL) {
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
    
    // Update mouse position
    mouseState->mouseX = (float)win_x_return;
    mouseState->mouseY = (float)win_y_return;
    
    // Update mouse button state
    mouseState->mouseDown = (mask_return & Button1Mask) ? 1 : 0;
}


int SEventProcess(SApplication *app, SMouseState *mouseState) {
    if (app == NULL) {
        return 0;
    }
    
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
            case ButtonPress:
                if (app->event.xbutton.button == Button1) {
                    mouseState->mouseDown = 1;
                    mouseState->mouseX = (float)app->event.xbutton.x;
                    mouseState->mouseY = (float)app->event.xbutton.y;
                }
                break;
            case ButtonRelease:
                if (app->event.xbutton.button == Button1) {
                    mouseState->mouseDown = 0;
                }
                break;
            case MotionNotify:
                mouseState->mouseX = (float)app->event.xmotion.x;
                mouseState->mouseY = (float)app->event.xmotion.y;
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
    
    // Reset model-view matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}


static inline void SBeginFrame(SApplication *app) {

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}


static inline void SUpdateViewport(SApplication *app, int width, int height) {
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, height, 0, -1, 1); // Origin at top-left
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}



#elif defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#include <GL/gl.h>

// Window procedure function prototype
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

typedef struct {
    HINSTANCE hinstance;
    HWND hwnd;
    HDC hdc;
    HGLRC hglrc;
    MSG msg;
} SApplication;

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
        NULL                        // Additional application data
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
    

    ShowWindow(app->hwnd, SW_SHOW);
    UpdateWindow(app->hwnd);
    
    return 1;
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

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_CLOSE:
            PostQuitMessage(0);
            return 0;
            
        case WM_KEYDOWN:
            if (wParam == VK_ESCAPE) {
                PostQuitMessage(0);
                return 0;
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
#endif
