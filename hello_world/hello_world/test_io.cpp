
#include <vector>
#include <iostream>
#include <numeric>
#include <deque>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <utility>
#include <map>
#include <iomanip>
#include <algorithm>

std::ofstream logger("logger.txt");

struct Point {
    Point(float x, float y): x(x), y(y){}
    float x = 0.0;
    float y = 0.0;
};

std::vector<Point> loadPointFile(std::ifstream &config_file){
    std::vector<Point> point_vec;
    std::string name;
    while(config_file >> name){
        Point point(0.0, 0.0);
        point.x = std::stoll(name);
        point_vec.push_back(point);
    }
    return point_vec;
};

// void read()
// {
// 	// turns on both fail and bad bits
// 	cin.setstate(cin.badbit | cin.eofbit | cin.failbit);
// }
// eof

std::istream& read(std::istream &is, Point &point) {
	is >> point.x >> point.y;
	return is;
}
// bad io

std::vector<Point> loadPointFileV2(std::ifstream &config_file){
    Point 
    if 
    while(read()){
        Point point(0.0, 0.0);
        point.x = std::stoll(name);
        point_vec.push_back(point);
    }
    return point_vec;
};

int main(int argc, char* argv[]){
    logger << 0 << std::endl;

    std::ifstream point_config(argv[1]);
    std::vector<Point> point_vec = loadPointFile(point_config);
    for (auto point: point_vec){
        std::cout << point.x << std::endl;
    }
    return 0;
}

