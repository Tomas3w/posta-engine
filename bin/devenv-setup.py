#!/usr/bin/env python
bearer_token = None
global_headers = {
    'Authorization': f'Bearer {bearer_token}'
} if bearer_token else {}

import os, zipfile, tarfile, importlib, platform
from datetime import datetime

path_to_lib_cmake = None

def install_required_python_dependencies():
    code = os.system('python -m pip install -r requirements.txt')
    if code != 0:
        exit()
    requests = importlib.import_module('requests')
    return (requests,)

try:
    import requests
except ImportError as e:
    requests, = install_required_python_dependencies()

def progress_bar(percent, newline_if_1 = True):
    length = 20
    if percent == -1:
        now = datetime.now().timestamp() * 1000
        spaces = list(' ' * length)
        spaces[int(now / length / 2) % len(spaces)] = '='
        spaces = ''.join(spaces)
        progress_bar = f'[{spaces}]'
        print(f'\r[posta] {progress_bar}', end='', flush=True)
        if percent == 1:
            print()
        return None
    if percent < 0:
        percent = 0
    elif percent > 1:
        percent = 1
    arrow = '=' * int(length * percent)
    spaces = ' ' * (length - len(arrow))
    progress_bar = f'[{arrow}{spaces}] {int(percent * 100)}%'
    print(f'\r[posta] {progress_bar}', end='', flush=True)
    if percent == 1:
        print()

def extract_zip(zip_path):
    with zipfile.ZipFile(zip_path, 'r') as zip_ref:
        namelist = zip_ref.namelist()
        l = len(namelist)
        for i, name in enumerate(namelist):
            progress_bar((i + 1) / l)
            zip_ref.extract(name)
    return namelist[0]

def extract_tar_gz(tar_path):
    with tarfile.open(tar_path, 'r:gz') as tar_ref:
        namelist = tar_ref.getnames()
        l = len(namelist)
        for i, name in enumerate(namelist):
            progress_bar((i + 1) / l)
            tar_ref.extract(name)
    return namelist[0]

def download(url, filepath):
    from urllib.request import urlretrieve
    return urlretrieve(url, filepath, lambda count, block_size, total_size: progress_bar(count * block_size / total_size if total_size != -1 else -1))

def install_w64devkit():
    if not os.path.exists('w64devkit'):
        response = requests.get("https://api.github.com/repos/skeeto/w64devkit/releases/latest", headers=global_headers)
        metadata = response.json()
        url = [x for x in metadata['assets'] if x['name'] == f'w64devkit-{metadata["name"]}.zip'][0]['browser_download_url']
        print('[posta] Downloading latest w64devkit release...')
        if not os.path.exists('w64devkit.zip'):
            download(url, 'w64devkit.zip')
        print('[posta] Extracting w64devkit...')
        extract_zip('w64devkit.zip')
        os.remove('w64devkit.zip')
    try:
        os.mkdir('w64devkit/x86_64-w64-mingw32/lib/cmake')
    except FileExistsError:
        pass
    global path_to_lib_cmake
    path_to_lib_cmake = os.path.abspath('w64devkit/x86_64-w64-mingw32/lib/cmake')
    return 0

def install_freetype():
    if not os.path.exists("freetype-master"):
        print('[posta] Downloading freetype release...')
        if not os.path.exists('freetype-master.zip'):
            download(f"https://gitlab.freedesktop.org/freetype/freetype/-/archive/master/freetype-master.zip", 'freetype-master.zip')
        print('[posta] Extracting freetype-master.zip...')
        extract_zip('freetype-master.zip')
        os.chdir('freetype-master')
        print('[posta] Compiling freetype...')
        resultcode = os.system("cmake -S . -B build -DCMAKE_PREFIX_PATH=\"" + path_to_lib_cmake + "\" -G \"MinGW Makefiles\" -DCMAKE_INSTALL_PREFIX=\"../w64devkit/x86_64-w64-mingw32\"")
        if resultcode != 0:
            return resultcode
        resultcode = os.system("cmake --build build")
        os.chdir('build')
        resultcode = os.system("make install")
        os.chdir('../..')
    return 0

def install_sdl_like_from_source(name, url_name, release_source, author = 'libsdl-org'):
    response = requests.get(f"https://api.github.com/repos/{author}/{url_name}/releases/latest", headers=global_headers)
    metadata = response.json()
    version = metadata["name"].split()[-1]
    folder_path = name + '-' + version
    if not os.path.exists(folder_path):
        url = [x for x in metadata['assets'] if x['name'] == f'{name}-{version}.zip'][0]['browser_download_url']
        print('[posta] Downloading latest ' + name + ' release...')
        if not os.path.exists(name + '.zip'):
            download(url, name + '.zip')
        print('[posta] Extracting ' + name + '...')
        extract_zip(name + '.zip')
        #os.remove(name + '.zip')
        print('[posta] Compiling ' + name + '...')
        os.chdir(folder_path)
        if name == 'glew':
            os.chdir('build/cmake')
            resultcode = os.system("cmake -S . -B build -DCMAKE_PREFIX_PATH=\"" + path_to_lib_cmake + "\" -G \"MinGW Makefiles\" -DCMAKE_INSTALL_PREFIX=\"../../../w64devkit/x86_64-w64-mingw32\"")
        else:
            resultcode = os.system("cmake -S . -B build -DCMAKE_PREFIX_PATH=\"" + path_to_lib_cmake + "\" -G \"MinGW Makefiles\" -DCMAKE_INSTALL_PREFIX=\"../w64devkit/x86_64-w64-mingw32\""
            + (" -DSDL2MIXER_OPUS=OFF -DSDL2MIXER_FLAC=OFF -DSDL2MIXER_MOD=OFF -DSDL2MIXER_MIDI=OFF" if name == 'SDL2_mixer' else '')
            + (" -DSDL2IMAGE_JPG=OFF" if name == "SDL2_image" else ''))
        if resultcode != 0:
            return resultcode
        resultcode = os.system("cmake --build build")
        if resultcode != 0:
            return resultcode
        os.chdir('build')
        resultcode = os.system("make install")
        if resultcode != 0:
            return resultcode
        if name == 'glew':
            os.chdir('../..')
        os.chdir('../..')
    return 0

def install_sdl2():
    return install_sdl_like_from_source('SDL2', 'SDL', 'https://github.com/libsdl-org/SDL/releases')

def install_sdl2_image():
    return install_sdl_like_from_source('SDL2_image', 'SDL_image', 'https://github.com/libsdl-org/SDL_image/releases')

def install_sdl2_ttf():
    return install_sdl_like_from_source('SDL2_ttf', 'SDL_ttf', 'https://github.com/libsdl-org/SDL_ttf/releases')

def install_sdl2_mixer():
    return install_sdl_like_from_source('SDL2_mixer', 'SDL_mixer', 'https://github.com/libsdl-org/SDL_mixer/releases')

def install_sdl2_net():
    return install_sdl_like_from_source('SDL2_net', 'SDL_net', 'https://github.com/libsdl-org/SDL_net/releases')

def install_sdl_all():
    print("[posta] Installing SDL2...")
    run(install_sdl2)
    print("[posta] Installing freetype...")
    run(install_freetype)
    print("[posta] Installing SDL_ttf...")
    run(install_sdl2_ttf)
    print("[posta] Installing SDL_mixer...")
    run(install_sdl2_mixer)
    print("[posta] Installing SDL_net...")
    run(install_sdl2_net)
    print("[posta] Installing zlib...")
    run(install_zlib)
    print("[posta] Installing libpng...")
    run(install_libpng)
    print("[posta] Installing SDL_image...")
    run(install_sdl2_image)
    return 0

def install_glew():
    return install_sdl_like_from_source('glew', 'glew', 'https://github.com/nigels-com/glew/releases', 'nigels-com')

def install_lua():
    if not os.path.exists('lua-5.4.6'):
        print('[posta] Downloading lua-5.4.6.tar.gz...')
        if not os.path.exists('lua-5.4.6.tar.gz'):
            download("https://www.lua.org/ftp/lua-5.4.6.tar.gz", "lua-5.4.6.tar.gz")
        print('[posta] Extracting lua-5.4.6.tar.zip...')
        extract_tar_gz('lua-5.4.6.tar.gz')
        print('[posta] Compiling lua-5.4.6...')
        os.chdir('lua-5.4.6')
        os.mkdir('src/build')
        resultcode = os.system('make INSTALL_TOP="../../w64devkit/x86_64-w64-mingw32" PLAT=mingw TO_BIN="lua.exe luac.exe"')
        if resultcode != 0:
            return resultcode
        resultcode = os.system('make install INSTALL_TOP="../../w64devkit/x86_64-w64-mingw32" PLAT=mingw TO_BIN="lua.exe luac.exe"')
        if resultcode != 0:
            return resultcode
        os.chdir('..')
    return 0

def install_glm():
    response = requests.get("https://api.github.com/repos/g-truc/glm/releases/latest", headers=global_headers)
    metadata = response.json()
    version = metadata["name"].split()[-1]
    folder_path = "glm"
    if not os.path.exists(folder_path):
        url = [x for x in metadata['assets'] if x['name'] == f'glm-{version}.zip'][0]['browser_download_url']
        print('[posta] Downloading latest CMake release...')
        if not os.path.exists(folder_path + '.zip'):
            download(url, folder_path + '.zip')
        print('[posta] Extracting ' + folder_path + '.zip...')
        extract_zip(folder_path + '.zip')
        print('[posta] Copying glm files to w64devkit...')
        return os.system('cp -r ' + folder_path + '/glm w64devkit/x86_64-w64-mingw32/include/glm')
    return 0

def install_cmake():
    response = requests.get("https://api.github.com/repos/Kitware/CMake/releases/latest", headers=global_headers)
    metadata = response.json()
    version = metadata["tag_name"].split('v')[-1]
    folder_path = "cmake-" + version + "-windows-x86_64"
    if not os.path.exists(folder_path):
        url = [x for x in metadata['assets'] if x['name'] == f'{folder_path}.zip'][0]['browser_download_url']
        print('[posta] Downloading latest CMake release...')
        if not os.path.exists(folder_path + '.zip'):
            download(url, folder_path + '.zip')
        print('[posta] Extracting ' + folder_path + '.zip...')
        extract_zip(folder_path + '.zip')
        print('[posta] Copying cmake to w64devkit folder...')
        return os.system('cp -r ' + folder_path + '/*' + ' w64devkit/')
    return 0

def install_bullet():
    response = requests.get("https://api.github.com/repos/bulletphysics/bullet3/releases/latest", headers=global_headers)
    metadata = response.json()
    version = metadata["tag_name"]
    folder_path = f"bullet3-{version}"
    if not os.path.exists(folder_path):
        url = metadata["zipball_url"]
        print('[posta] Downloading latest bullet release...')
        if not os.path.exists(folder_path + '.zip'):
            download(url, folder_path + '.zip')
        print('[posta] Extracting ' + folder_path + '.zip...')
        folder = extract_zip(folder_path + '.zip')
        os.chdir(folder)
        print('[posta] Compiling bulletphysics...')
        #resultcode = os.system('cmake-gui . -DCMAKE_PREFIX_PATH=' + path_to_lib_cmake + ' -G "MinGW Makefiles" -DCMAKE_INSTALL_PREFIX="../w64devkit/x86_64-w64-mingw32"')
        resultcode = os.system('cmake -S . -B build -DCMAKE_PREFIX_PATH=' + path_to_lib_cmake + ' -G "MinGW Makefiles" -DCMAKE_INSTALL_PREFIX="../w64devkit/x86_64-w64-mingw32" -DBUILD_BULLET2_DEMOS=OFF -DBUILD_CPU_DEMOS=OFF -DBUILD_OPENGL3_DEMOS=OFF -DBUILD_EXTRAS=OFF -DBUILD_UNIT_TESTS=OFF -DINSTALL_LIBS=ON')
        if resultcode != 0:
            return resultcode
        os.chdir('build')
        resultcode = os.system('make install')
        os.chdir('../..')
        os.rename(folder, folder_path)
        return resultcode
    return 0

def install_zlib():
    folder_name = 'zlib-1.3'
    zip_filename = 'zlib-1.3.tar.gz'
    if not os.path.exists(folder_name):
        url = 'https://www.zlib.net/zlib-1.3.tar.gz'
        print('[posta] Downloading latest zlib release...')
        if not os.path.exists(zip_filename):
            download(url, zip_filename)
        print('[posta] Extracting ' + zip_filename + '...')
        folder = extract_tar_gz(zip_filename)
        os.chdir(folder)
        print('[posta] Compiling zlib...')
        resultcode = os.system("cmake -S . -B build -DCMAKE_PREFIX_PATH=\"" + path_to_lib_cmake + "\" -G \"MinGW Makefiles\" -DCMAKE_INSTALL_PREFIX=\"../w64devkit/x86_64-w64-mingw32\"")
        if resultcode != 0:
            return resultcode
        os.chdir('build')
        resultcode = os.system('make install')
        os.chdir('../..')
        return resultcode
    return 0

def install_libpng():
    folder_name = 'lpng1640'
    zip_filename = 'lpng1640.zip'
    if not os.path.exists(folder_name):
        url = 'https://sitsa.dl.sourceforge.net/project/libpng/libpng16/1.6.40/lpng1640.zip'
        print('[posta] Downloading latest libpng release...')
        if not os.path.exists(zip_filename):
            download(url, zip_filename)
        print('[posta] Extracting ' + zip_filename + '...')
        folder = extract_zip(zip_filename)
        os.chdir(folder)
        print('[posta] Compiling libpng...')
        resultcode = os.system("cmake -S . -B build -DCMAKE_PREFIX_PATH=\"" + path_to_lib_cmake + "\" -G \"MinGW Makefiles\" -DCMAKE_INSTALL_PREFIX=\"../w64devkit/x86_64-w64-mingw32\"")
        if resultcode != 0:
            return resultcode
        os.chdir('build')
        resultcode = os.system('make install')
        os.chdir('../..')
        return resultcode
    return 0

def run(func):
    if func() != 0:
        exit()

def make_development_environment():
    print("[posta] Installing required dependencies...")
    try:
        os.mkdir('internal')
    except FileExistsError:
        pass
    os.chdir('internal')
    if platform.system() == "Windows":
        print("[posta] Windows OS Detected")
        print("[posta] Installing w64devkit...")
        run(install_w64devkit)
        os.environ['PATH'] = os.environ.get('PATH', '') + ';' + os.path.abspath('w64devkit/bin')
        print("[posta] Installing CMake...")
        run(install_cmake)
        print("[posta] Installing SDL libraries...")
        run(install_sdl_all)
        print("[posta] Installing GLEW...")
        run(install_glew)
        print("[posta] Installing Lua...")
        run(install_lua)
        print("[posta] Installing glm...")
        run(install_glm)
        print("[posta] Installing Bullet...")
        run(install_bullet)
    else:
        print("[posta] !! Automatically compiling and installing dependencies only works in windows at the time !!")
        print("[posta] !! Continuing with the rest of the installation... !!")
        if platform.system() == "Linux":
            print("[posta] Linux Kernel Detected")
            global path_to_lib_cmake
            path_to_lib_cmake = '/usr/lib/cmake/'
    if path_to_lib_cmake != None:
        print("[posta] Retwriting path_to_lib_cmake.txt...")
        with open('../../path_to_lib_cmake.txt', 'w') as file:
            file.write(path_to_lib_cmake)
    print("[posta] Creating generator file...")
    with open('generator.txt', 'w') as file:
        if platform.system() == "Windows":
            file.write('MinGW Makefiles')
        elif platform.system() == "Linux":
            file.write('Unix Makefiles')
    print("[posta] Done! -- Note: You can delete everything inside bin/internal now, except for w64devkit and generator.txt")

if __name__ == "__main__":
    make_development_environment()

