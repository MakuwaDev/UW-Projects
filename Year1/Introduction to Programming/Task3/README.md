# Origami Layers Problem
## Short task summary/tranlsation:  
This program is designed for origami enthusiasts to study the number of layers in a given point of a cleverly folded piece of paper. The program should read the description of various origami sheets and queries about the number of layers in specified points of these sheets.
## Problem Description:

The input consists of two numbers n and q:

    n: The number of origami sheets.
    q: The number of queries.

Each sheet is described in one of the following forms:

    Rectangle (P x1 y1 x2 y2): Represents a closed rectangle with sides parallel to the coordinate axes.  
    The bottom-left corner is at point (x1, y1) and the top-right corner is at (x2, y2).  
    A pin inserted inside or on the edges of this rectangle increases the layer count by 1 for that point.

    Circle (K x y r): Represents a closed circle with center at (x, y) and radius r.  
    A pin inserted inside or on the boundary of this circle increases the layer count by 1 for that point.

    Fold (Z k x1 y1 x2 y2): This represents a fold of the k-th origami sheet along a line passing through  
    points (x1, y1) and (x2, y2). When a pin is inserted on the folded sheet:
        If the pin is on the right side of the folding line, it adds 0 layers.
        If the pin is exactly on the folding line, the layer count will match the layers of the sheet before folding.
        If the pin is on the left side of the folding line,  
        the layer count is the sum of the layers before folding and the layers of the folded part.

The queries have the following format:

    k x y: Represents a query asking for the number of layers in the k-th sheet at point (x, y).

The program should output the number of layers for each query.
## Input Constraints:

    The coordinates and radius are floating-point numbers.
    There are no time complexity constraints, including exponential solutions.

## Example:

### Input:

4 5  
P 0 0 2.5 1  
Z 1 0.5 0 0.5 1  
K 0 1 5  
Z 3 0 1 -1 0  
1 1.5 0.5  
2 1.5 0.5  
2 0 0.5  
3 4 4  
4 4 4  

### Output:

1  
0  
2  
1  
2  
