#include <iostream>
#include <cstdlib>
#include <filesystem>
#include <windows.h>

int main(int argc, char* argv[])
{
    char buffer[MAX_PATH];
    DWORD length = GetModuleFileNameA(NULL, buffer, MAX_PATH);
    if (length == 0)
	{
        std::cerr << "Error getting the executable path. Error code: " << GetLastError() << std::endl;
        return 1;
    }
    buffer[length] = '\0';
	auto actual_program_path = std::filesystem::path(buffer);

    std::string command = "python \"" + (actual_program_path.parent_path() / actual_program_path.filename().stem()).string() + ".py\"";

    for (int i = 1; i < argc; ++i)
	{
        command += " ";
        command += argv[i];
    }
	std::system(command.c_str());

    return 0;
}

