#include <boost/algorithm/string.hpp>
#include <string>
#include <vector>
#include <iostream>



int main(int argc, char** argv) {
  std::string who = "world";

  std::cout << "hello world" << std::endl;

  std::string s = "376,258"; 
  std::vector<std::string> v; 
  boost::algorithm::split(v, s, boost::is_any_of(",")); 
  std::cout << v.size() << " " << v[0] << " " << v[1] << std::endl; 

  return 0;
}
