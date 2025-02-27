# Medal Standings

## Short task summary/translation:
This project involves implementing a program to maintain the Olympic medal standings using C++ and the Standard Template Library (STL). The task focuses on practicing STL containers and algorithms while ensuring proper input validation, structured decision-making, and thorough testing.

## Task Description
The program processes three types of commands from standard input:
- **Adding medals**: Registers a country and updates its medal count (gold, silver, or bronze).
- **Revoking medals**: Removes a specified medal from a country's count.
- **Printing standings**: Displays the current ranking based on custom medal weights.

Countries are ranked using a weighted medal count, and ties are resolved lexicographically.

## Input Format
Each line follows one of three formats:
1. **Awarding a medal** → `"CountryName X"`  
   - `X = 1` (gold), `2` (silver), `3` (bronze), `0` (adds the country without medals).  
2. **Revoking a medal** → `"-CountryName X"`  
   - Removes a medal (`X = 1, 2, or 3`).  
3. **Printing standings** → `"= G S B"`  
   - Prints the ranking using weights `G`, `S`, and `B` for gold, silver, and bronze.  

Invalid inputs (e.g., revoking a non-existent medal) result in an error message:  
```
  ERROR L
```
where `L` is the line number.

## Output Format
- The ranking is displayed with each country on a separate line, prefixed by its position.  
- If no medals have been recorded yet, no output is produced.  

## Implementation Details
- The program **must use STL containers and algorithms** instead of custom data structures.  
- Input/output should be handled via **streams**.  
- The solution should be submitted as `medals.cpp` and will be compiled using:  
  ```sh
  g++ -Wall -Wextra -O2 -std=c++20 medals.cpp -o medals
  ```
  
## Error Handling

    The program validates input correctness in real time.
    Errors are reported to standard error output.
    The program must exit with code 0.
