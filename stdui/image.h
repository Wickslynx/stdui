#ifndef IMAGE_H
#define IMAGE_H
//Copyright (C) <2025>  <Wickslynx>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#ifndef WIDGETS_H
#include "widgets.h"
#endif

#ifndef STDUI_IMAGE_SUPPORT_OFF
#include <GL/gl.h>

#define STB_IMAGE_IMPLEMENTATION
#include "internal/stb_image.h"


typedef struct {
    GLuint textureID;
    GLuint VAO;
    GLuint VBO;
    GLuint EBO;
    GLuint shaderProgram;
} ImageRenderer;


unsigned char* loadImage(const char* filename, int* width, int* height, int* channels);
ImageRenderer* createImageRenderer(const char* filename, int width, int height, float posX, float posY);
void SDrawImage(const char* filename, int width, int height, float posX, float posY);
void renderImage(ImageRenderer* renderer);
void destroyImageRenderer(ImageRenderer* renderer);

// Fixed: Added extern for global variable declaration
extern ImageRenderer* imageRenderer;
// Fixed: Added proper initialization in the implementation section
ImageRenderer* imageRenderer = NULL;

// Vertex shader
const char* vertexShaderSource = 
    "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec2 aTexCoord;\n"
    "out vec2 TexCoord;\n"
    "void main()\n"
    "{\n"
    "    gl_Position = vec4(aPos, 1.0);\n"
    "    TexCoord = aTexCoord;\n"
    "}\0";

// Fragment shader
const char* fragmentShaderSource = 
    "#version 330 core\n"
    "out vec4 FragColor;\n"
    "in vec2 TexCoord;\n"
    "uniform sampler2D texture1;\n"
    "void main()\n"
    "{\n"
    "    FragColor = texture(texture1, TexCoord);\n"
    "}\0";

unsigned char* loadImage(const char* filename, int* width, int* height, int* channels) {
    stbi_set_flip_vertically_on_load(true);  // Flip image vertically for OpenGL
    unsigned char* data = stbi_load(filename, width, height, channels, 0);
    if (!data) {
        fprintf(stderr, "Failed to load image: %s\n", filename);
    }
    return data;
}




// Renamed function to avoid name conflict with widgets.h
static GLuint createImageShaderProgram() {
    GLuint vertexShader = compileShader(vertexShaderSource, GL_VERTEX_SHADER);
    GLuint fragmentShader = compileShader(fragmentShaderSource, GL_FRAGMENT_SHADER);
    
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    
    // Check for linking errors
    GLint success;
    char infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        fprintf(stderr, "ERROR::SHADER::PROGRAM::LINKING_FAILED\n%s\n", infoLog);
    }
    
    // Delete shaders as they're linked into our program and no longer necessary
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    return shaderProgram;
}

ImageRenderer* createImageRenderer(const char* filename, int width, int height, float posX, float posY) {
    ImageRenderer* renderer = (ImageRenderer*)malloc(sizeof(ImageRenderer));
    if (!renderer) return NULL;

    int channels;
    unsigned char* data = loadImage(filename, &width, &height, &channels);
    if (!data) {
        free(renderer);
        return NULL;
    }

    // Create texture
    glGenTextures(1, &renderer->textureID);
    glBindTexture(GL_TEXTURE_2D, renderer->textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, channels == 4 ? GL_RGBA : GL_RGB, width, height, 0,
                 channels == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, data);
    stbi_image_free(data);

    // Create shader program - updated to use the renamed function
    renderer->shaderProgram = createImageShaderProgram();

    // Vertex data with dynamic position
    float vertices[] = {
        // positions                  // texture coords
        posX - 0.5f, posY - 0.5f, 0.0f,  0.0f, 0.0f,  // bottom left
        posX + 0.5f, posY - 0.5f, 0.0f,  1.0f, 0.0f,  // bottom right
        posX + 0.5f, posY + 0.5f, 0.0f,  1.0f, 1.0f,  // top right
        posX - 0.5f, posY + 0.5f, 0.0f,  0.0f, 1.0f   // top left
    };
    unsigned int indices[] = {
        0, 1, 2,  // first triangle
        2, 3, 0   // second triangle
    };

    // Create buffers
    glGenVertexArrays(1, &renderer->VAO);
    glGenBuffers(1, &renderer->VBO);
    glGenBuffers(1, &renderer->EBO);

    glBindVertexArray(renderer->VAO);

    glBindBuffer(GL_ARRAY_BUFFER, renderer->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Texture attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    return renderer;
}

void renderImage(ImageRenderer* renderer) {
    if (!renderer) return;

    glUseProgram(renderer->shaderProgram);
    glBindTexture(GL_TEXTURE_2D, renderer->textureID);
    glBindVertexArray(renderer->VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void SDrawImage(const char* filename, int width, int height, float posX, float posY) {
    // Fixed: Free existing image renderer before creating a new one
    if (imageRenderer) {
        destroyImageRenderer(imageRenderer);
        imageRenderer = NULL;
    }
    imageRenderer = createImageRenderer(filename, width, height, posX, posY);
}

void destroyImageRenderer(ImageRenderer* renderer) {
    if (!renderer) return;

    glDeleteVertexArrays(1, &renderer->VAO);
    glDeleteBuffers(1, &renderer->VBO);
    glDeleteBuffers(1, &renderer->EBO);
    glDeleteTextures(1, &renderer->textureID);
    glDeleteProgram(renderer->shaderProgram);
    free(renderer);
}

#else

void SDrawImage(const char* filename, int width, int height, float posX, float posY) {
    printf("(WARNING) Images is not supported in the vulkan version, will not be rendered.\n");
    // Fixed: Added newline character to the warning message
}


#endif //STDUI_IMAGE_SUPPORT_OFF
#endif //IMAGE_H
