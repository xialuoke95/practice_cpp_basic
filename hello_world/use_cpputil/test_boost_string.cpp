#include <boost/algorithm/string.hpp> 
#include <locale> 
#include <iostream> 
#include <vector> 

// https://www.jianshu.com/p/65dfcc4e8342

int main() 
{ 
  // std::locale::global(std::locale("German")); 
  std::string s = "376,258"; 
  std::vector<std::string> v; 
  // boost::algorithm::split(v, s, boost::algorithm::is_space()); 
  boost::algorithm::split(v, s, boost::is_any_of(",")); 
  std::cout << v.size() << " " << v[0] << " " << v[1] << std::endl; 

  std::vector<std::string> list;
  list.push_back("Hello");
  // list.push_back("World!");

  std::string joined = boost::algorithm::join(list, ", ");
  std::cout << joined << std::endl;

  std::string remove_str = "abcd我是";
  std::vector<std::string> to_remove = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "用户", "从业", "从业者", "青云计划获奖者", "头条青云获奖者", "问答", "达人", "专家"};
  for (auto &str: to_remove){
    boost::algorithm::erase_all(remove_str, str);
  }
  std::cout << remove_str << std::endl;

}