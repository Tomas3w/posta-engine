import sys, re, shutil, os, subprocess

def replace_template_name(name):
    with open(f"apps/{name}/CMakeLists.txt", encoding="utf-8") as file:
        fileread = file.read()
    matches = []
    for x in re.finditer("[^\\w]?template[^\\w]?", fileread):
        span = [x.start(), x.end() - 1]
        if fileread[span[0]] != 't':
            span[0] += 1
        if fileread[span[1]] != 'e':
            span[1] -= 1
        matches.append(span)
    matches.reverse()
    for m in matches:
        fileread = fileread[:m[0]] + name + fileread[m[1] + 1:]
    #print(fileread)
    with open(f"apps/{name}/CMakeLists.txt", "w") as file:
        file.write(fileread)

def copy_template_folder(name):
    shutil.copytree("template", f"apps/{name}")

def append_subdirectory(name):
    with open("apps/CMakeLists.txt", "a") as file:
        file.write(f"\nadd_subdirectory({name})")

def create_build_symlinks_and_build_folder(name):
    os.makedirs(f"build/apps/{name}")
    subprocess.run(["ln", "-s" , os.path.abspath('common-assets/common'), os.path.abspath(f'build/apps/{name}/common')])
    subprocess.run(["ln", "-s" , os.path.abspath(f'apps/{name}/assets'), os.path.abspath(f'build/apps/{name}/assets')])

def main():
    #value = input("Do you have permission for making symlinks? (Y/N): ")
    #if value != 'Y':
    #    print("Can't make project if there is no permission")
    #    return None
    print(f"Creating project {sys.argv[1]} from template...")
    print("Copying template to a new folder...")
    copy_template_folder(sys.argv[1])
    #print("Adding line to make compilation possible...")
    #append_subdirectory(sys.argv[1])
    print("Creating custom CMakeLists...")
    replace_template_name(sys.argv[1])
    #print("Creating build folders and symlinks...")
    #create_build_symlinks_and_build_folder(sys.argv[1])
    print("Done!")

if __name__ == "__main__":
    main()

