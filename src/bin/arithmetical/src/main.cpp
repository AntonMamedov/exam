#include <iostream>

#include "service.hpp"



int main(int argc, char *argv[]) {
    if (argc < 2) {
        return 1;
    }

    std::string sAddr = argv[1];

    arithmetical::service::runService(sAddr);
    return 0;
}