#include <cstdlib>
#include <cstdio>
#include <algorithm>
#include <vector>
#include <iostream>

using namespace std;
vector<int> valid_base;
vector<int> prime_list;

long long n;

int main() {
    cin >>n;
    valid_base.resize(0);
    prime_list.resize(0);
    int ans = 0;
    for (int i = 2; n / i /  i / i >= 1; i ++) {
        int prime_cnt = 0;
        int flag = 1;
        for (int j = 0; j < prime_list.size() && flag; j ++) {
            if (i % prime_list[j] == 0) {
                prime_cnt ++;
                if (i % (prime_list[j] * prime_list[j]) == 0)
                    flag = 0;
            }
        }
        if (!flag)
            continue;
        if (prime_cnt == 0) {
            prime_cnt ++;
            prime_list.push_back(i);
        }
        //cout << i << " " << prime_cnt << " " << n / (i * i * i) << endl;
        if (prime_cnt & 1)
            ans += n / (i * i * i);
        else
            ans -= n / (i * i * i);
    }
    cout << ans << endl;
    return 0;
}