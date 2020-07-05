#include <iostream>
#include <unordered_map>

#include <cpputil/json/json_params.h>
#include <rapidjson/document.h>

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>


// https://www.cnblogs.com/aquester/p/10331346.html


int main(int argc, char **argv) {
    std::cout << "hello_world!" << std::endl;

    rapidjson::Document doc;
    rapidjson::Value::ConstMemberIterator iter;
    rapidjson::Value::ConstValueIterator jter;
    rapidjson::Value::ConstMemberIterator kter;

    if (boost::algorithm::starts_with("hello_world", "hello")){
        std::cout << "success!" << std::endl;
    }

    std::cout << "end_world!" << std::endl;
    return 0;
}