Preliminar
==========

Solo recuerda que los headers van en este orden:

~~~c++
#include<glad/glad.h>
#include<GLFW/glfw3.h>
~~~

La primera ventana
==================

Primero que nada, incializamos openGL:

~~~c++
    // Inicializacion de openGL
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // Elegimos el perfil core, para tener las funciones modernas
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
~~~

Aqui se puede ver el ejemplo de que este es una maquina de estado, ya que tiene
los datos, como GLFW_CONTEXT_VERSION_MAJOR, que nosotros modificamos mediante
funciones. Aqui lo que hacemos es darle la versión, y el perfil que queremos
usar. En este caso usaremos el perfil core, que es el que nos da mas control

Luego procedemos a montar la ventana:

~~~c++
    // Creamos la ventana
    GLFWwindow* window = glfwCreateWindow(800, 600, "Aprendiendo",
    // Revisamos que se creara correctamente
    if (window == nullptr) {
        std::cout << "No se pudo crear la ventana\n";
        glfwTerminate();
        return 1;
    }
    // Y hacemos el contexto de rendering la ventana
    glfwMakeContextCurrent(window);
~~~

Ahora tenemos que acceder a las funciones de OpenGL, para eso usaremos glad,
que nos juntara los punteros a las funciones

~~~c++
    // Ahora recivimos los punteros a las funciones usando GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "No se pudo inicializar glad\n";
        return 2;
    }
~~~

Procedemos a darle el tamaño de la ventana

~~~c++
    // Ahora una ultima cosa, tenemos que darle el tamaño de la ventana
    //         x  y   w    h
    glViewport(0, 0, 800, 600);
    glClearColor(0.2f, 0.3f, 0.3f, 0.1f);
~~~

Y terminamos con el siguiente codigo con algunos añadidos

~~~c++
#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>

void procesoEntradas(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

int main(void) {
    // Inicializacion de openGL
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // Elegimos el perfil core, para tener las funciones modernas
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Creamos la ventana
    GLFWwindow* window = glfwCreateWindow(800, 600, "Aprendiendo",
                                                    nullptr, nullptr);
    if (window == nullptr) {
        std::cout << "No se pudo crear la ventana\n";
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(window);


    // Ahora recivimos los punteros a las funciones usando GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "No se pudo inicializar glad\n";
        return 2;
    }

    // Ahora una ultima cosa, tenemos que darle el tamaño de la ventana
    //         x  y   w    h
    glViewport(0, 0, 800, 600);
    glClearColor(0.2f, 0.3f, 0.3f, 0.1f);

    // Checkeamos por eventos y cerramos si toca
    while (!glfwWindowShouldClose(window)) {
        // Entrada
        procesoEntradas(window);

        // El rendering se hace despues de procesar la entrada
        glClear(GL_COLOR_BUFFER_BIT);

        // Explicado mas abajo
        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    // Cerramos todo
    glfwTerminate();
    return 0;
~~~
