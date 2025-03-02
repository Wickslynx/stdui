#include "window.h"

int SDisplayOpen(application *app) {
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

int SWindowCreate(application *app, const char *title, int x, int y, int width, int height) {
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

int SEventProcess(application *app) {
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

void SDisplayClose(application *app) {
    if (app == NULL) {
        return;
    }

    XDestroyWindow(app->display, app->window);
    XCloseDisplay(app->display);
}
