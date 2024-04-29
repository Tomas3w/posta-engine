import os

def inc_text(class_name):
    guard_name = class_name.replace('/', '_').upper() + '_H'
    return f"""#ifndef {guard_name}
#define {guard_name}

class {class_name[class_name.rfind('/') + 1:]}
{{
    public:
        //
    private:
        //
}};

#endif // {guard_name}
"""

def src_text(class_name):
    return f"""#include "{class_name}.h"
"""

def exists(path):
    try:
        with open(path) as file:
            pass
    except FileNotFoundError:
        return False
    return True

def make_class(class_name, project_name):
    if '/' in class_name:
        os.makedirs(f"apps/{project_name}/src/{class_name[:class_name.rfind('/')]}", exist_ok=True)
        os.makedirs(f"apps/{project_name}/include/{class_name[:class_name.rfind('/')]}", exist_ok=True)

    source_path = f"apps/{project_name}/src/{class_name}.cpp"
    include_path = f"apps/{project_name}/include/{class_name}.h"
    if exists(source_path):
        print(f'There is already a source file named {class_name}.cpp')
        return None
    if exists(include_path):
        print(f'There is already an include file named {class_name}.h')
        return None
    with open(f'apps/{project_name}/sources.cmake') as file:
        lines = []
        for i, line in enumerate(file.readlines()):
            if line.find(')') != -1:
                lines.append('\t' + f"src/{class_name}.cpp" + "\n")
            lines.append(line)
    with open(f'apps/{project_name}/sources.cmake', 'w') as file:
        file.write(''.join(lines))
    with open(source_path, 'w') as file:
        file.write(src_text(class_name))
    with open(include_path, 'w') as file:
        file.write(inc_text(class_name))

if __name__ == '__main__':
    make_class('hola', 'cubes')

