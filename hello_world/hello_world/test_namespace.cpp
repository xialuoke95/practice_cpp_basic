#include <iostream>
#include <unordered_map>
#include "test_namespace.h"

float safe_div(float a, float b){
    if (b == 0) {
        return -1;
    } else {
        return (a / b);
    }
}

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
    int bb = 0;
    std::cout << safe_div(aa, bb) << std::endl;
    std::cout << test::safe_div(aa, bb) << std::endl;

    return 0;
}