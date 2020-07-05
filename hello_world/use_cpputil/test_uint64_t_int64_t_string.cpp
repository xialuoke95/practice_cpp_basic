#include <boost/lexical_cast.hpp> 
#include <iostream> 
#include <vector> 
#include <string>

int main() 
{ 
  int64_t a_int = 6820204494886797575;
  uint64_t b_u_int = 6820204494886797575;
  std::string a_string = boost::lexical_cast<std::string>(b_u_int);
  // uint64_t a_u_int = boost::lexical_cast<uint64_t>(a_string);
  int64_t b_int = boost::lexical_cast<int64_t>(a_string);

  std::cout <<  b_int << std::endl; 
}