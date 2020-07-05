#include <iostream>
#include <unordered_map>
#include "test_include.h"

float test::safe_div(float a, float b){
    if (b == 0) {
        return 0;
    } else {
        return (a / b);
    }
}

int main(int argc, char **argv) {
    std::cout << "hello_world!" << std::endl;
    int aa = 1;
    int bb = 2;
    std::cout << test::safe_div(aa, bb) << std::endl;

    return 0;
}