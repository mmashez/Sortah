#include <iostream>
#include <string>
#include <cstdlib>

inline int start(const std::string& softwarePath, bool verbose = false) {
    std::string command = "powershell -WindowStyle Hidden -Command \"Start-Process -FilePath '" + softwarePath + "' -WindowStyle Hidden\"";

    if (verbose) {
        std::cout << "attempting command: " << command << std::endl;
    }

    return system(command.c_str());
}
