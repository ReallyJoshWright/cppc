#pragma once

#include <cstdlib>
#include <iostream>
#include <vector>
#include <filesystem>
#include <fstream>
#include <string>

///////////////////////////////////////////////////////////////////////////////
// prepocessor statements
///////////////////////////////////////////////////////////////////////////////
#ifdef _WIN32
    inline std::string os = "windows";
    inline std::string compiler = "g++";
    inline std::string compiler_full_path = "";
#elif __APPLE__
    inline std::string os = "macos";
    inline std::string compiler = "clang++";
    inline std::string compiler_full_path = "";
#elif __linux__
    inline std::string os = "linux";
    inline std::string compiler = "g++";
    inline std::string compiler_full_path = "/usr/bin/g++";
#else
    inline std::string os = "linux";
    inline std::string compiler = "g++";
    inline std::string compiler_full_path = "/usr/bin/g++";
#endif

///////////////////////////////////////////////////////////////////////////////
// enums
///////////////////////////////////////////////////////////////////////////////
enum class Version {
    V11,
    V14,
    V17,
    V20,
    V23,
};

enum class Debug {
    G,
    Wall,
    Wextra,
    Pedantic,
};

enum class Optimize {
    Embedded,
    Release,
    Debug,
};

enum class Targets {
    Linux,
    Windows,
    MacOS,
};

///////////////////////////////////////////////////////////////////////////////
// structs
///////////////////////////////////////////////////////////////////////////////
struct Options {
    std::string name;
    std::string root_source_file;
    Version version;
    std::vector<Debug> debug;
    Optimize optimize;
    Targets target;
};

///////////////////////////////////////////////////////////////////////////////
// classes
///////////////////////////////////////////////////////////////////////////////
class Builder {
    public:
        ///////////////////////////////////////////////////////////////////////
        // public methods
        ///////////////////////////////////////////////////////////////////////
        Builder(int argc, char *argv[]) {
            yes_run = false;

            if (argc > 1) {
                std::string cmd = argv[1];
                if (cmd == "run") {
                    yes_run = true;
                }
            }
        }

        void setOptions(Options options) {
            this->options = options;
        }

        void addIncludeDir(std::string dir) {
            include_dirs.push_back(dir);
        }

        void addSourceFile(std::string source_file) {
            source_files.push_back(source_file);
        }

        void addLibDir(std::string dir) {
            lib_dirs.push_back(dir);
        }

        void addLibrary(std::string lib) {
            libs.push_back(lib);
        }

        void build() {
            createCompileCommands();
            std::string exe = getCompileCommand();
            std::system(exe.c_str());

            if (yes_run) {
                std::string run = "./" + options.name;
                std::system(run.c_str());
            }
        }

    private:
        ///////////////////////////////////////////////////////////////////////
        // private members
        ///////////////////////////////////////////////////////////////////////
        Options options;
        std::vector<std::string> include_dirs;
        std::vector<std::string> source_files;
        std::vector<std::string> lib_dirs;
        std::vector<std::string> libs;
        bool yes_run;

        ///////////////////////////////////////////////////////////////////////
        // private methods
        ///////////////////////////////////////////////////////////////////////
        void createCompileCommands() {
            std::string filename = "compile_commands.json";
            std::ofstream file(filename, std::ios::out);

            if (!file.is_open()) {
                std::cerr << "Error: Could not open the file "
                          << filename << std::endl;
                return;
            }

            std::vector<std::string> debug = getDebugStringList(options.debug);
            std::string optimize = getOptimizeString(options.optimize);
            std::string version = getVersionString(options.version);
            std::filesystem::path cwd_path = std::filesystem::current_path();
            std::string cwd = cwd_path.string();
            std::string sub_root_source_path = cleanUpSubDir(options.root_source_file);

            file << "[" << std::endl;
            file << "  {" << std::endl;
            file << "    \"arguments\": [" << std::endl;
            file << "      \"" + compiler_full_path + "\"," << std::endl;
            file << "      \"-c\"," << std::endl;
            for (auto d : debug) {
                file << "      \"" << d << "\"," << std::endl;
            }
            file << "      \"" << optimize << "\"," << std::endl;
            file << "      \"" << version << "\"," << std::endl;
            file << "      \"-I" << getHomePath() << "/.config/.cppc\"," << std::endl;
            for (auto i : include_dirs) {
                file << "      \"" << i << "\"," << std::endl;
            }
            for (auto libd : lib_dirs) {
                file << "      \"" << libd << "\"," << std::endl;
            }
            file << "      \"-o\"," << std::endl;
            file << "      \"" << options.name << "\"," << std::endl;
            file << "      \"" << options.root_source_file << "\"," << std::endl;
            file << "    ]," << std::endl;
            file << "    \"directory\": \"" << cwd << "\"," << std::endl;
            file << "    \"file\": \"" << cwd << "/" << sub_root_source_path << "\"," << std::endl;
            file << "    \"output\": \"" << cwd << "/" << options.name << "\"" << std::endl;
            if (source_files.size() == 0) {
                file << "  }" << std::endl;
            } else {
                file << "  }," << std::endl;
            }
            unsigned long counter = source_files.size();
            for (auto f : source_files) {
                std::string minus_prefix = removeFirstTwoChars(f);
                file << "  {" << std::endl;
                file << "    \"arguments\": [" << std::endl;
                file << "      \"" + compiler_full_path + "\"," << std::endl;
                file << "      \"-c\"," << std::endl;
                for (auto d : debug) {
                    file << "      \"" << d << "\"," << std::endl;
                }
                file << "      \"" << optimize << "\"," << std::endl;
                file << "      \"" << version << "\"," << std::endl;
                for (auto i : include_dirs) {
                    file << "      \"" << i << "\"," << std::endl;
                }
                for (auto libd : lib_dirs) {
                    file << "      \"" << libd << "\"," << std::endl;
                }
                file << "      \"-o\"," << std::endl;
                file << "      \"" << options.name << "\"," << std::endl;
                file << "      \"" << f << "\"," << std::endl;
                file << "    ]," << std::endl;
                file << "    \"directory\": \"" << cwd << "\"," << std::endl;
                file << "    \"file\": \"" << cwd << "/" << minus_prefix << "\"," << std::endl;
                file << "    \"output\": \"" << cwd << "/" << options.name << "\"" << std::endl;
                counter--;
                if (counter == 0) {
                    file << "  }" << std::endl;
                } else {
                    file << "  }," << std::endl;
                }
            }
            file << "]" << std::endl;

            file.close();
        }

        std::vector<std::string> getDebugStringList(
            std::vector<Debug> options
        ) {
            std::vector<std::string> list;
            std::string value = "";

            for (auto d : options) {
                if (d == Debug::G) {
                    value = "-g";
                    list.push_back(value);
                } else if (d == Debug::Wall) {
                    value = "-Wall";
                    list.push_back(value);
                } else if (d == Debug::Wextra) {
                    value = "-Wextra";
                    list.push_back(value);
                } else if (d == Debug::Pedantic) {
                    value = "-pedantic";
                    list.push_back(value);
                }
            }

            return list;
        }

        std::string getOptimizeString(Optimize option) {
            std::string value = "";
            if (option == Optimize::Debug) {
                value = "-O0";
            } else if (option == Optimize::Embedded) {
                value = "-Os";
            } else if (option == Optimize::Release) {
                value = "-O3";
            }

            return value;
        }

        std::string getVersionString(Version option) {
            std::string value = "";
            if (option == Version::V11) {
                value = "-std=c++11";
            } else if (option == Version::V14) {
                value = "-std=c++14";
            } else if (option == Version::V17) {
                value = "-std=c++17";
            } else if (option == Version::V20) {
                value = "-std=c++20";
            } else if (option == Version::V23) {
                value = "-std=c++23";
            }

            return value;
        }

        std::string getCompileCommand() {
            std::string debug_string = "";
            for (Debug d : options.debug) {
                if (d == Debug::G) {
                    debug_string += " -g ";
                } else if (d == Debug::Wall) {
                    debug_string += " -Wall ";
                } else if (d == Debug::Wextra) {
                    debug_string += " -Wextra ";
                } else if (d == Debug::Pedantic) {
                    debug_string += " -pedantic ";
                }
            }

            std::string optimize_string = "";
            if (options.optimize == Optimize::Debug) {
                optimize_string = "-O0";
            } else if (options.optimize == Optimize::Release) {
                optimize_string = "-O3";
            } else if (options.optimize == Optimize::Embedded) {
                optimize_string = "-Os";
            }

            std::string version_string = "";
            if (options.version == Version::V11) {
                version_string = "-std=c++11";
            } else if (options.version == Version::V14) {
                version_string = "-std=c++14";
            } else if (options.version == Version::V17) {
                version_string = "-std=c++17";
            } else if (options.version == Version::V20) {
                version_string = "-std=c++20";
            } else if (options.version == Version::V23) {
                version_string = "-std=c++23";
            }

            if (os == "linux") {
                if (options.target == Targets::Linux) {
                    compiler = "g++";
                } else if (options.target == Targets::Windows) {
                    compiler = "x86_64-w64-mingw32-g++";
                }
            } else if (os == "windows") {
                if (options.target == Targets::Windows) {
                    compiler = "g++";
                }
            }

            unsigned long dir_count = include_dirs.size();
            unsigned long file_count = source_files.size();
            unsigned long lib_count = libs.size();

            if (dir_count == 0 && file_count == 0 && lib_count == 0) {
                std::string exe = compiler + " "
                    + debug_string
                    + optimize_string + " "
                    + version_string + " "
                    + options.root_source_file
                    + " -o "
                    + options.name;

                if (os == "windows") {
                    exe = exe
                        + " -static-libgcc"
                        + " -static-libstdc++";
                }

                return exe;
            } else {
                std::string dir_string = "";
                std::string file_string = "";
                std::string lib_string = "";
                std::string lib_dir_string = "";

                for (auto dir : include_dirs) {
                    std::string temp = " " + dir + " ";
                    dir_string += temp;
                }

                for (auto f : source_files) {
                    std::string temp = " " + f + " ";
                    file_string += temp;
                }

                for (auto libd : lib_dirs) {
                    std::string temp = " " + libd + " ";
                    lib_dir_string += temp;
                }

                for (auto lib : libs) {
                    std::string temp = " " + lib + " ";
                    lib_string += temp;
                }

                std::string exe = compiler + " "
                    + debug_string
                    + optimize_string + " "
                    + version_string + " "
                    + dir_string
                    + lib_dir_string
                    + file_string
                    + options.root_source_file
                    + " -o "
                    + options.name
                    + lib_string;

                if (os == "windows") {
                    exe = exe
                        + " -static-libgcc"
                        + " -static-libstdc++";
                }

                return exe;
            }
        }

        std::string removeFirstTwoChars(std::string item) {
            return item.erase(0, 2);
        }

        std::string getHomePath() {
            std::string home = std::getenv("HOME");
            return home;
        }

        std::string cleanUpSubDir(std::string root_source_file) {
            if (root_source_file.substr(0, 2) == "./") {
                return root_source_file.erase(0, 2);
            } else {
                return root_source_file;
            }
        }
};
