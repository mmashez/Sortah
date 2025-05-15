// made for handling strings, integers, etc.. this is mostly for API.

#ifndef TYPES_H
#define TYPES_H

#include <string>
#include <iostream>
#include <vector>
#include <future>
#include <variant>
#include <utility>

using namespace std;

using Object = vector<pair<string, variant<int, vector<string>, string>>>;

class Types {
    public:
        Object rates = {
            {"STRING_MAXIMUM_CHARACTERS", 4096},
            {"STRING_MINIMUM_CHARACTERS", 0},
            {"ILLEGAL_CHARACTERS", {"<", ">", ":", "\"", "`", "?", "*"}}
        };
    private:
        int a = 0;
};

#endif 