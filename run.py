import subprocess, os, sys

def run(project_name):
    import build
    if build.build(project_name, open('path_to_lib_cmake.txt').read().strip()):
        os.chdir(f"build/apps/{project_name}")
        
        p = subprocess.Popen(f"./{project_name}.exe", stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        while p.poll() is None:
            l = p.stdout.readline()
            print(l.decode(), end='')
        print(p.stdout.read().decode(), end='')

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print('Missing project name to run')
    else:
        project_name = sys.argv[1]
        run(project_name)

