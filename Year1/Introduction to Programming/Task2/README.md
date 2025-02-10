# Motel Trip Problem
A short summary/translation of the task:  
Along a highway, which we can imagine as a straight line, there are n motels. The motels are numbered sequentially from 1 to n along the highway. Each motel belongs to a particular motel chain, represented by a number in the range from 1 to n.

Bajtek told Bitek that during his trip along the highway, he stopped at three different motels, each from a different chain. Bitk is curious about the distances between these motels. Specifically, he wants to find the nearest and farthest triplet of motels located in the following order along the highway, where each motel belongs to a different chain:

    Nearest Triplet: The triplet where the maximum distance between consecutive motels is minimized.
    Farthest Triplet: The triplet where the minimum distance between consecutive motels is maximized.

## Task:

Write a program that reads the number of motels, then reads the descriptions of the motels in order along the highway (for each motel: its chain number and distance from the beginning of the highway). The program should output two numbers:

    Maximum of the minimum distances between consecutive motels for the nearest triplet.
    Minimum of the maximum distances between consecutive motels for the farthest triplet.

If there is no valid triplet of motels with different chains, output 0 0.
## Constraints:

    The number of motels (n) will not exceed one million.
    All numbers will fit within the int type.
    Some motels may be at the same position on the highway, and Bajtek may have stayed at multiple motels in the same position.

## Example:

### Input:

9
1 2
2 6
2 9
1 13
1 17
3 20
1 26
3 27
1 30

## Output:

7 10

### Explanation:

    Motel triplet (3, 4, 6) has the distances 9, 13, 20 and minimizes the maximum of consecutive distances.
    Motel triplet (2, 6, 9) has the distances 6, 20, 30 and maximizes the minimum of consecutive distances.
