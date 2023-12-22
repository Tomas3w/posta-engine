#!/usr/bin/env python
from enum import Enum
import sys, os

script_path = os.path.realpath(os.path.abspath(sys.argv[0]))
parent_dir = os.path.abspath(os.path.join(script_path, "../.."))
sys.path.append(parent_dir)

class argument_error_type(Enum):
    LACKING_ARG = 1
    ARGUMENT_NOT_EXPECTED = 2
    NOERROR = 3
def argument(index, name):
    if len(sys.argv) < index + 1:
        return argument_error_type.LACKING_ARG
    if sys.argv[index] != name:
        return argument_error_type.ARGUMENT_NOT_EXPECTED
    return argument_error_type.NOERROR

def print_help():
    print("Posta, possible uses:")
    print(" posta run <project-name>")
    print("\tRuns the project specified by <project-name>")
    print(" posta debug <project-name>")
    print("\tOpens a gdb session with the project's executable")
    print(" posta build [<project-name>]")
    print("\tBuild the project specified by <project-name>, if not specified it builds all the projects known by posta")
    print(" posta make <project-name>")
    print("\tGenerates a template project with the name <project-name>")
    print(" posta make-class <class-name> <project-name>")
    print("\tGenerates a class with the name <class-name> and puts it in the files <project-name>/include/<class-name> and <project-name>/src/<class-name>\n\tIncludes those files inside of the sources.cmake file")
    print(" posta export <project-name> <path>")
    print("\tExports the project named <project-name> into a folder pointed by <path>, the final export folder will then be in <path>/<project_name>")
def main():
    script_dir = os.path.dirname(script_path)
    os.chdir(script_dir)
    os.chdir('..')

    os.environ['PATH'] = os.environ.get('PATH', '') + ';' + os.path.abspath('bin/internal/w64devkit/bin')

    if argument(1, "run") == argument_error_type.NOERROR:
        if len(sys.argv) < 3:
            print('Missing project name to run')
        else:
            import run
            run.run(sys.argv[2])
        return None
    elif argument(1, "debug") == argument_error_type.NOERROR:
        if len(sys.argv) < 3:
            print('Missing project name to debug')
        else:
            import debug
            debug.debug(sys.argv[2])
        return None
    elif argument(1, "build") == argument_error_type.NOERROR:
        import build
        if len(sys.argv) < 3:
            project_name = None
        else:
            project_name = sys.argv[2]

        build.build(project_name, open('path_to_lib_cmake.txt').read().strip())
        return None
    elif argument(1, "make") == argument_error_type.NOERROR:
        import generate_project
        if len(sys.argv) < 3:
            project_name = None
        else:
            project_name = sys.argv[2]
        generate_project.main(sys.argv[2])
        return None
    elif argument(1, "make-class") == argument_error_type.NOERROR:
        if len(sys.argv) < 3:
            print("Missing class name")
        elif len(sys.argv) < 4:
            print("Missing project name")
        else:
            import make_class
            class_name = sys.argv[2]
            project_name = sys.argv[3]
            make_class.make_class(class_name, project_name)
            return None
    elif argument(1, "export") == argument_error_type.NOERROR:
        if len(sys.argv) < 3:
            print("Missing project name")
        elif len(sys.argv) < 4:
            print("Missing export path")
        else:
            import export_project
            project_name = sys.argv[2]
            path = sys.argv[3]
            export_project.export_project(project_name, path)
            return None
    elif len(sys.argv) >= 2:
        print("Argument not recognized:", sys.argv[1])
    print_help()


if __name__ == "__main__":
    main()

