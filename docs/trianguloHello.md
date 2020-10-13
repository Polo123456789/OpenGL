Resumen
=======

1. Escribimos y compilamos los shaders
2. Los linkeamos en un programa
3. Generamos los vertex arrays y buffers
4. Bindeamos el vertex array
5. Le damos los datos y atributos al buffer
6. Desbindeamos el vertex array

Cuando dibujamos:

1. Bindeamos el vertex array
2. Dibujamos
3. Desbindeamos el vertex array

Hola Mundo
==========

Vertex Buffer
-------------

Partiendo de el codigo de la vez pasada, y con la lectura de el proceso en la 
primera parte, vamos a dibujar un triangulo.

Primero las posiciones de los vertices. Con las coordenadas de Z en cero.
Recuerda que OpenGl trabaja con un sistema de coordenadas de -1 a 1

~~~c++
    // Los vertices del triangulo a dibujar.
    float vertices[] = {
        //x      y     z
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f, -0.5f, 0.0f,
    };
~~~

Ahora vamos a enviar a la memoria de la GPU los datos. Este se guarda en lo que
se llama como vertex buffer. Para entenderlo quitale lo de vertex, es un buffer
de datos.

*Nota:* Pasar los datos del CPU a la GPU es una operacion costosa, asi que
igual que cuando escribes al disco, trata de hacer todo lo que se pueda de un
solo.

~~~c++
    // Se le va a dar un id, que guardaremos aqui
    unsigned int vertex_buffer;
    //           Cantidad, Puntero a los buffer
    glGenBuffers(1, &vertex_buffer);
~~~

Y le damos el tipo:

~~~c++
    // Lo bindeamos a un GL_ARRAY_BUFFER 
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
~~~

De ahora en adelante lo que haremos sera configurar este buffer. Ahora vamos a
copiar los vertices a el buffer.

~~~c++
    // Y copiamos los datos dandole un uso
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
~~~

El uso puede ser:
1. GL_STREAM_DRAW: Se pone solo una vez, y se usa varias veces en el GPU
2. GL_STATIC_DRAW: Se pone solo una vez, se usa varias veces
3. GL_DYNAMIC_DRAW: Se cambia y se usa varias veces.

El Vertex Shader
----------------

### Escribimos el shader

Ahora vamos a escribir el shader. Este se escribe en el lenguaje GLSL (OpenGL
Shading Language). Primero le damos la version, luego declaramos los atributos
del vertex_buffer en el shader usando la keyword in. Dejamos layout (location =
0), eso lo veremos despues. La salida tiene que ser el gl_Position, que sera un
vec4. En este caso solo pasaremos del vec3 al vec4 dejando vec.w en 1.0

~~~glsl
#version 330 core
layout (location=0) in vec3 aPos;

void main()
{
    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
}
~~~

Este sera probablemente el shader mas facil que se puede hacer, ya que solo
pasamos la informacion. Esto lo podemos hacer gracias a que ya tenemos los
datos normalizados, pero esto no siempre sera asi.

### Compilacion del shader

Lo dejamos escrito como tal en nuestro codigo fuente:

~~~c++
const char* vertex_shader_source = "#version 330 core\n"
"layout (location=0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";
~~~

Este sera compilado en el runtime, asi que procederemos a asignarle una id,
darle el codigo fuente a la id, y a compilarlo.

~~~c++
    unsigned int vertex_shader;
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    
    glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
    glCompileShader(vertex_shader);
    
    // Revisamos que se haya compilado exitosamente
    int success;
    char info_log[512];
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex_shader, 512, NULL, info_log);
        std::cout << "Error al compilar el shader: " << info_log << std::endl;
        glfwTerminate();
        return -1;
    }
~~~

Fragment shader
---------------

En el mismo lenguaje que el vertex shader, tiene que sacar un vec4 que
representa el RGBA.

~~~glsl
#version 330 core
out vec4 FragColor;

void main()
{
    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
}
~~~

### Compilacion del shader

~~~c++
    // Fragment shader
    unsigned int fragment_shader;
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, fragment_shader_source, NULL);
    glCompileShader(fragment_shader);
    // Revisamos que se haya compilado exitosamente
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment_shader, 512, NULL, info_log);
        std::cout << "Error al compilar el shaderf: " << info_log << std::endl;
        glfwTerminate();
        return -1;
    }
~~~

El programa Shader
------------------

Estos son varios shaders juntos. Vamos a linkear los shaders en un programa, y
a llamar este programa cuando vayamos a renderizar los objetos.

Creamos el objeto programa:

~~~c++
    unsigned int shader_program;
    shader_program = glCreateProgram();
    // Linkeamos los shaders
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);
    // Revisamos que se haya compilado exitosamente
    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shader_program, 512, NULL, info_log);
        std::cout << "Error al compilar el programa" << info_log << std::endl;
        glfwTerminate();
        return -1;
    }
~~~

Para usarlo indicas que lo tenemos que usar:

~~~c++
    glUseProgram(shader_program);
~~~

Y recuerda que ahora puedes borrar los shaders.

~~~c++
    // Ya con el programa borramos los shaders
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
~~~

Esto deveriamos abstraerlo a una  funcion afuera de el codigo.

*ULTIMOS ESFUERZOS:* Ya enviamos los datos al GPU, y le dimos las instrucciones
al GPU sobre como procesar la informacion. El ultimo problema es que OpenGl no
sabe como interptetar el vertex buffer que enviamos la memoria, ni sabe como
conectar el vertex buffer con los atributos del vertex shader.

Linking de los atributos del Vertex
-----------------------------------

Aqui es donde le decimos a OpenGL como tenemos la informacion en la memoria.

~~~c++
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float)*3, nullptr);
    glEnableVertexAttribArray(0);
~~~

Los argumentos son los siguientes:

* El primero indica cual de los atributos queremos configurar. Recuerda que en
  el vertex_shader dijimos que usariamos el que este el (location=0)
* Luego cuantos van. Pueden ser numeros del 1 al 4.
* El tipo de dato
* Si queremos que nos normalize los datos
* El stride (La distancia entre atributos)
* EL offset de el primer valor. Como no tenemos ningun offset, lo dejo como un
  puntero nulo

Y luego lo habilitamos, si no lo habilitamos no nos va a funcionar.

### Por buena practicas

Bueno, el resumen de lo que tendremos que hacer cada vez que quieras dibujar
algo en OpenGL es:

~~~c++
    // Lo bindeamos a un GL_ARRAY_BUFFER 
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);

    // Y copiamos los datos dandole un uso
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Explicamos como esta distribuido en la memoria y lo habilitamos
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float)*3, nullptr);
    glEnableVertexAttribArray(0);

    glUseProgram(shader_program);

    alguna_funcion_que_dibuja();
~~~

El repetir este proceso para no puede parecer mucho, pero si tenemos mas de 5
vertex_atributes ya valimos. Asi que mejor hacemos un objeto en le que podamos
guardar estas configuraciones, y ya despues solo lo bindeamos.

Ademas, no es por fregar, pero OpenGL en la configuracion core se negara a
dibujar si no hacemos esto, es solo un detallito pequeño.

#### Vertex Array Object

Simple y sensillamente lo bindeamos cuando configuremos el vertex_buffer, lo
desbindeamos despues, y ya luego lo bindeamos y antes de dibujar, y lo
desbindeamos despues de dibujar

~~~c++
    // Los generamos
    unsigned int vertex_array;
    glGenVertexArrays(1, &vertex_array);
    unsigned int vertex_buffer;
    glGenBuffers(1, &vertex_buffer);

    // Bindeamos el vertex_array
    glBindVertexArray(vertex_array);

    // Lo bindeamos a un GL_ARRAY_BUFFER 
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);

    // Y copiamos los datos dandole un uso
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Explicamos como esta distribuido en la memoria y lo habilitamos
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float)*3, nullptr);
    glEnableVertexAttribArray(0);

    // DesBindeamos el vertex_array
    glBindVertexArray(0);
~~~

Ahora para dibujar hacemos esto:

~~~c++
    glUseProgram(shader_program);
    glBindVertexArray(vertex_array);
    alguna_funcion_que_dibuja();
    glBindVertexArray(0);
~~~


Al fin el bendito triangulo
===========================

Ahora solo lo dibujamos:

~~~c++
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
~~~

glDrawArrays toma los siguientes argumentos:

* El primitivo a dibujar
* El indice del que empieza en el vertex buffer
* Cuandos vertices va a dibujar

Y con esto ya estamos. Que camino mas largo para dibujar un bendito triangulo.

Ya limpiando el codigo un poco. Pasando los shaders a ser leidos por archivos,
llegamos a este resultado:

~~~c++
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

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
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
~~~

El codigo igual esta en el archivo src/primer_triangulo.cpp
