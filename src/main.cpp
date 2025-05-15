#include <filesystem>
#include <iostream>
#include <shellapi.h>
#include <windows.h>
#include <general.h>

int main(int argc, char* argv[]) {
    init(argc, argv, false);

    if (argc > 1 && std::string(argv[1]) == "-verbose") {
        setVerbose(true);
        std::cout << "verbose mode enabled!" << std::endl;
    }

    return handle_logic();
}