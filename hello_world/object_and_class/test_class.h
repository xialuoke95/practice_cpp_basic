#include <string>

// basic struct: 一般如果定义类的所有成员都公开时，使用struct
struct Point {
    Point(float x, float y): x(x), y(y){}
    float x = 0.0;
    float y = 0.0;

    // in-class initialization of non-static data member is a C++11 extension
};

// basic class with ptr

// basic class with init

// this

//-------------------  Herichal class
class BaseData {
public:
    virtual std::string check_data(int status){
        if (status == 0){
            return "ok";
        } else {
            return "wrong";
        }
    };

    // wrong method 
    // virtual std::string wrong_check_data_v2(int status);

    // 纯虚函数
    virtual std::string check_data_v2() = 0;

    virtual int fcn(){
        return 1;
    };
};

class HighLevelData: public BaseData {
public: 
    
    // wrong method
    // std::string check_data(int status); 重新写一遍定义也不行，会被认为是要自己实现方法，没实现就编不过。
    // std::string wrong_check_data_v2(int status);
    // a missing vtable usually means the first non-inline virtual member function has no definition. 虚函数未实现

    std::string check_data_v2() {return "ok";};
    int fcn(int) {return 0;}; // 参数list不一样，和virtual fcn是两个函数

    std::string check(int status){
        return check_data(status);
    };
    std::string always_valid();
};


// some method can be used. some cannot. Class can be used.

