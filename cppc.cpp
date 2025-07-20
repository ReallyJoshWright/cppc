#include <iostream>
#include <string>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <chrono>
#include <thread>

///////////////////////////////////////////////////////////////////////////////
// preprocessor commands
///////////////////////////////////////////////////////////////////////////////
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

///////////////////////////////////////////////////////////////////////////////
// function prototypes
///////////////////////////////////////////////////////////////////////////////
void handleLinuxArgs(std::string cmd, std::string opt1);
void buildLinux(bool is_verbose);
void runLinux(bool is_verbose);
void runLinuxTest(bool is_verbose);
void createLinuxBuildCpp(std::filesystem::path build_cpp);
void createLinuxCompileCommands(std::string project_name);

void handleWindowsArgs(std::string cmd, std::string opt1);
void buildWindows(bool is_verbose);
void runWindows(bool is_verbose);
void runWindowsTest(bool is_verbose);
void createWindowsBuildCpp(std::filesystem::path build_cpp);
void createWindowsCompileCommands(std::string project_name);

void handleMacosArgs(std::string cmd, std::string opt1);

void printHelp();
void handleArgs(std::string cmd, std::string opt1);
bool buildFileExists();
void createMainCpp(std::filesystem::path main_cpp);
void createProject(std::string project_name);

///////////////////////////////////////////////////////////////////////////////
// linux functions
///////////////////////////////////////////////////////////////////////////////
void
handleLinuxArgs(std::string cmd, std::string opt1) {
    if (cmd == "--help") {
        printHelp();
    } else if (cmd == "build" && opt1 != "-v") {
        buildLinux(false);
    } else if (cmd == "build" && opt1 == "-v") {
        buildLinux(true);
    } else if (cmd == "run" && opt1 != "-v") {
        runLinux(false);
    } else if (cmd == "run" && opt1 == "-v") {
        runLinux(true);
    } else if (cmd == "test" && opt1 != "-v") {
        runLinuxTest(false);
    } else if (cmd == "test" && opt1 == "-v") {
        runLinuxTest(true);
    } else if (cmd == "new" && opt1 != "") {
        createProject(opt1);
    } else {
        printHelp();
    }
}

void
buildLinux(bool is_verbose) {
    if (!buildFileExists()) {
        std::cout << "No build.cpp file exists." << std::endl;
        return;
    }

    std::string command = compiler
        + " -std=c++23 -I$HOME/.config/.cppc build.cpp -o build && ./build";
    std::string clean = "rm -rf build";

    if (is_verbose) {
        std::cout << command << std::endl;
        system(command.c_str());
        std::cout << clean << std::endl;
        system(clean.c_str());
    } else {
        system(command.c_str());
        system(clean.c_str());
    }
}

void
runLinux(bool is_verbose) {
    if (!buildFileExists()) {
        std::cout << "No build.cpp file exists." << std::endl;
        return;
    }

    std::string command = compiler
        + " -std=c++23 -I$HOME/.config/.cppc build.cpp -o "
        "build && ./build run";
    std::string clean = "rm -rf build";

    if (is_verbose) {
        std::cout << command << std::endl;
        std::cout << clean << std::endl;
        system(command.c_str());
        system(clean.c_str());
    } else {
        system(command.c_str());
        system(clean.c_str());
    }
}

void
runLinuxTest(bool is_verbose) {
    if (!buildFileExists()) {
        std::cout << "No build.cpp file exists." << std::endl;
        return;
    }
}

void
createLinuxBuildCpp(std::filesystem::path build_cpp) {
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
createLinuxCompileCommands(std::string project_name) {
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

///////////////////////////////////////////////////////////////////////////////
// windows functions
///////////////////////////////////////////////////////////////////////////////
void
handleWindowsArgs(std::string cmd, std::string opt1) {
    if (cmd == "--help") {
        printHelp();
    } else if (cmd == "build" && opt1 != "-v") {
        buildWindows(false);
    } else if (cmd == "build" && opt1 == "-v") {
        buildWindows(true);
    } else if (cmd == "run" && opt1 != "-v") {
        runWindows(false);
    } else if (cmd == "run" && opt1 == "-v") {
        runWindows(true);
    } else if (cmd == "test" && opt1 != "-v") {
        runWindowsTest(false);
    } else if (cmd == "test" && opt1 == "-v") {
        runWindowsTest(true);
    } else if (cmd == "new" && opt1 != "") {
        createProject(opt1);
    } else {
        printHelp();
    }
}

void
buildWindows(bool is_verbose) {
    if (!buildFileExists()) {
        std::cout << "No build.cpp file exists." << std::endl;
        return;
    }

    std::string command = "cmd.exe /c \""
        + compiler
        + " /std:c++latest /EHsc /I\"%USERPROFILE%\\.config\\.cppc\" "
        "build.cpp /Febuild.exe\"";
    std::string command_exe = "cmd.exe /c \"build.exe\"";
    std::string clean = "del build.exe";

    if (is_verbose) {
        std::cout << command << std::endl;
        system(command.c_str());
        std::cout << command_exe << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(5));
        system(command_exe.c_str());
        std::cout << clean << std::endl;
        system(clean.c_str());
    } else {
        system(command.c_str());
        system(command_exe.c_str());
        std::this_thread::sleep_for(std::chrono::seconds(5));
        system(clean.c_str());
    }
}

void
runWindows(bool is_verbose) {
    if (!buildFileExists()) {
        std::cout << "No build.cpp file exists." << std::endl;
        return;
    }

    std::string command = "cmd.exe /c \""
        + compiler
        + " /std:c++latest /EHsc /I\"%USERPROFILE%\\.config\\.cppc\" "
        "build.cpp /Febuild.exe\"";
    std::string command_exe = "cmd.exe /c \"build.exe run\"";
    std::string clean = "del build.exe";

    if (is_verbose) {
        std::cout << command << std::endl;
        std::cout << command_exe << std::endl;
        std::cout << clean << std::endl;
        system(command.c_str());
        system(command_exe.c_str());
        std::this_thread::sleep_for(std::chrono::seconds(5));
        system(clean.c_str());
    } else {
        system(command.c_str());
        system(command_exe.c_str());
        std::this_thread::sleep_for(std::chrono::seconds(5));
        system(clean.c_str());
    }
}

void
runWindowsTest(bool is_verbose) {
    if (!buildFileExists()) {
        std::cout << "No build.cpp file exists." << std::endl;
        return;
    }
}

void
createWindowsBuildCpp(std::filesystem::path build_cpp) {
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
    file << "        .name = \"app.exe\"," << std::endl;
    file << "        .root_source_file = \"src/main.cpp\"," << std::endl;
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
createWindowsCompileCommands(std::string project_name) {
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

///////////////////////////////////////////////////////////////////////////////
// macos functions
///////////////////////////////////////////////////////////////////////////////
void
handleMacosArgs(std::string cmd, std::string opt1) {
    std::cout << "Not implemented for MacOs yet" << std::endl;
}

///////////////////////////////////////////////////////////////////////////////
// general functions
///////////////////////////////////////////////////////////////////////////////
void
printHelp() {
    std::string message = "Usage:\n"
        "  cppc [commands] [arguments]\n\n"
        "Commands\n"
        "  build              run the build.cpp file to create an executable\n"
        "  run                build and run the project\n"
        "  test               run tests for the project\n"
        "  new                create a new project with the name given\n"
        "\nArguments\n"
        "  <project name>     example: cppc new <project_name>\n"
        "  -v                 verbose for build, run, and test commands\n";
    std::cout << message << std::endl;
}

void
handleArgs(std::string cmd, std::string opt1) {
    if (os == "linux") {
        handleLinuxArgs(cmd, opt1);
    } else if (os == "windows") {
        handleWindowsArgs(cmd, opt1);
    } else if (os == "macos") {
        handleMacosArgs(cmd, opt1);
    } else {
        std::cout << "This operating system is not supported" << std::endl;
    }
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
createProject(std::string project_name) {
    std::filesystem::path name = project_name;
    std::filesystem::path src = "src";
    std::filesystem::path dirs = name / src;
    std::filesystem::create_directories(dirs);
    std::filesystem::path main_cpp = dirs / "main.cpp";
    std::filesystem::path build_cpp = name / "build.cpp";
    createMainCpp(main_cpp);

    if (os == "linux") {
        createLinuxBuildCpp(build_cpp);
        createLinuxCompileCommands(project_name);
    } else if (os == "windows") {
        createWindowsBuildCpp(build_cpp);
        createWindowsCompileCommands(project_name);
    }
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

///////////////////////////////////////////////////////////////////////////////
// main function
///////////////////////////////////////////////////////////////////////////////
int
main(int argc, char *argv[]) {
    std::string tool_name = "";
    std::string command = "";
    std::string option1 = "";

    if (argc == 1) {
        tool_name = argv[0];
    } else if (argc == 2) {
        tool_name = argv[0];
        command = argv[1];
    } else if (argc == 3) {
        tool_name = argv[0];
        command = argv[1];
        option1 = argv[2];
    } else if (argc == 3) {
        tool_name = argv[0];
        command = argv[1];
        option1 = argv[2];
    } else {
        printHelp();
    }

    handleArgs(command, option1);

    return 0;
}
