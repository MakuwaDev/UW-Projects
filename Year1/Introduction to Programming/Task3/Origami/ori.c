// Author: Krzysztof Halubek
// Complexity: O(q*2^n)
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <stdbool.h>

#define EPS 1e-12

typedef struct {
    double x;
    double y;
} Point;

typedef struct {
    Point p1; // lower left corner
    Point p2; // upper right corner
} Rectangle;

typedef struct {
    Point center;
    double radius;
} Circle;

typedef struct {
    Point p1;
    Point p2;
    int prev;
} Fold;

typedef struct {
    char type;
    void* data; // pointer to the adequate structure, dependent on the type of the sheet
} Sheet;

typedef struct {
    double a;
    double b;
    double c;
} Line; // ax + by + c = 0

//a few functions which load data to adequate structures
void input_point(Point* p) { 
    assert(scanf("%lf%lf", &(p->x), &(p->y))); 
}

void input_rectangle(Rectangle* r) { 
    input_point(&(r->p1)); 
    input_point(&(r->p2)); 
}

void input_circle(Circle* c) {
    input_point(&(c->center)); 
    assert(scanf("%lf", &(c->radius))); 
}

void input_fold(Fold* b) {
    assert(scanf("%d", &(b->prev))); 
    input_point(&(b->p1)); 
    input_point(&(b->p2)); 
}

void input_sheets(Sheet* sheets, int n) { // loading data about given sheets
    for(int i = 0; i <= n; ++i)
        sheets[i].data = NULL;
    for(int i = 1; i <= n; ++i) {
        assert(scanf(" %c", &sheets[i].type));
        switch (sheets[i].type) {
        case 'P': // rectangle
            sheets[i].data = (void*)malloc(sizeof(Rectangle));
            input_rectangle(sheets[i].data);
            break;
        case 'K': // circle
            sheets[i].data = (void*)malloc(sizeof(Circle));
            input_circle(sheets[i].data);
            break;
        default: // fold
            sheets[i].data = (void*)malloc(sizeof(Fold));
            input_fold(sheets[i].data);
            break;
        }
    }
}

void free_data(Sheet* sheets, int n) { // freeing memory previously allocated in input_sheets()
    for(int i = 0; i <= n; ++i)
        if(sheets[i].data != NULL) free(sheets[i].data);
    free(sheets);
}

bool equal(double a, double b) { // a == b
    return fabs(a - b) < EPS; 
}

bool less_equal(double a, double b) { // a <= b
    return a < b || equal(a, b); 
}

bool greater_equal(double a, double b) { // a >= b
    return a > b || equal(a, b); 
} 

bool inside_rectangle(Rectangle r, Point p) { // checks whether p lays inside tha boundaries of r
    return greater_equal(p.x, r.p1.x) && greater_equal(p.y, r.p1.y) && less_equal(p.x, r.p2.x) && less_equal(p.y, r.p2.y); 
}

bool inside_circle(Circle c, Point p) { // // checks whether p lays inside tha boundaries of c
    return less_equal((c.center.x - p.x) * (c.center.x - p.x) + (c.center.y - p.y) * (c.center.y - p.y), c.radius * c.radius); 
} 

double cross_product(Point a, Point b, Point c) { // cross product of vectors ab and ac
    return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x); 
} 

bool collinear(Point a, Point  b, Point c) { // checks whether given points are collinear
    return equal(cross_product(a, b, c), 0); 
} 

bool left_side(Point p1, Point p2, Point p) { // checks whether p lays on the left of the line p1p2 (from p1's perspective)
    return cross_product(p1, p2, p) > 0; 
} 

Line create_line(Point p1, Point p2) { // generates a line going through points p1 and p2
    return (Line){p1.y - p2.y, p2.x - p1.x, p1.x * p2.y - p1.y * p2.x}; 
} 

Line perpendicular(Line l, Point p) { // generates a line perpendicular to l, which goes through p
    return (Line){ -l.b, l.a,  l.b * p.x - l.a * p.y}; 
} 

Point intersection(Line l1, Line l2) { // calculates the point of intersection for lines l1 and l2
    Point res;
    res.x = (l2.c * l1.b - l1.c * l2.b) / (l1.a * l2.b - l2.a * l1.b);
    res.y = (l1.c * l2.a - l2.c * l1.a) / (l1.a * l2.b - l2.a * l1.b);
    return res;
}

Point reflect(Point p1, Point p2, Point p) { // symmetrical reflection of point p relative to the line p1p2
    Point res;
    Line axis1 = create_line(p1, p2); // axis of symmetry
    Line axis2 = perpendicular(axis1, p); // axis perpendicular to axis1 going through p
    Point mid = intersection(axis1, axis2); // intersection point of axis1 and axis2
    res.x = mid.x + (mid.x - p.x);
    res.y= mid.y + (mid.y - p.y);
    return res;
}

int count(Sheet* sheets, int k, Point p) { // counts how many times a pin stuck in point p pierces the kth sheet
    Fold curr;
    switch (sheets[k].type) {
    case 'P': // rectangle
        return (int)inside_rectangle(*((Rectangle*)(sheets[k].data)), p);
    case 'K': // circle
        return (int)inside_circle(*((Circle*)(sheets[k].data)), p);
    default: // fold
        curr = *((Fold*)(sheets[k].data));
        if(collinear(curr.p1, curr.p2, p)) return count(sheets, curr.prev, p);
        if(!left_side(curr.p1, curr.p2, p)) return 0;
        return count(sheets, curr.prev, p) + count(sheets, curr.prev, reflect(curr.p1, curr.p2, p));
    }
}

int main() {
    int n, q;

    assert(scanf("%d%d", &n, &q));
    
    Sheet* sheets = (Sheet*)malloc((size_t)(n + 1) * sizeof(Sheet));
    input_sheets(sheets, n);

    for(int i = 0; i < q; ++i) {
        int k;
        double x, y;
        assert(scanf("%d%lf%lf", &k, &x, &y));
        printf("%d\n", count(sheets, k, (Point){x, y}));
    }

    free_data(sheets, n);
    return 0;
}
