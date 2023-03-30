import subprocess, os, sys

def main():
    n = sys.argv[1]

    import build
    if build.build():
        os.chdir(f"build/apps/{n}")
        
        p = subprocess.Popen(f"./{n}.exe", stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        while p.poll() is None:
            l = p.stdout.readline()
            print(l.decode(), end='')
        print(p.stdout.read().decode(), end='')

if __name__ == "__main__":
    main()

