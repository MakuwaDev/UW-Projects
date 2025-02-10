//Autor: Krzysztof Hałubek
#include <vector>
#include <unordered_map>
#include <climits>

#include "wys.h"

char n, k;

// kluczem jest hash stanu gry, liczony w sposób poniżej
// wartościami są odpowiednio optymalny ruch oraz maksymalna ilość ruchów do rozwiązania danej pozycji
std::unordered_map<std::size_t, std::pair<char, char>> dp;

// stan gry jest jednoznacznie reprezentowany przez k + 1 najlepszych ograniczeń górnych i dolnych
// jako, że 2 * (k + 1) <= 8 oraz n <= 12, można bezstratnie skompresować stan gry do jednego unsigned long long'a
// stan gry zapisujemy jako sumę tych liczb przemnożonych przez odpowiednią potęgę dwójki
// poniższa funkcja dokonuje takiej kompresji: zamienia dwa wektory rozmiaru k + 1 na jednego unsigned long long'a
std::size_t compress(std::vector<char> &low, std::vector<char> &top) {
    std::size_t res = 0;
    for(std::size_t i = 0; i < low.size(); ++i)
        res += (std::size_t)low[i] * ((std::size_t)1<<((std::size_t)8 * i));
    for(std::size_t i = 0; i < top.size(); ++i)
        res += (std::size_t)top[i] * ((std::size_t)1<<((std::size_t)8 * i + (std::size_t)32));
    return res;
}

// funckja odkodowuje wartość odpowiedniego ograniczenia ze skompresowanego hasha stanu
// mamy tu dla i z przedziału [0, 3] odpowiednio i-te ograniczenie dolne
// oraz dla i z przedziału [4, 7] odpowiednio i-4'te ograniczenie górne
char get(std::size_t hash, std::size_t i) {
    return (char)((hash >> ((std::size_t)8 * i)) % ((std::size_t)1 << (std::size_t)8));
}

// backtrack rozwiązujący grę
char solve(std::size_t hash) {
    if(dp.find(hash) != dp.end()) return dp[hash].second; // ten stan już jest policzony

    // dekodujemy aktualny stan
    std::vector<char> low, top;
    for(char i = 0; i <= k; ++i)
        low.push_back(get(hash, (std::size_t)i));
    for(char i = 4; i <= 4 + k; ++i)
        top.push_back(get(hash, (std::size_t)i));

    // przechodzimy się po wszystkich możliwych ruchach i liczymy min-maxa
    char res = CHAR_MAX, best_move = 0, curr = 0, cnt = 0, first = 0;
    std::vector<char> new_low(k + 1), new_top(k + 1); // nowe ograniczenia górne i dolne w zależności od odpowiedzi
    for(char i = low[k]; i < top[k]; ++i) {
        char contradictions = 0;
        for(char j = 0; j <= k; ++j) {
            if(i >= top[j]) ++contradictions;
            if(i < low[j]) ++contradictions;
        }
        if(contradictions > k) continue; // aby i było odpowiedzią Ala musiałaby skłamać k + 1 razy
        ++cnt;

        if(cnt == 1) { // zapamiętujemy pierwszą możliwą odpowiedź, na wypadek gdyby była jedyna
            first = i;
            continue;
        }

        // generujemy nowe stany
        new_low = low, new_top = top;
        curr = i;
        for(char j = 0; j <= k; ++j)
            if(curr > new_low[j]) std::swap(curr, new_low[j]); // wymieniamy ograniczenia dolne, utrzymując listę posortowaną
        curr = i;
        for(char j = 0; j <= k; ++j)
            if(curr < new_top[j]) std::swap(curr, new_top[j]); // wymieniamy ograniczenia górne, utrzymując listę posortowaną
        curr = std::max(solve(compress(new_low, top)), solve(compress(low, new_top))); // liczymy pesymistyczny przypadek odpowiedzi Ali na nasze pytanie
        if(curr < res) {
            res = curr;
            best_move = i;
        }
    }

    if (cnt == 1) { // mamy tylko jedną możliwą odpowiedź, nie musimy o nic pytać
        dp[hash] = {-first, 0};
        return 0;
    }

    dp[hash] = {best_move, res + 1};
    return res + 1;
}

// funkcja symuluję pojedynczą rozgrywkę, wymaga wcześniejszego rozwiązania gry
void play(std::size_t hash) {
    while(dp[hash].second != 0) {
        char mv = dp[hash].first; // optymalny ruch

        // dekodujemy aktualny stan gry
        std::vector<char> low, top;
        for(char i = 0; i <= k; ++i)
            low.push_back(get(hash, (std::size_t)i));
        for(char i = 4; i <= 4 + k; ++i)
            top.push_back(get(hash, (std::size_t)i));

        // zadajemy pytanie i uaktualniamy stan gry
        if(mniejszaNiz((int)mv)) {
            for(char i = 0; i <= k; ++i)
                if(mv < top[i]) std::swap(mv, top[i]);  
        }
        else {
            for(char i = 0; i <= k; ++i)
                if(mv > low[i]) std::swap(mv, low[i]);  
        }
        hash = compress(low, top);
    }
    odpowiedz((int)(-dp[hash].first));
}

int main() {
    int _n, _k, g;
    std::size_t hash;
    dajParametry(_n, _k, g);
    n = (char)_n, k = (char)_k;

    // generujemy i hashujemy początkowy stan gry
    std::vector<char> low, top;
    for(char i = 0; i <= k; ++i) {
        low.push_back((char)1);
        top.push_back((char)(n + 1));
    }
    hash = compress(low, top);

    // rozwiązujemy grę i symulujemy rozgrywki
    solve(hash);
    for(int i = 0; i < g; ++i)
        play(hash);

    return 0;
}
