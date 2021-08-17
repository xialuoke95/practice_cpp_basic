#include <iostream>
#include <unordered_map>
#include <vector>
#include "test_include.h"


int main(int argc, char **argv) {
    std::cout << "hello_world!" << std::endl;

    std::vector<std::vector<int> *> q_vec;
    // std::vector<int>* aq_vec = {1,2,3};
    int* aq_vec = {1,2,3};
    q_vec.emplace_back(aq_vec);

    aq_vec[2] = 1;

    for (auto val: *aq_vec){
        std::cout << val << std::endl;
    }

    for (auto avec: q_vec){
        for (auto val: *avec){
            std::cout << val << std::endl;
        }

    }

    return 0;
}