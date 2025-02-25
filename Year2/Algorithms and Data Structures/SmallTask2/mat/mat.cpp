#include <iostream>
#include <vector>
#include <array>
#include <numeric>

int main() {
    std::string wallpaper;
    int32_t res;
    std::array<std::vector<int32_t>, ('Z' - 'A' + 1)> closestDiff;

    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::cin>>wallpaper;
    res = wallpaper.length() + 69;

    for(auto &v : closestDiff) {
        v.resize(wallpaper.length());
        std::fill(v.rbegin(), v.rend(), (int32_t)wallpaper.length());
    }

    for(size_t i = wallpaper.length() - 1; i > 0; --i) {
        char c = wallpaper[i];

        if(c == '*') {
            for(size_t j = 0; j < closestDiff.size(); ++j)
                closestDiff[j][i - 1] = closestDiff[j][i] + 1;
        }
        else {
            for(size_t j = 0; j < closestDiff.size(); ++j)
                if(j != (size_t)c - 'A')
                    closestDiff[j][i - 1] = 1;
            
            closestDiff[c - 'A'][i - 1] = closestDiff[c - 'A'][i] + 1;
        }
    }

    for(size_t i = 0; i < wallpaper.length(); ++i) {
        if(wallpaper[i] == '*')
            continue;
        
        res = std::min(res, closestDiff[wallpaper[i] - 'A'][i]);
    }

    if(res > (int32_t)wallpaper.length())
        res = (int32_t)wallpaper.length();

    std::cout << wallpaper.length() - res + 1 << "\n";
}
