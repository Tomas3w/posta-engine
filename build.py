import os, subprocess

def main():
    os.chdir('./build')
    subprocess.run(['make'])

if __name__ == "__main__":
    main()

