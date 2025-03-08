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
        
        // Draw widgets
        SDrawSlider(&app, &slider);
        SDrawButton(&app, &button);
        SDrawToggle(&app, &checkbox);
        
        // Handle input
        SUpdateSlider(&app, &slider, mouseX, mouseY, mouseDown);
        SUpdateButton(&app, &button, mouseX, mouseY, mouseDown);
        SUpdateToggle(&app, &checkbox, mouseX, mouseY, mouseDown);
        
        // End frame
        SSwapBuffers(&app);
    }

    return 0;
}
