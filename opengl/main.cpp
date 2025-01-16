#include <assert.h>
#include <cstdio>
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

constexpr int window_width  = 500;
constexpr int window_height = 500;



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


// static char *load_file(std::string &filename) {
//
//     // TODO:
//     std::fstream file(filename);
//     std::string str;
//
//     std::getline(file, str);
//
//
//     return nullptr;
// }


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


struct Point {
public:
    int x;
    int y;
};

static int generate_num(int max) {
    int min = 0;
    int range = max - min + 1;
    int num = rand() % range + min;
    return num;
}

static std::vector<Point> generate_points(int n) {
    std::vector<Point> v;

    for (size_t i=0; i < (size_t) n; ++i) {
        Point p = {
            .x = generate_num(window_width),
            .y = generate_num(window_height)
        };
        v.push_back(p);
    }

    return v;

}



int main() {

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
    float data[4] = {
        (float) window_width/2, (float) window_height/2,
        1, 1
    };
    glUniform2fv(glGetUniformLocation(shader_program, "points"), ARRAY_LEN(data)/2, data);



    float color_bg = 0.278f;
    glClearColor(color_bg, color_bg, color_bg, 1.0f);

    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    while (!glfwWindowShouldClose(window)) {
        handle_input(window);
        glClear(GL_COLOR_BUFFER_BIT);

        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    glfwTerminate();

    return EXIT_SUCCESS;
}
