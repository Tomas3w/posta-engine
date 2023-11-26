import os, sys, subprocess, platform

def folders_of_directory(directory):
    return [name for name in os.listdir(directory) if os.path.isdir(os.path.join(directory, name))]

def run_command(command):
    p = subprocess.Popen(command)
    while p.poll() is None:
        pass
    return p.returncode

def create_build_symlinks_and_build_folder(name):
    try:
        os.makedirs(f"build/apps/{name}")
    except FileExistsError:
        pass
    if platform.system() == "Windows":
        if not os.path.exists(fr'build\apps\{name}\common'):
            subprocess.run(["mklink", "/J" , fr'build\apps\{name}\common', r'common-assets\common'], shell=True)
        if not os.path.exists(fr'build\apps\{name}\assets'):
            subprocess.run(["mklink", "/J" , fr'build\apps\{name}\assets', fr'apps\{name}\assets'], shell=True)
    else:
        if not os.path.exists(f'build/apps/{name}/common'):
            subprocess.run(["ln", "-s" , os.path.abspath('common-assets/common'), os.path.abspath(f'build/apps/{name}/common')])
        if not os.path.exists(f'build/apps/{name}/assets'):
            subprocess.run(["ln", "-s" , os.path.abspath(f'apps/{name}/assets'), os.path.abspath(f'build/apps/{name}/assets')])

def build(project_name, path_to_lib_cmake):
    # creating CMakeLists.txt file
    with open("apps/CMakeLists.txt", 'w') as file:
        if project_name:
            found = False
            for name in folders_of_directory('apps'):
                if name == project_name:
                    file.write('add_subdirectory(' + name + ')\n')
                    found = True
                    break
            if not found:
                print(f'The project named {project_name} does not exist')
                return False
        else:
            for name in folders_of_directory('apps'):
                file.write('add_subdirectory(' + name + ')\n')
    # creating folders and symlinks (or junctions in windows)
    for name in folders_of_directory('apps'):
        create_build_symlinks_and_build_folder(name)
    # initial configuration of cmake
    if run_command("cmake -S . -B build -DCMAKE_PREFIX_PATH=" + path_to_lib_cmake + " -G \"Ninja\" -DCMAKE_EXPORT_COMPILE_COMMANDS=1") != 0:
        return False
    # building
    if run_command("cmake --build build") != 0:
        return False
    return True

if __name__ == "__main__":
    project_name = None
    if len(sys.argv) > 1:
        project_name = sys.argv[1]
    build(project_name, open('path_to_lib_cmake.txt').read().strip())

