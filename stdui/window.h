// Global stuff declared here.
#include <stdio.h>

#if defined(__linux__)
// Linux implementation.

#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <GL/gl.h>

typedef struct {
    Display *display;
    int screen;
    Window window;
    XEvent event;
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
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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
