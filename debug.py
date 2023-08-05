import subprocess, os, sys

def debug(project_name):
    import build
    if build.build(project_name, open('path_to_lib_cmake.txt').read().strip()):
        os.chdir(f"build/apps/{project_name}")
        
        return os.system(f'gdb ./{project_name}.exe')

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print('Missing project name to debug')
    else:
        project_name = sys.argv[1]
        debug(project_name)

