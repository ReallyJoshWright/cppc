#include <iostream>
#include <string>
#include <cstdlib>
#include <filesystem>
#include <fstream>

#ifdef _WIN32
    std::string os = "windows";
    std::string compiler = "cl";
    std::string compiler_full_path = "";
#elif __APPLE__
    std::string os = "macos";
    std::string compiler = "clang++";
    std::string compiler_full_path = "";
#elif __linux__
    std::string os = "linux";
    std::string compiler = "g++";
    std::string compiler_full_path = "/usr/bin/g++";
#else
    std::string os = "linux";
    std::string compiler = "g++";
    std::string compiler_full_path = "/usr/bin/g++";
#endif

void
printHelp() {
    std::string message = "Usage:\n"
        "  cppc [commands] [arguments]\n\n"
        "Commands\n"
        "  build               run the build.cpp file to create an executable"
        "  new                 create a new project with the name given\n"
        "\nArguments\n"
        "  <project name>      example: cppc new <project_name>\n";
    std::cout << message << std::endl;
}

bool
buildFileExists() {
    std::filesystem::path build_file = "build.cpp";
    if (std::filesystem::exists(build_file)) {
        return true;
    } else {
        return false;
    }
}

void
buildLinux() {
    if (!buildFileExists()) {
        std::cout << "No build.cpp file exists." << std::endl;
        return;
    }

    std::string command = compiler
        + " -std=c++23 -I$HOME/.config/.cppc build.cpp -o build && ./build";
    system(command.c_str());
    std::string clean = "rm -rf build";
    system(clean.c_str());
}

void
buildWindows() {
    if (!buildFileExists()) {
        std::cout << "No build.cpp file exists." << std::endl;
        return;
    }

    std::string command = compiler
        + " /std:c++latest /EHsc /I\"%USERPROFILE%\\.config\\.cppc\" build.cpp /Febuild.exe";
    system(command.c_str());
    std::string exe_cmd = "build.exe";
    system(exe_cmd.c_str());
    std::string clean = "del build.exe";
    system(clean.c_str());
}

void
runLinux() {
    if (!buildFileExists()) {
        std::cout << "No build.cpp file exists." << std::endl;
        return;
    }

    std::string command = compiler
        + " -std=c++23 -I$HOME/.config/.cppc build.cpp -o build && ./build run";
    system(command.c_str());
    std::string clean = "rm -rf build";
    system(clean.c_str());
}

void
runWindows() {
    if (!buildFileExists()) {
        std::cout << "No build.cpp file exists." << std::endl;
        return;
    }

    std::string command = compiler
        + " /std:c++latest /EHsc /I$HOME/.config/.cppc build.cpp /Febuild.exe && build.exe run";
    system(command.c_str());
    std::string clean = "rm -rf build";
    system(clean.c_str());
}

void
createBuildCpp(std::filesystem::path build_cpp) {
    std::ofstream file(build_cpp);
    file << "#include <vector>\n" << std::endl;
    file << "#include \"builder.h\"\n" << std::endl;
    file << "int main(int argc, char *argv[]) {" << std::endl;
    file << "    Builder builder(argc, argv);\n" << std::endl;
    file << "    std::vector<Debug> debug;" << std::endl;
    file << "    debug.push_back(Debug::G);" << std::endl;
    file << "    debug.push_back(Debug::Wall);" << std::endl;
    file << "    debug.push_back(Debug::Wextra);" << std::endl;
    file << "    debug.push_back(Debug::Pedantic);\n" << std::endl;
    file << "    builder.setOptions(Options{" << std::endl;
    file << "        .name = \"app\"," << std::endl;
    file << "        .root_source_file = \"./src/main.cpp\"," << std::endl;
    file << "        .version = Version::V23," << std::endl;
    file << "        .debug = debug," << std::endl;
    file << "        .optimize = Optimize::Debug," << std::endl;
    file << "        .target = Targets::Linux," << std::endl;
    file << "    });\n" << std::endl;
    file << "    builder.build();" << std::endl;
    file << "}" << std::endl;
    file.close();
}

void
createMainCpp(std::filesystem::path main_cpp) {
    std::ofstream file(main_cpp);
    file << "#include <iostream>\n" << std::endl;
    file << "int main() {" << std::endl;
    file << "    std::cout << \"Hello World!\" << std::endl;\n" << std::endl;
    file << "    return 0;" << std::endl;
    file << "}" << std::endl;
    file.close();
}

void
createCompileCommands(std::string project_name) {
    std::filesystem::path project_name_path = project_name;
    std::filesystem::path cwd_path = std::filesystem::current_path();
    std::filesystem::path full_path = cwd_path / project_name_path;
    std::string cwd = full_path.string();

    std::filesystem::path filename_path = "compile_commands.json";
    std::filesystem::path filename_full_path = full_path / filename_path;

    std::string filename = filename_full_path.string();
    std::ofstream file(filename, std::ios::out);

    if (!file.is_open()) {
        std::cerr << "Error: Could not open the file "
                  << filename << std::endl;
        return;
    }

    file << "[" << std::endl;
    file << "  {" << std::endl;
    file << "    \"arguments\": [" << std::endl;
    file << "      \"" + compiler_full_path + "\"," << std::endl;
    file << "      \"-c\"," << std::endl;
    file << "      \"-g\"," << std::endl;
    file << "      \"-Wall\"," << std::endl;
    file << "      \"-Wextra\"," << std::endl;
    file << "      \"-pedantic\"," << std::endl;
    file << "      \"" << "-O0" << "\"," << std::endl;
    file << "      \"" << "-std=c++23" << "\"," << std::endl;
    file << "      \"-I" << std::getenv("HOME") << "/.config/.cppc\"," << std::endl;
    file << "      \"-o\"," << std::endl;
    file << "      \"app\"," << std::endl;
    file << "      \"./src/main.cpp\"," << std::endl;
    file << "    ]," << std::endl;
    file << "    \"directory\": \"" << cwd << "\"," << std::endl;
    file << "    \"file\": \"" << cwd << "/" << "src/main.cpp" << "\"," << std::endl;
    file << "    \"output\": \"" << cwd << "/" << "app" << "\"" << std::endl;
    file << "  }" << std::endl;
    file << "]" << std::endl;

    file.close();
}

void
createProject(std::string project_name) {
    std::filesystem::path name = project_name;
    std::filesystem::path src = "src";
    std::filesystem::path dirs = name / src;
    std::filesystem::create_directories(dirs);
    std::filesystem::path main_cpp = dirs / "main.cpp";
    std::filesystem::path build_cpp = name / "build.cpp";
    createBuildCpp(build_cpp);
    createMainCpp(main_cpp);
    createCompileCommands(project_name);
}

void
runWindowsTest() {
    if (!buildFileExists()) {
        std::cout << "No build.cpp file exists." << std::endl;
        return;
    }
}

void
runLinuxTest() {
    if (!buildFileExists()) {
        std::cout << "No build.cpp file exists." << std::endl;
        return;
    }
}

int
main(int argc, char *argv[]) {
    if (os == "linux") {
        if (argc == 1) {
            printHelp();
        } else if (argc == 2) {
            std::string command = argv[1];
            if (command == "build") {
                buildLinux();
            } else if (command == "run") {
                runLinux();
            } else if (command == "test") {
                runLinuxTest();
            } else {
                printHelp();
            }
        } else if (argc == 3) {
            std::string command = argv[1];
            if (command == "new") {
                std::string project_name = argv[2];
                createProject(project_name);
            } else {
                printHelp();
            }
        } else {
            printHelp();
        }
    } else if (os == "windows") {
        if (argc == 1) {
            printHelp();
        } else if (argc == 2) {
            std::string command = argv[1];
            if (command == "build") {
                buildWindows();
            } else if (command == "run") {
                runWindows();
            } else if (command == "test") {
                runWindowsTest();
            } else {
                printHelp();
            }
        } else if (argc == 3) {
            std::string command = argv[1];
            if (command == "new") {
                std::string project_name = argv[2];
                createProject(project_name);
            } else {
                printHelp();
            }
        } else {
            printHelp();
        }
    } else if (os == "macos") {
    } else {
        printHelp();
    }

    return 0;
}
