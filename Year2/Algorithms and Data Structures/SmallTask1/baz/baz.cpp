#include <iostream>
#include <vector>
#include <numeric>

int main() {
    int n, m;
    std::vector<long long> arr, minOdd, maxOdd, minEven, maxEven;
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::cin>>n;

    arr.resize(n);
    minOdd.resize(n);
    maxOdd.resize(n);
    minEven.resize(n);
    maxEven.resize(n);

    for(int i = 0; i < n; ++i)
        std::cin>>arr[i];

    std::fill(minOdd.begin(), minOdd.end(), -1);
    std::fill(maxOdd.begin(), maxOdd.end(), -1);
    std::fill(minEven.begin(), minEven.end(), -1);
    std::fill(maxEven.begin(), maxEven.end(), -1);

    for(int i = 1; i < n; ++i) {
        if(arr[i - 1] % 2 == 0) {
            maxOdd[i] = maxOdd[i - 1];
            maxEven[i] = arr[i - 1];
        }
        else {
            maxOdd[i] = arr[i - 1];
            maxEven[i] = maxEven[i - 1];
        }
    }

    minEven.back() = arr.back() % 2 == 0 ? arr.back() : -1;
    minOdd.back() = arr.back() % 2 == 1 ? arr.back() : -1;

    for(int i = n - 2; i >= 0; --i) {
        if(arr[i] % 2 == 0) {
            minEven[i] = arr[i];
            minOdd[i] = minOdd[i + 1];
        }
        else {
            minOdd[i] = arr[i];
            minEven[i] = minEven[i + 1];
        }
    }

    for(int i = n - 2; i >= 0; --i)
        arr[i] += arr[i + 1];

    std::cin>>m;

    for(int i = 0; i < m; ++i) {
        int k;
        long long case1, case2;

        std::cin>>k;

        if(arr[n - k] % 2 == 1) {
            std::cout<<arr[n - k]<<"\n";
            continue;
        }

        case1 = (minEven[n - k] != -1 && maxOdd[n - k] != -1) ? arr[n - k] - minEven[n - k] + maxOdd[n - k] : -1;
        case2 = (maxEven[n - k] != -1 && minOdd[n - k] != -1) ? arr[n - k] - minOdd[n - k] + maxEven[n - k] : -1;

        std::cout<<std::max(case1, case2)<<"\n";
    }
}
