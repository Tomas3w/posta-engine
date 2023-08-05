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

def main(project_name):
    print(f"Creating project {project_name} from template...")
    print("Copying template to a new folder...")
    copy_template_folder(project_name)
    print("Creating custom CMakeLists...")
    replace_template_name(project_name)
    print("Done!")

if __name__ == "__main__":
    main(sys.argv[1])

