//Author: Krzysztof Halubek
//Complexity: O(n)
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <limits.h>
#include <stdbool.h>

#define min(a,b) (((a) < (b)) ? (a) : (b))
#define max(a,b) (((a) > (b)) ? (a) : (b))

static inline int plus1(int x) { return x + 1; }
static inline int minus1(int x) { return x - 1; }
static inline bool less(int a, int b) { return a < b; }
static inline bool greater_equal(int a, int b) { return a >= b; }

// funkcje sprawdzajace czy takie trojki spelniaja warunki zadania (algorytm dziala inaczej dla najblizszych i najdalszych, patrz deklaracje tablic)
static inline bool checkClosest(int left, int right, int *brand) { return (left != INT_MAX && right != INT_MIN && brand[left] != brand[right]) ? true : false; } 
static inline bool checkFarthest(int mid, int left, int right, int* brand) { return (left != INT_MAX && right != INT_MIN && brand[left] != brand[right] && brand[left] != brand[mid] && brand[right] != brand[mid]) ? true : false; }

// funkcja kalkulujaca wartosci tablic closest_..._first i closest_..._second
void calculate_closest(int *closest_first, int *closest_second, int *brand, int beg, int end, int (*next)(int), int (*prev)(int), bool (*inRange)(int, int), int notFound)
{
    closest_first[beg] = closest_second[beg] = notFound; // pierwszy motel nie ma zadnego przed nim
    for(int i = next(beg); inRange(i, end); i = next(i))
    {
        if(brand[i] == brand[prev(i)]) // jesli motel jest tej samej sieci co poprzedni to nic sie nie zmienia
        {
            closest_first[i] = closest_first[prev(i)]; 
            closest_second[i] = closest_second[prev(i)]; 
        }
        else
        {
            closest_first[i] = prev(i); // nowy najblizszy to poprzedni
            if(closest_first[prev(i)] != notFound && brand[closest_first[prev(i)]] != brand[i]) closest_second[i] = closest_first[prev(i)]; // jesli poprzedni najblizszy byl tej innej sieci niz aktualny motel to nowy drugi najblizszy to najlbizszy poprzednio
            else closest_second[i] = closest_second[prev(i)]; // w przeciwnym wypadku drugi najblizszy to drugi najblizszy dla poprzedniego
        }
    } 
}

// funkcja kalkulujaca wartosci tablic farthest_..._first, farthest_..._second i farthest_..._third
void calculate_farthest(int *farthest_first, int *farthest_second, int *farthest_third, int *brand, int beg, int end, int (*next)(int), int (*prev)(int), bool (*inRange)(int, int), int notFound) 
{
    farthest_first[beg] = farthest_second[beg] = farthest_third[beg] = notFound; // pierwszy motel nie ma zadnego przed nim
    for(int i = next(beg); inRange(i, end); i = next(i))
    {
        farthest_first[i] = farthest_first[prev(i)];
        farthest_second[i] = farthest_second[prev(i)];
        farthest_third[i] = farthest_third[prev(i)];
        if(farthest_first[i] == notFound) farthest_first[i] = prev(i); // najdalszy
        else if(farthest_second[i] == notFound && brand[prev(i)] != brand[farthest_first[i]]) farthest_second[i] = prev(i); // drugi najdalszy
        else if(farthest_third[i] == notFound && farthest_second[i] != notFound && brand[prev(i)] != brand[farthest_second[i]] && brand[prev(i)] != brand[farthest_first[i]]) farthest_third[i] = prev(i); // trzeci najdalszy
    }
}

int main()
{
    int n, closest = INT_MAX, farthest = INT_MIN;

    assert(scanf("%d", &n));

    int *brand = (int*)malloc((size_t)n * sizeof(int)); // nr sieci n-tego motelu
    int *dist = (int*)malloc((size_t)n * sizeof(int)); // odl n-tego motelu od poczatku autostrady

    // tablice potrzebne do znalezienia najblizszej trojki
    int *closest_prefix_first = (int*)malloc((size_t)n * sizeof(int)); // najblizszy i-tego motel w strone poczatku autostrady o roznym nr sieci
    int *closest_prefix_second = (int*)malloc((size_t)n * sizeof(int)); // najblizszy i-tego motel w strone poczatku autostrady o nr sieci roznym od i-tego oraz od closest_prefix_first[i]
    int *closest_sufix_first = (int*)malloc((size_t)n * sizeof(int)); // najblizszy i-tego motel w strone konca autostrady o roznym nr sieci
    int *closest_sufix_second = (int*)malloc((size_t)n * sizeof(int)); // najblizszy i-tego motel w strone konca autostrady o nr sieci roznym od i-tego oraz od closest_sufix_first[i]

    // tablice potrzebne do znalezienia najdalszej trojki
    int *farthest_prefix_first = (int*)malloc((size_t)n * sizeof(int)); // najdalszy i-tego motel w strone poczatku autostrady (o nr sieci niezaleznym od nr sieci i-tego motelu)
    int *farthest_prefix_second = (int*)malloc((size_t)n * sizeof(int)); // najdalszy i-tego motel w strone poczatku autostrady o nr sieci roznym od farthest_prefix_first[i] (o nr sieci niezaleznym od nr sieci i-tego motelu)
    int *farthest_prefix_third = (int*)malloc((size_t)n * sizeof(int)); // najdalszy i-tego motel w strone poczatku autostrady o nr sieci roznym od farthest_prefix_first[i] i farthest_prefix_second[i] (o nr sieci niezaleznym od nr sieci i-tego motelu)
    int *farthest_sufix_first = (int*)malloc((size_t)n * sizeof(int)); // najdalszy i-tego motel w strone konca autostrady (o nr sieci niezaleznym od nr sieci i-tego motelu)
    int *farthest_sufix_second = (int*)malloc((size_t)n * sizeof(int)); // najdalszy i-tego motel w strone konca autostrady o nr sieci roznym od farthest_prefix_first[i] (o nr sieci niezaleznym od nr sieci i-tego motelu)
    int *farthest_sufix_third = (int*)malloc((size_t)n * sizeof(int)); // najdalszy i-tego motel w strone konca autostrady o nr sieci roznym od farthest_prefix_first[i] i farthest_prefix_second[i] (o nr sieci niezaleznym od nr sieci i-tego motelu)

    for(int i = 0; i < n; ++i)
        assert(scanf("%d %d", brand + i, dist + i));

    calculate_closest(closest_prefix_first, closest_prefix_second, brand, 0, n, plus1, minus1, less, INT_MAX);
    calculate_closest(closest_sufix_first, closest_sufix_second, brand, n - 1, 0, minus1, plus1, greater_equal, INT_MIN);

    calculate_farthest(farthest_prefix_first, farthest_prefix_second, farthest_prefix_third, brand, 0, n, plus1, minus1, less, INT_MAX);
    calculate_farthest(farthest_sufix_first, farthest_sufix_second, farthest_sufix_third, brand, n - 1, 0, minus1, plus1, greater_equal, INT_MIN);

    // kalkulowanie najblizszej trojki: dla kazdego motelu sprawdzamy wszystkie kombinacje dwoch najblizszych (o ile istnieja) moteli sieci roznych od siebie i od "srodkowego" motelu na lewo i prawo od niego
    // mamy gwarancje, ze optymalny wybor moteli "bocznych" dla danego "srodkowego" motelu (o ile istnieje) zostanie sprawdzony, wiec znajdziemy w ten sposob najblizsza trojke
    for(int mid = 1; mid < n - 1; ++mid)
    {
        int left1 = closest_prefix_first[mid], left2 = closest_prefix_second[mid], right1 = closest_sufix_first[mid], right2 = closest_sufix_second[mid];
        if(checkClosest(left1, right1, brand)) closest = min(closest, max(dist[mid] - dist[left1], dist[right1] - dist[mid]));
        if(checkClosest(left1, right2, brand)) closest = min(closest, max(dist[mid] - dist[left1], dist[right2] - dist[mid]));
        if(checkClosest(left2, right1, brand)) closest = min(closest, max(dist[mid] - dist[left2], dist[right1] - dist[mid]));
        if(checkClosest(left2, right2, brand)) closest = min(closest, max(dist[mid] - dist[left2], dist[right2] - dist[mid]));
    }

    // kalkulowanie najdalszej trojki : dla kazdego motelu sprawdzamy wszystkie kombinacje trzech (o ile istnieja) najdalszych moteli roznych sieci (ale niekoniecznie roznych od sieci "srodkowego" motelu) na lewo i prawo od niego
    // mamy gwarancje, ze optymalny wybor moteli "bocznych" dla danego "srodkowego" motelu (o ile istnieje) zostanie sprawdzony, wiec znajdziemy w ten sposob najdalsza trojke
    for(int mid = 1; mid < n - 1; ++mid)
    {
        int left1 = farthest_prefix_first[mid], left2 = farthest_prefix_second[mid], left3 = farthest_prefix_third[mid];
        int right1 = farthest_sufix_first[mid], right2 = farthest_sufix_second[mid], right3 = farthest_sufix_third[mid];
        if(checkFarthest(mid, left1, right1, brand)) farthest = max(farthest, min(dist[mid] - dist[left1], dist[right1] - dist[mid]));
        if(checkFarthest(mid, left1, right2, brand)) farthest = max(farthest, min(dist[mid] - dist[left1], dist[right2] - dist[mid]));
        if(checkFarthest(mid, left1, right3, brand)) farthest = max(farthest, min(dist[mid] - dist[left1], dist[right3] - dist[mid]));
        if(checkFarthest(mid, left2, right1, brand)) farthest = max(farthest, min(dist[mid] - dist[left2], dist[right1] - dist[mid]));
        if(checkFarthest(mid, left2, right2, brand)) farthest = max(farthest, min(dist[mid] - dist[left2], dist[right2] - dist[mid]));
        if(checkFarthest(mid, left2, right3, brand)) farthest = max(farthest, min(dist[mid] - dist[left2], dist[right3] - dist[mid]));
        if(checkFarthest(mid, left3, right1, brand)) farthest = max(farthest, min(dist[mid] - dist[left3], dist[right1] - dist[mid]));
        if(checkFarthest(mid, left3, right2, brand)) farthest = max(farthest, min(dist[mid] - dist[left3], dist[right2] - dist[mid]));
        if(checkFarthest(mid, left3, right3, brand)) farthest = max(farthest, min(dist[mid] - dist[left3], dist[right3] - dist[mid]));
    }

    if(closest == INT_MAX && farthest == INT_MIN) printf("0 0\n"); // tak naprawde wystarczy tylko jeden z tych warunkow, bo jesli istnieje jakakolwiek trojka moteli roznych sieci to istnieje rowniez najblizsza oraz najdalsza
    else printf("%d %d\n", closest, farthest);

    free(brand);
    free(dist);
    free(closest_prefix_first);
    free(closest_prefix_second);
    free(closest_sufix_first);
    free(closest_sufix_second);
    free(farthest_prefix_first);
    free(farthest_prefix_second);
    free(farthest_prefix_third);
    free(farthest_sufix_first);
    free(farthest_sufix_second);
    free(farthest_sufix_third);
    return 0;
}
