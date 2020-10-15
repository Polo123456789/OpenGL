Que es OpenGL
=============

Esta es una maquina de estado. Nosotros vamos a acceder a los miembros de ese
estado, modificandolos mediante funciones.

El pipeline
-----------

OpenGL trabaja con coordenadas en 3D. Pero tu pantalla es un array 2D de
pixeles, la forma en la que hace la conversión es mediante el pipeline. Este se
puede separar en 2 partes:

1. La transformación de coordenadas 3D a puntos 2D.
2. La transformación de esos puntos a pixeles en la pantalla.

Nuestra interacción con ella se puede hacer mediante varios pasos, cada uno
dependiendo del resultado del anterior. Vea el Triangulo Hola Mundo

Proceso
-------

1. Vertex Shader: Coordenadas 3D a otras
2. Primitive Assembly: Coordenadas a figuras (Primitivas, como triangulos)
3. Geometry Shader: Toma primirivas, y genera mas figuras
4. Rasterization: De figura a pixeles en la pantalla. En el proceso de eliminan
   todos los fragmentos que quedan fuera de la vista del usuario para mejorar
   la performance.
5. Fragment Shader: Le da el color al pixel. Aqui entra la luz, blending, etc.

En openGL moderno, nosotros tenemos que definir almenos un vertex shader, y un
fragment shader.


