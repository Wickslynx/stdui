#ifndef STDUITYPES_H
#define STDUITYPES_H

typedef struct {
    float r;
    float g;
    float b;
    float a;
} SColor;

typedef struct {
    SColor color;
    // Add other shape properties if needed
} SShapeProps;

// Forward declaration of SApplication
typedef struct SApplication SApplication;

#endif // STDUITYPES_H
