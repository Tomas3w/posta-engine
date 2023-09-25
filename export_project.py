import os

def export_project(project_name, path):
    os.system('ls build/apps/' + project_name)
    print()
    os.system('ls ' + path)
    print()
    os.system(f'cp build/apps/{project_name}/* {path}/{project_name}')

if __name__ == '__main__':
    export_project('formulaya', '.')

