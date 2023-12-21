import subprocess, os, sys, platform

def debug(project_name):
    import build
    if build.build(project_name, open('path_to_lib_cmake.txt').read().strip()):
        os.chdir(f"build/apps/{project_name}")

        """
        p = subprocess.Popen(f"gdb {project_name}.exe", stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        while p.poll() is None:
            l = p.stdout.readline()
            print(l.decode(), end='')
        print(p.stdout.read().decode(), end='')
        return p.returncode
        """
        if platform.system() == 'Windows':
            return os.system(f'gdb {project_name}.exe')
        else:
            return os.system(f'gdb {project_name}')

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print('Missing project name to debug')
    else:
        project_name = sys.argv[1]
        debug(project_name)

