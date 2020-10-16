#include <iostream>
#include <fstream>
#include <sstream>
#include <exception>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

constexpr int SCR_WIDTH = 800;
constexpr int SCR_HEIGHT = 600;

constexpr size_t CANTIDAD_VERTICES = 9;

void procesarEntrada(GLFWwindow* window);
unsigned int crearPrograma();
void debug_callback (GLenum source, GLenum type, GLuint id, GLenum severity,
        GLsizei lenght, const GLchar *message, const void* userParam);

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT,
            "Primer triangulo", nullptr, nullptr);
    if (!window) {
        std::cout << "No se ha podido crear la ventana\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "No se ha podido inicializar glad\n";
        return -2;
    }

    // Lo que me va a salvar de ahora en adelante.
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(debug_callback, nullptr);

    auto size_callback = [] (GLFWwindow *window, int width, int height) {
        glViewport(0, 0, width, height);
    };
    glfwSetFramebufferSizeCallback(window, size_callback);

    unsigned int shader_program = crearPrograma();

    // Datos para dibujar
    float vertices[CANTIDAD_VERTICES] = {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f
    };

    unsigned int vertex_buffer, vertex_array;
    glGenVertexArrays(1, &vertex_array);
    glGenBuffers(1, &vertex_buffer);

    glBindVertexArray(vertex_array);

    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(
            GL_ARRAY_BUFFER,
            sizeof(vertices),
            vertices,
            GL_STATIC_DRAW
    );

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

    while(!glfwWindowShouldClose(window)) {
        procesarEntrada(window);

        glClearColor(0.2f, 0.3f, 0.3f, 0.1f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        glUseProgram(shader_program);
        glBindVertexArray(vertex_array);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    // Limpieza de la casa
    glDeleteVertexArrays(1, &vertex_array);
    glDeleteBuffers(1, &vertex_buffer);
    glDeleteProgram(shader_program);

    glfwTerminate();
    return 0;
}

void procesarEntrada(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}


void debug_callback (GLenum source, GLenum type, GLuint id, GLenum severity,
        GLsizei lenght, const GLchar *message, const void* userParam) {

    std::cout << "<OpenGL Error> [type: " << type << ", Id: " << id << "] "
              << "[" << severity << "]" << '\n';
    std::cout << message << '\n';
}

unsigned int crearPrograma() {
    // Lo que necesitamos para crear el programa
    std::ifstream vertex_source_file("vertex_shader.glsl");
    std::ifstream fragment_source_file("fragment_shader.glsl");
    if (!vertex_source_file.is_open() || !fragment_source_file.is_open()) {
        std::cout << "No se han podido abrir los archivos de los shaders\n";
        throw std::exception("No se han podido abrir los archivos");
    }
    std::stringstream ss;
    const int log_size = 512;
    int succes;
    char infoLog[log_size];

    // Creamos el vertex shader
    unsigned int vertex_shader = glCreateShader(GL_VERTEX_SHADER);

    // Obtenemos el const char* en el que este el codigo fuente
    ss << vertex_source_file.rdbuf();
    std::string vertex_shader_source = ss.str();
    const char* vertex_source_ptr = vertex_shader_source.c_str();

    // Compilamos y sacamos el resultado
    glShaderSource(vertex_shader, 1, &vertex_source_ptr, nullptr);
    glCompileShader(vertex_shader);
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &succes);
    if (!succes) {
        glGetShaderInfoLog(vertex_shader, log_size, nullptr, infoLog);
        std::cout << "Error al compilar el vertex shader: " << infoLog << '\n';
        throw std::exception("Error al compilar el vertex shader");
    }

    // Creamos el fragment shader
    unsigned int fragment_shader = glad_glCreateShader(GL_FRAGMENT_SHADER);

    // De igual manera obtenemos el const char* para el fragment shader
    ss.str("");
    ss << fragment_source_file.rdbuf();
    std::string fragment_source = ss.str();
    const char* fragment_source_ptr = fragment_source.c_str();

    // Compilamos y obtenemos el resultado
    glShaderSource(fragment_shader, 1, &fragment_source_ptr, nullptr);
    glCompileShader(fragment_shader);
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &succes);
    if (!succes) {
        glGetShaderInfoLog(fragment_shader, log_size, nullptr, infoLog);
        std::cout << "Error al compilar el fragment shader: " << infoLog << '\n';
        throw std::exception("Error al compilar el fragment shader");
    }

    // Ahora hacemos el programa
    unsigned int shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);

    // Y verificamos que se haya linkeado correctametne
    glGetProgramiv(shader_program, GL_LINK_STATUS, &succes);
    if (!succes) {
        glGetProgramInfoLog(shader_program, log_size, nullptr, infoLog);
        std::cout << "Error al linkear el programa: " << infoLog;
        throw std::exception("Error al linkear programa");
    }

    // Limpiamos la basura
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    return shader_program;
}
