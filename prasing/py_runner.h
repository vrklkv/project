#include <iostream>
#include <string>
#include <cstdlib> // Для system()

int run_python_script(const std::string& script_path) {
    std::string command = "python " + script_path;
    int result = std::system(command.c_str());
    if (result == 0) {
        std::cout << "Python script executed successfully." << std::endl;
    } else {
        std::cerr << "Error: Python script execution failed with code: " << result << std::endl;
    }
    return result; //возвращает код завершения процесса
}
