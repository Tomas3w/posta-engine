import os

# for, it only works in windows
def export_project(project_name, path):
    path_to_folder = f'{path}/{project_name}'
    path_to_build_app = os.path.abspath("build/apps/" + project_name)
    os.mkdir(path_to_folder)
    os.chdir(path_to_folder)
    os.system(f'xcopy "{path_to_build_app}" /e')
    [os.system(f'rm -r {x}') for x in os.listdir() if not x in ('assets', 'common', f'{project_name}.exe')]

if __name__ == '__main__':
    export_project('formulaya', '..')

