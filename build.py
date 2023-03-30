import os, subprocess

def build():
    os.chdir('./build')
    p = subprocess.Popen("make")#, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    while p.poll() is None:
        pass#l = p.stdout.readline()
        #print(l.decode(), end='')
    #print(p.stdout.read().decode(), end='')
    os.chdir('..')
    return p.returncode == 0

if __name__ == "__main__":
    build()

