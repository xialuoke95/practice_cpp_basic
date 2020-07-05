// bool AuthorUserInfoRetriever::back_mget(std::vector<int64_t> &uids,
//                                         std::unordered_map<int64_t, api::AuthorUserInfoDataPtr> &res,
//                                         RpcRequestContext &ctx,
//                                         const std::string &source)

#include <mutex>
#include <vector>
#include <iostream>
#include <string>

int print_str(const std::string &name){ 
    std::cout << name << std::endl;
};

int print_const_str(const std::string name){ 
    std::cout << name << std::endl;
};

int print_vec_0(std::vector<int> &a_vec){ 
    std::cout << a_vec[0] << std::endl;
};

int main(int argc, char** argv) {
    std::cout << "hello world!" << std::endl;
    // std::string a_ = "a_name";
    // print_str(a_);
    // print_const_str(a_);

    // 这样不行：print_vec_0({1,2});
    std::vector<int> a_vec = {1,2};
    print_vec_0(a_vec);
    return 0;
}
