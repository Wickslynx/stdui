#include <stdui/window.h>
#include <stdui/widgets.h>
#include <stdui/colors.h>

int main() {
    SApplication app;

    if (!SDisplayOpen(&app)) {
        return 1;
    }
    if (!SWindowCreate(&app, "My Window", 100, 100, 400, 300)) {
        SDisplayClose(&app);
        return 1;
    }

    SUpdateViewport(&app, 400, 300);
    
    while (SEventProcess(&app)) { //MAIN LOOP

        SBeginFrame(&app);
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        SDrawTriangle(&app, blue, 100.0f, 100.0f);    
        SDrawRectangle(&app, red, 200.0f, 100.0f);    
        SDrawCircle(&app, green, 300.0f, 100.0f);
    
        SSwapBuffers(&app);
    }
    SDisplayClose(&app);

    return 0;
}
