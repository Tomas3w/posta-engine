# posta-engine

## Instalación

### Windows

Primero, instalar lo siguiente:
- Python (versión 3.10 en adelante, aunque probablemnte funcione con versiones más antiguas)

Luego, clona el repositorio donde quieras, dentro del mismo ejecutar:

    cd bin
    python bin\devenv-setup.py
Con esto, se instalarán las librerias del motor junto a `w64devkit` (y con esto último un compilador de C++)
Una vez instaladas estas librerias, agregar `posta-engine\bin\internal\w64devkit\x86_64-w64-mingw32\bin` en el PATH del sistema.

## Úso
Si la instalación funcionó, ejecutar dentro de la carpeta `posta-engine`:
    
    python bin/posta.py make template
Esto con el fin de crear un proyecto simple basado en la plantilla y para poder comprobar que no hubo errores con las dependencias.
Si todo salio bien, debería ser posible ejecutar el template con:

    python bin/posta.py run template
El código fuente de los proyectos creados por el motor se encontrarán dentro de `posta-engine/apps`.
Para exportar un proyecto, ejecutar:

    python bin/posta.py export your-project path/to/project
`your-project` es el nombre del proyecto que se quiere exportar y `path/to/project` es el lugar donde se exportará el proyecto, por ejemplo, en este caso, el proyecto `your-project` se instalará en la carpeta `path/to/project/your-project`.
El projecto exportado contendrá un .exe, dlls y dos carpetas: `assets` y `common`. También, se puede ejecutar sin ningúna configuración extra tan solo ejecutando el exe, debería funcionar en cualquiera computadora.

## Documentación
Link: https://tomas3w.github.io/posta-engine/
