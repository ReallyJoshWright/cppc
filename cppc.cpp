#include <iostream>
#include <string>
#include <cstdlib>
#include <filesystem>
#include <fstream>

#ifdef _WIN32
    std::string compiler = "mingw";
#elif __APPLE__
    std::string compiler = "clang++";
#elif __linux__
    std::string compiler = "g++";
#else
    std::string compiler = "g++";
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

void
build() {
    std::string command = compiler
        + " -std=c++23 build.cpp -o build && ./build";
    system(command.c_str());
    std::string clean = "rm -rf build";
    system(clean.c_str());
}

void
createBuildCpp(std::filesystem::path build_cpp) {
    std::ofstream file(build_cpp);
    file << "#include \"builder.h\"\n" << std::endl;
    file << "int main() {" << std::endl;
    file << "    Builder builder;\n" << std::endl;
    file << "    Vector<Debug> debug;" << std::endl;
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
    file << "    });\n" << std::endl;
    file << "    builder.build();" << std::endl;
    file << "}" << std::endl;
    file.close();
}

void
createMainCpp(std::filesystem::path main_cpp) {
    std::ofstream file(main_cpp);
    file << "#include<iostream>\n" << std::endl;
    file << "int main() {" << std::endl;
    file << "    std::cout << \"Hello World!\" << std::endl;\n" << std::endl;
    file << "    return 0;" << std::endl;
    file << "}" << std::endl;
    file.close();
}

void
createProject(std::string project_name) {
    std::filesystem::path name = project_name;
    std::filesystem::path src = "src";
    std::filesystem::path dirs = name / src;
    std::filesystem::create_directory(dirs);
    std::filesystem::path main_cpp = dirs / "main.cpp";
    std::filesystem::path build_cpp = name / "build.cpp";
    createBuildCpp(build_cpp);
    createMainCpp(main_cpp);
    // TODO: run cppc build in name dir
    // TODO: fix .src/main.cpp compile json bug
    // TODO: setup auto install script and paths
}

int
main(int argc, char *argv[]) {
    if (argc == 1) {
        printHelp();
    } else if (argc == 2) {
        std::string command = argv[1];
        if (command == "build") {
            build();
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

    return 0;
}
