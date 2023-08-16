import os

def replace_in_file(file_path, old_string, new_string):
    with open(file_path, 'r') as file:
        content = file.read()
    content = content.replace(old_string, new_string)
    with open(file_path, 'w') as file:
        file.write(content)

def recursively_replace_in_directory(directory, old_string, new_string):
    for root, _, files in os.walk(directory):
        for filename in files:
            file_path = os.path.join(root, filename)
            replace_in_file(file_path, old_string, new_string)
            print(f"Replaced in {file_path}")

if __name__ == "__main__":
    directory_path = "posta"
    old_string = "Engine"
    new_string = "Posta"

    recursively_replace_in_directory(directory_path, old_string, new_string)
