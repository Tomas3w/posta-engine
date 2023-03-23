import subprocess, os, sys

def main():
    n = sys.argv[1]
    os.chdir(f"build/apps/{n}")
    p = subprocess.Popen("./template.exe", stdout=subprocess.PIPE)
    while p.poll() is None:
        l = p.stdout.readline()
        print(l.decode(), end='')
    print(p.stdout.read().decode(), end='')

if __name__ == "__main__":
    main()

