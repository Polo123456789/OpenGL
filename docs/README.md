---
title: "OpenGl"
author: "Pablo Sanchez"
descripcion:
    "Pues estas seran mis notas en el camino para aprender OpenGl, y como se
    bien las dudas que me van surjiendo, espero poder ahorrarle dolores de
    cabeza a alguien en el futuro si las llega a leer."
---

Preeliminares
=============

Lo voy a trabajar usando GLFW, y obteniendo los punteros a las funciones usando
GLAD

Temas
=====

* [Que es?](#que-es)
* [Primera Ventana](#primera-ventana)
* [Triangulo Hello World](#triangulo-hello-world)

Que es
======

Que es OpenGL
-------------

Esta es una maquina de estado. Nosotros vamos a acceder a los miembros de ese
estado, modificandolos mediante funciones.

### El pipeline

OpenGL trabaja con coordenadas en 3D. Pero tu pantalla es un array 2D de
pixeles, la forma en la que hace la conversión es mediante el pipeline. Este se
puede separar en 2 partes:

1. La transformación de coordenadas 3D a puntos 2D.
2. La transformación de esos puntos a pixeles en la pantalla.

Nuestra interacción con ella se puede hacer mediante varios pasos, cada uno
dependiendo del resultado del anterior. Vea el Triangulo Hola Mundo

### Proceso

1. Vertex Shader: Coordenadas 3D a otras
2. Primitive Assembly: Coordenadas a figuras (Primitivas, como triangulos)
3. Geometry Shader: Toma primirivas, y genera mas figuras
4. Rasterization: De figura a pixeles en la pantalla. En el proceso de eliminan
   todos los fragmentos que quedan fuera de la vista del usuario para mejorar
   la performance.
5. Fragment Shader: Le da el color al pixel. Aqui entra la luz, blending, etc.

En openGL moderno, nosotros tenemos que definir almenos un vertex shader, y un
fragment shader.


La primera ventana
==================

Preliminar
----------

Solo recuerda que los headers van en este orden:

~~~c++
#include<glad/glad.h>
#include<GLFW/glfw3.h>
~~~

La primera ventana
------------------

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

Triangulo Hello World
=====================

Resumen
-------

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
----------

### Vertex Buffer

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

### El Vertex Shader

#### Escribimos el shader

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

#### Compilacion del shader

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

### Fragment shader

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

#### Compilacion del shader

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

### El programa Shader

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

### Linking de los atributos del Vertex

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

#### Por buena practicas

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

##### Vertex Array Object

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
---------------------------

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

Y con esto ya estamos. Que camino mas largo para dibujar un bendito triangulo. El codigo completo queda en el archivo src/primer_triangulo.cpp
