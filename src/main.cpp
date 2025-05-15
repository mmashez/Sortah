#include <iostream>
#include <vector>
#include <windows.h>
#include <shellapi.h>
#include <general.h>

namespace fs = std::filesystem;

using namespace std;
using namespace fs;
using namespace paths;

int main(int argc, char* argv[]) {
    init(argc, argv, false);
    
    for (int i = 1; i < argc; ++i) {
        if (string(argv[i]) == "-verbose") {
            setVerbose(true);
            cout << "verbose mode enabled!" << endl;
        }
    }
    return handle_logic();

}
