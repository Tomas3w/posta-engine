# posta-engine

## Instalación

### Windows

Primero, instalar lo siguiente:
- El compilador *MinGW*, recomiendo instalar el kit *w64devkit*.
- Python (versión 3.10 en adelante)
- CMake (versión 3.16 en adelante)

Una vez instalado, busca la carpeta que contenga include, lib, bin, man, etc. En *w64devkit* esta dentro de x86_64-w64-mingw32, en esta carpeta se van a copiar la version de development de las siguientes librerias:
- SDL

  Para instalar SDL vaya a: https://github.com/libsdl-org/SDL/releases y descarge la ultima version con el nombre: SDL2-devel-X.X.X-mingw.zip, donde X.X.X es la versión.
  Una vez descargado extraega los elementos de la sub carpeta x86_64-w64-mingw32 dentro de la carpeta x86_64-w64-mingw32 anteriormente mencionada
- SDL_image
  
  Mismo proceso que SDL pero con este link: https://github.com/libsdl-org/SDL_image/releases
- SDL_ttf

  Mismo proceso que SDL pero con este link: https://github.com/libsdl-org/SDL_ttf/releases
- SDL_net

  https://github.com/libsdl-org/SDL_net/releases
- SDL_mixer

  https://github.com/libsdl-org/SDL_mixer
- GLEW

  Descarge los binarios de este link: https://glew.sourceforge.net/, una vez tenga el comprimido, se extrae los elementos de la sub carpeta glew-X.X.X dentro de x86_64-w64-mingw32
- Lua

    Descarge el codigo fuente en https://www.lua.org/download.html (lua-5.4.x.tar.gz), extraegalo en una carpeta, ejecute los siguiente comandos:

        mkdir src/build
        make install INSTALL_TOP=src/build PLAT=mingw TO_BIN="lua.exe luac.exe"
    
    Una vez hecho esto, copie el contenido dentro de src/build en x86_64-w64-mingw32
- Bullet

  Descarge el codigo en https://github.com/bulletphysics/bullet3, y dentro de la carpeta descargada, ejecuta:
        
        mkdir build
        cd build
        cmake-gui ..
  Recomiendo utilizar cmake-gui y deshabilitar los ejemplos y los unit tests, además de todo esto es necesario elegir donde se va a instalar y al final hacer:
        
        make install
  Luego copiamos el contenido de la carpeta que elegimos para que se instale en la carpeta x86_64-w64-mingw32

Una vez instaladas estas librerias, clona el repositorio, y dentro ejecuta:
    
    python generate_project.py template
Esto con el fin de crear un proyecto simple basado en la plantilla y para poder comprobar que no hubo errores con las dependencias.
Luego, para compilar el motor junto a la plantilla (es importante remplazar /path/to/x86_64-w64-mingw32 con el path absoluto de x86_64-w64-mingw32):
    
    cd build
    cmake .. -DCMAKE_PREFIX_PATH=/path/to/x86_64-w64-mingw32/lib/cmake/ -G "MinGW Makefiles" -DCMAKE_EXPORT_COMPILE_COMMANDS=1
    make
Si todo salio bien, debería ser posible ejecutar el template con:
    
    python run.py template

## Documentación
Link: https://tomas3w.github.io/posta-engine/
