#include <filesystem>
#include <fstream>
#include <string>
#include <stdexcept>

namespace fs = std::filesystem;

using namespace fs;
using namespace std;

string read(const string& path) {
    string result;
    ifstream file(path);
    if (!file) {
        throw runtime_error("could not open file: " + path);
    }
    string line;
    while (getline(file, line)) {
        result += line + "\n";
    }
    return result;
}
