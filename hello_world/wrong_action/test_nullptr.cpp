#include <boost/algorithm/string.hpp> 
#include <locale> 
#include <iostream> 
#include <vector> 
#include <unordered_map>

int main() 
{ 
     std::unordered_map<int, std::unordered_map<int64_t, int32_t>> group_counter_results;
     group_counter_results[1][1] = 100; 
     group_counter_results[1][2] = 200;
     std::cout << group_counter_results[1][2] << std::endl;

     for (auto counter_name : group_counter_results) {
         std::cout << "test right" << counter_name.second.find(5)->second << std::endl;
     }
     // core

     for (auto counter_name : group_counter_results) {
         std::cout << "test right" << counter_name.second.find(1)->second << std::endl;
     }

}