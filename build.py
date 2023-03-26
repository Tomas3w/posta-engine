import os, subprocess

def build():
    os.chdir('./build')
    subprocess.run(['make'])
    os.chdir('..')

if __name__ == "__main__":
    build()

