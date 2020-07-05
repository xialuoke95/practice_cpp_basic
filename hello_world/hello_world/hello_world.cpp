#include <iostream>
#include <unordered_map>


int main(int argc, char **argv) {
    std::cout << "hello_world!" << std::endl;
    std::unordered_map<int, int> b_map = {{1,1}};
    for (auto &kv: b_map){
        std::cout << "key " << kv.first << " val " << kv.second << std::endl;
    }
    return 0;
}