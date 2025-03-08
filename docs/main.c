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
    
    SSlider slider = SCreateSlider(200, 50, 150, 20, 0, 100, 50);
    SButton button = SCreateButton(200, 100, 100, 30, "Click Me");
    SToggle checkbox = SCreateToggle(200, 150, 20, 20, "Enable Feature", false);
    
    // Main loop
    while (SEventProcess(&app)) {
        // Begin frame
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        SGetMouseState(&app);
        // Draw widgets
        SDrawSlider(&app, &slider);
        SDrawButton(&app, &button);
        SDrawToggle(&app, &checkbox);
        
        // Handle input
        SUpdateSlider(&app, &slider, app.mouseX, app.mouseY, app.mouseDown);
        SUpdateButton(&app, &button, app.mouseX, app.mouseY, app.mouseDown);
        SUpdateToggle(&app, &checkbox, app.mouseX, app.mouseY, app.mouseDown);


        SDrawTriangle(&app, blue, 100.0f, 150.0f, 50.0f);   
        SDrawRectangle(&app, red, 200.0f, 150.0f, 40.0f, 40.0f);  
        SDrawCircle(&app, green, 300.0f, 150.0f, 25.0f); 
        
        // End frame
        SSwapBuffers(&app);
    }

    return 0;
}
