//Global stuff declared here.
#include <stdio.h>

#if defined(__linux__) 
//Linux impl.

//Include x11.
#include <X11/Xlib.h> 
#include <X11/keysym.h>

typedef struct {
    Display *display;
    int screen;
    Window window;
    XEvent event;
    GLXContext glc;
} SApplication;


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

    app->window = XCreateSimpleWindow(
        app->display,
        RootWindow(app->display, app->screen),
        x, y,
        width, height,
        1,
        BlackPixel(app->display, app->screen),
        WhitePixel(app->display, app->screen)
    );
    
    XStoreName(app->display, app->window, title);
    XSelectInput(app->display, app->window, ExposureMask | KeyPressMask);
    XMapWindow(app->display, app->window);
    XFlush(app->display);

    static int visual_attribs[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
    int num_fbc = 0;
    GLXFBConfig *fbc = glXChooseFBConfig(app->display, DefaultScreen(app->display), visual_attribs, &num_fbc);
    XVisualInfo *vi = glXGetVisualFromFBConfig(app->display, fbc[0]);
    app->glc = glXCreateContext(app->display, vi, NULL, GL_TRUE);
    glXMakeCurrent(app->display, app->window, app->glc);
    
    return 1;
}

int SEventProcess(SApplication *app) {
    if (app == NULL) {
        return 0;
    }

    if (XPending(app->display) > 0) {
        XNextEvent(app->display, &app->event);
        
        switch (app->event.type) {
            case Expose:
                break;
                
            case KeyPress:
                if (XLookupKeysym(&app->event.xkey, 0) == XK_Escape) {
                    return 0;
                }
                break;
        }
    }
    return 1;
}

void SDisplayClose(SApplication *app) {
    if (app == NULL) {
        return;
    }

    glXMakeCurrent(app->display, None, NULL);
    glXDestroyContext(app->display, app->glc);
    XDestroyWindow(app->display, app->window);
    XCloseDisplay(app->display);
}


#elif defined(_WIN32) || defined(_WIN64)
#include <windows.h>

typedef struct {
    HINSTANCE hinstance;
    HWND hwnd;
} SApplication;

#elif defined(__APPLE__) && defined(__MACH__)
#include <ApplicationServices/ApplicationServices.h>
#endif


