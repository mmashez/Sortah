#include "general.h"
#include <iostream>
int main(int argc, char* argv[]) {
    init(argc, argv, true);
    verbose = false;
    return worker();
}