---
title: "OpenGl"
author: "Pablo Sanchez"
toc: true
descripcion:
    "Pues estas seran mis notas en el camino para aprender OpenGl, y como se
    bien las dudas que me van surjiendo, espero poder ahorrarle dolores de
    cabeza a alguien en el futuro si las llega a leer. Ya cuando sepa mas lo
    complemento"
---

Preliminares
=============

Lo voy a trabajar usando GLFW, y obteniendo los punteros a las funciones usando
GLAD. Se notara que voy leyendo el learopengl.com (@sitioWeb) mientras escribo
esto. En principio es para recordar la informacion, y conforme sepa mas lo voy
a ir complementando.

Que es
======

Que es OpenGL
-------------

Esta es una maquina de estado. Nosotros vamos a acceder a los miembros de ese
estado, modificándolos mediante funciones.

### El pipeline

OpenGL trabaja con coordenadas en 3D. Pero tu pantalla es un array 2D de
pixeles, la forma en la que hace la conversión es mediante el pipeline. Este se
puede separar en 2 partes:

1. La transformación de coordenadas 3D a puntos 2D.
2. La transformación de esos puntos a pixeles en la pantalla.

Nuestra interacción con ella se puede hacer mediante varios pasos, cada uno
dependiendo del resultado del anterior. Vea el Triangulo Hola Mundo

### Proceso

1. **Vertex Shader:** Coordenadas 3D a otras
2. **Primitive Assembly:** Coordenadas a figuras (Primitivas, como triángulos)
3. **Geometry Shader:** Toma primitivas, y genera mas figuras
4. **Rasterization:** De figura a pixeles en la pantalla. En el proceso de
   eliminan todos los fragmentos que quedan fuera de la vista del usuario para
   mejorar la performance.
5. **Fragment Shader:** Le da el color al pixel. Aquí entra la luz, blending,
   etc.

En openGL moderno, nosotros tenemos que definir al menos un vertex shader, y un
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

Aquí se puede ver el ejemplo de que este es una maquina de estado, ya que tiene
los datos, como GLFW_CONTEXT_VERSION_MAJOR, que nosotros modificamos mediante
funciones. Aquí lo que hacemos es darle la versión, y el perfil que queremos
usar. En este caso usaremos el perfil core, que es el que nos da mas control.

Luego procedemos a montar la ventana:

~~~c++
    // Creamos la ventana
    GLFWwindow* window = glfwCreateWindow(800, 600, "Aprendiendo",
        nullptr, nullptr);
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

Y terminamos con el siguiente código con algunos añadidos

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

Nota
----

Si no te funciona, saltate un capitulo, leete el otro y de regreso a el
triangulo.

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

Partiendo de el código de la vez pasada, y con la lectura de el proceso en la 
primera parte, vamos a dibujar un triangulo.

Primero las posiciones de los vértices. Con las coordenadas de Z en cero.
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

**Nota:** Pasar los datos del CPU a la GPU es una operación costosa, así que
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
copiar los vértices a el buffer.

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
0), eso lo veremos después. La salida tiene que ser el gl_Position, que sera un
vec4. En este caso solo pasaremos del vec3 al vec4 dejando vec.w en 1.0

~~~glsl
#version 330 core
layout (location=0) in vec3 aPos;

void main()
{
    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
}
~~~

Este sera probablemente el shader mas fácil que se puede hacer, ya que solo
pasamos la información. Esto lo podemos hacer gracias a que ya tenemos los
datos normalizados, pero esto no siempre sera asi.

#### Compilación del shader

Lo dejamos escrito como tal en nuestro código fuente:

~~~c++
const char* vertex_shader_source = "#version 330 core\n"
"layout (location=0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";
~~~

Este sera compilado en el runtime, así que procederemos a asignarle una id,
darle el código fuente a la id, y a compilarlo.

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

#### Compilación del shader

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

Esto deberíamos abstraerlo a una  función afuera de el código.

**ÚLTIMOS ESFUERZOS:** Ya enviamos los datos al GPU, y le dimos las instrucciones
al GPU sobre como procesar la información. El ultimo problema es que OpenGl no
sabe como interpretar el vertex buffer que enviamos la memoria, ni sabe como
conectar el vertex buffer con los atributos del vertex shader.

### Linking de los atributos del Vertex

Aquí es donde le decimos a OpenGL como tenemos la información en la memoria.

~~~c++
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float)*3, nullptr);
    glEnableVertexAttribArray(0);
~~~

Los argumentos son los siguientes:

* El primero indica cual de los atributos queremos configurar. Recuerda que en
  el vertex_shader dijimos que usaríamos el que este el (location=0)
* Luego cuantos van. Pueden ser numeros del 1 al 4.
* El tipo de dato
* Si queremos que nos normalize los datos
* El stride (La distancia entre atributos)
* EL offset de el primer valor. Como no tenemos ningún offset, lo dejo como un
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

Simple y sencillamente lo bindeamos cuando configuremos el vertex_buffer, lo
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
* Cuantos vertices va a dibujar

Y con esto ya estamos. Que camino mas largo para dibujar un bendito triangulo.
El código completo queda en el archivo src/primer_triangulo.cpp

Debugging
=========

Déjame contarte una historia. Ese primer triangulo fue un dolor de cabeza
tremendo. Fácil me heche unas 2 o 3 horas en que me saliera. 

Yo estaba todo contento, siguiendo el tutorial, y como habrás visto, para
llegar al triangulo hay que hacer un montón de cosas. Y en realidad es muy
fácil equivocarse en cualquiera de ellas. Y bueno, pues eso hice, me equivoque,
lo volví a intentar, volví a fallar, y a la tercera lo logre, y vi mi error.

Encima, la API no muy te ayuda. A las funciones así como `glBindBuffer`, el
primer parámetro que les pasamos es un `GLenum`. Bueno, pues resulta que si vas
a la definición de el `GLenum`, es en realidad un `unsigned int`. Ves el
problema?

En C++ tenemos los `enum class`. Porque? Porque el `enum` nada mas es solo un
numero entero. Si usas el `enum` nada mas, puedes pasar enteros en el lugar en
el que va el `enum`. También puedes pasar un `enum` diferente a la función. En
cambio con el `enum class`, no puedes usar uno diferente al que esta en la
lista de parámetros, errores tontos pasan a ser errores de compilación. Ahora,
entiendo que la librería esta en C, así que no puedo pedir que se usen los
`enum class`, pero mínimo que si sean `enum`. Si fueran `enum` diferentes al
menos creo que un analizador estático me tiraría el error, "Hey! Estas usando
el `enum` incorrecto". Pero no, es un `unsigned int`. Pasas el `enum`
incorrecto, y es imposible saber que te equivocaste hasta que ya estas
corriendo el programa.

Que me paso?
------------

En esta linea:

~~~c++
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
~~~

Había colocado lo siguiente:

~~~c++
    glBindBuffer(GL_VERTEX_ARRAY, vertex_buffer);
~~~

Usar un `enum` que nada que ver me costo mínimo una hora, releyendo el
tutorial, comparando con mi código, comparando con el código que dejaron de
resultado. Hasta que me di cuenta, lo cambie, y al final pude ver el triangulo.

Y que puedo hacer?
------------------

Pasamos mas tiempo arreglando el código que escribimos mal, que escribiendo
código nuevo. El ser capaces de encontrar nuestros errores es una parte
fundamental. Por eso me sorprendió ver que en el libro que estoy siguiendo
(@sitioWeb), se habla de el manejo de errores hasta en el capitulo 47, en la
pagina 436. 

Yo la verdad es que prefiero sacarlo del paso de un solo. Así que voy a hacer
lo siguiente: Voy a cambiar la versión de openGL que estoy usando a la versión
4.6. Porque? Porque el buscar errores en esta versión es mas fácil usando la
función `glDebugMessageCallback`. En las versiones anteriores a la 4.3, se
hacia un macro `glCall()`, y cada función que llamabas la llamabas con este
macro: `glCall(glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer));`. Recomiendo
mucho el tutorial de `The Cherno` [Dealing with Errors in
OpenGL](https://youtu.be/FBbPWSOQ0-w) si este es el camino que deseas tomar.

Yo lo haré con el con `glDebugMessageCallback` ya que siento que el código
queda mas limpio así, y es mas fácil.

glDebugMessageCallback
----------------------

Primero que nada, el cambio de versión:

~~~c++
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
~~~

Luego vamos a definir la función para hacer el callback:

~~~c++
void debug_callback (GLenum source, GLenum type, GLuint id, GLenum severity,
        GLsizei lenght, const GLchar *message, const void* userParam) {

    std::cout << "<OpenGL Error> [type: " << type << ", Id: " << id << "] "
              << "[" << severity << "]" << '\n';
    std::cout << message << '\n';
}
~~~

Esta de momento se vera bastante simple, e incompleta. Pero bueno, lo que de
momento me interesa el el mensaje. Ya después la iremos mejorando. Mejor dicho
la mejorare, y pondré otra versión después, pero por ahora esta me sirve.

Ya finalmente asignamos el callback después de inicializar glad, y antes de el
callback para ajustar tamaños.

~~~c++
    // Lo que me va a salvar de ahora en adelante.
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(debug_callback, nullptr);
~~~

Ahora si, si te equivocas, mínimo vas a recibir el mensaje de error, y vas
a saber que hiciste mal. Esto junto con unos print, y un buen uso de debugger,
nos va a ayudar a salvar problemas de ahora en adelante.

Siguiente
=========

Referencias
===========

* (@sitioWeb) [Learn OpenGL, Joey de Vries. Junio
  2020](https://learnopengl.com/)
