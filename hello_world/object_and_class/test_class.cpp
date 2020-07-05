#include <string>
#include <iostream>
#include "test_class.h"



// basic class

class Data{
public:
    std::string check_status(int status) {
        return _check_status(status);
    }

private:
    std::string _check_status(int status){
        if (status == 0){
            return "ok";
        } else {
            return "wrong";
        }
    }
};

std::string HighLevelData::always_valid(){
    return "always_ok";
};

void print_v1(){
    Data data = Data();
    int aa = 1;
    std::cout << "data: " << data.check_status(aa) << std::endl;
};

void print_herical(){
    HighLevelData data = HighLevelData();
    // std::cout << "high_level_data always valid: " << data.always_valid() << std::endl;
    std::cout << "high_level_data check data: " << data.check_data(1) << std::endl;
    // std::cout << "high_level_data check self: " << data.check(0) << std::endl;
    // std::cout << "high_level_data check data v2: " << data.check_data_v2(0) << std::endl;

};

int main(int argc, char **argv){
    // print_v1();
    // print_herical();
    Point point = Point(3.0, 4.0);
    std::cout << "x: " << point.x << ", y: " << point.y << std::endl;
}



 