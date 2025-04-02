#include <stdui/window.h>
#include <stdui/widgets.h>
#include <stdui/image.h>
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

    

    // Main loop
    while (SEventProcess(&app)) {
        // Begin frame
        SBeginFrame(&app);
        
        
        SDrawText(&app, "OpenGL Text Rendering Demo", 10, SGetCurrentWindowHeight(&app) - 30, 2.0f, 1.0f, 1.0f, 1.0f);
      

        SDrawTriangle(&app, blue, 100.0f, 150.0f, 50.0f);   
        SDrawRectangle(&app, red, 200.0f, 150.0f, 40.0f, 40.0f);  
        SDrawCircle(&app, green, 300.0f, 150.0f, 25.0f); 
        SDrawImage("docs/images/image.png", 1920, 1080, 0.5, -0.5);
        // End frame
        SSwapBuffers(&app);
    }

    return 0;
}
