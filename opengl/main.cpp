#include <array>
#include <assert.h>
#include <cstdio>
#include <cstdlib>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include <random>
#include <iostream>
#include <fstream>
#include <print>
#include <vector>
#include <string>

#define GLFW_INCLUDE_NONE
#include <glad/gl.h>
#include <GLFW/glfw3.h>


#define ARRAY_LEN(arr) (sizeof(arr) / sizeof(*arr))


typedef unsigned int uint;

constexpr int window_width  = 1200;
constexpr int window_height = window_width;



static char *load_file(const char *filename) {
    FILE *f = fopen(filename, "r");

    struct stat statbuf = {0};
    stat(filename, &statbuf);

    char *str = (char*) calloc(statbuf.st_size + 1, sizeof(char));
    size_t index = 0;

    char c = '\0';
    while ((c = fgetc(f)) != EOF)
        str[index++] = c;

    assert(index == (size_t)statbuf.st_size);

    fclose(f);
    return str;
}

static uint load_shader(const std::string &filename, GLuint type) {
    char *shader_src = load_file(filename.c_str());
    GLuint id = glCreateShader(type);
    glShaderSource(id, 1, (const char **)&shader_src, nullptr);
    glCompileShader(id);

    int success = false;
    char log[512] = { 0 };
    glGetShaderiv(id, GL_COMPILE_STATUS, &success);

    if(!success) {
        glGetShaderInfoLog(id, sizeof log, nullptr, log);
        std::print("Shader Compilation Error: {}: {}\n", filename, log);
    }

    free(shader_src);
    return id;
}


static uint load_shaders(const std::string &filename_vert, const std::string &filename_frag) {
    uint vs = load_shader(filename_vert, GL_VERTEX_SHADER);
    uint fs = load_shader(filename_frag, GL_FRAGMENT_SHADER);

    uint shader_program = glCreateProgram();
    glAttachShader(shader_program, fs);
    glAttachShader(shader_program, vs);
    glLinkProgram(shader_program);

    int success = false;
    char log[512] = { 0 };
    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shader_program, sizeof log, NULL, log);
        std::print("Shader Linking Error: {}\n", log);
    }

    glUseProgram(shader_program);

    glDeleteShader(vs);
    glDeleteShader(fs);  
    return shader_program;
}


static void handle_input(GLFWwindow *window) {
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}


static float generate_num(float max) {
    float min = 0;
    int range = max - min + 1;
    float num = rand() % range + min;
    return num;
}

static void generate_points(float *points, size_t n) {
    for (size_t i=0; i < n; i+=2) {
        float x = generate_num(window_width) / window_width;
        float y = generate_num(window_height) / window_height;
        points[i]   = x;
        points[i+1] = y;
    }
}



int main() {

    srand(time(NULL));

    if (!glfwInit())
        return EXIT_FAILURE;

    GLFWwindow *window = glfwCreateWindow(
        window_width,
        window_height,
        "Hello World",
        nullptr,
        nullptr
    );
    if (window == nullptr) {
        glfwTerminate();
        return EXIT_FAILURE;
    }

    glfwMakeContextCurrent(window);
    gladLoadGL(glfwGetProcAddress);



    float vertices[] = {
         0.5f,  0.5f, 0.0f,  // top right
         0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  // bottom left
        -0.5f,  0.5f, 0.0f   // top left 
    };

    uint indices[] = {
        0, 1, 3,   // first triangle
        1, 2, 3    // second triangle
    };

    uint vao = 0;
    glGenVertexArrays(1, &vao);  
    glBindVertexArray(vao);

    uint ebo;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof indices, indices, GL_STATIC_DRAW); 

    uint vbo = 0;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glNamedBufferData(vbo, sizeof vertices, vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        3 * sizeof(float),
        nullptr
    );
    glEnableVertexAttribArray(0);

    uint shader_program = load_shaders("vert.glsl", "frag.glsl");


    // window size uniform
    glUniform2f(glGetUniformLocation(shader_program, "window_size"), window_width, window_height);


    // points uniform
    float data[12] = { 0 };
    assert(ARRAY_LEN(data) % 2 == 0);
    generate_points(data, ARRAY_LEN(data));
    glUniform2fv(glGetUniformLocation(shader_program, "points"), ARRAY_LEN(data)/2, data);



    float color_bg = 0.278f;
    glClearColor(color_bg, color_bg, color_bg, 1.0f);

    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    double time_old = glfwGetTime();
    double delay = 0.1f;

    while (!glfwWindowShouldClose(window)) {
        handle_input(window);
        glClear(GL_COLOR_BUFFER_BIT);

        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        double time = glfwGetTime();

        if (time - time_old >= delay) {
            generate_points(data, ARRAY_LEN(data));
            glUniform2fv(glGetUniformLocation(shader_program, "points"), ARRAY_LEN(data)/2, data);
            time_old = time;
        }


        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    glfwTerminate();

    return EXIT_SUCCESS;
}
