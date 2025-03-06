#include <stdui/window.h>
#include <stdui/widgets.h>

int main() {
    SApplication app;

    if (!SDisplayOpen(&app)) {
        return 1;
    }
    if (!SWindowCreate(&app, "My Window", 100, 100, 400, 300)) {
        SDisplayClose(&app);
        return 1;
    }
    while (SEventProcess(&app)) {
        STriangle(&app, 1.0);
        printf("HELLO");
    }
    SDisplayClose(&app);

    return 0;
}
