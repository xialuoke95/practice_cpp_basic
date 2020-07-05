#include <boost/algorithm/string.hpp>
#include <string>
#include <vector>
#include <iostream>

// 显然, local版本的boost也可以work
// 如何查看版本？vim /usr/include/boost/version.hpp
// 如何查看链接位置是否真的是那里?

int main(int argc, char** argv) {
  std::string who = "world";

  std::cout << "hello world" << std::endl;

  std::string s = "376,258"; 
  std::vector<std::string> v; 
  boost::algorithm::split(v, s, boost::is_any_of(",")); 
  std::cout << v.size() << " " << v[0] << " " << v[1] << std::endl; 

  return 0;
}
