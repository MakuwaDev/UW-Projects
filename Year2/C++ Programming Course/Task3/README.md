# Knight Tournament (knights)

## Short task summary/translation:
This project involves implementing a **knight tournament module** in C++. The module consists of a **header file (`knights.h`)** and optionally an **implementation file (`knights.cpp`)**. The task focuses on constructors, operator overloading, type conversions, initialization techniques, and `constexpr` usage.

## Functionality
The module provides two main classes:

### **Knight**
Represents a knight with:
- **Gold** (for meals and souvenirs)  
- **Weapon class** (higher value = stronger weapon)  
- **Armor class** (higher value = better protection)  

#### Key Features:
- `MAX_GOLD`: Maximum allowed gold (`size_t` max value).  
- Constructor: Requires gold, weapon class, and armor class.  
- Copy/move constructors & assignment operators.  
- Accessors for gold, weapon, and armor.  
- Modifiers to add/remove gold, change weapons/armor.  
- `+=` Operator: Steals gold and better equipment from another knight.  
- `+` Operator: Creates a new knight with combined gold (capped at `MAX_GOLD`) and the best equipment.  
- Comparison (`<=>` and `==`): Determines which knight would win a duel based on weapon vs. armor strength.  
- `constexpr` support: Knights can be used in constant expressions.  
- `TRAINEE_KNIGHT`: A compile-time constant knight with 0 gold, weapon, and armor class 1.

##### Comparison conditions
The duel (comparison) is won primarily by the warrior whose weapon is stronger than the opponent's armor and whose armor class is simultaneously not lower than the opponent's weapon class. Furthermore, if both knights have weapons stronger than the opponent's armor, the winner will be the one with the better armor or better weapon when the armors are of the same class. In all other cases, it will be a draw.    

### **Tournament**
Manages a **list of competing knights** and **eliminated knights**.  

#### Key Features:
- Created from a list of knights (defaults to `TRAINEE_KNIGHT` if empty).  
- Supports copy/move constructors & assignment operators.  
- `+=` Operator: Adds a knight to the tournament.  
- `-=` Operator: Removes all knights matching a given knight's attributes.  
- **Tournament execution**:
  - Knights fight in order.
  - The **winner** takes the gold and better equipment of the **loser** and continues.
  - **Losers** are added to the eliminated list (in reverse order of elimination).
  - If a fight results in a **draw**, both are eliminated.  
  - The tournament ends when **only one or zero** knights remain.
- Provides an iterator for the **winner** or `end()` if no winner exists.  
- Prints the list of **remaining** and **eliminated** knights.  

### **Global Function**
- `max_diff_classes()`: Computes the largest difference between weapon and armor classes in a given knight list (constexpr function).  

## Implementation Details
- Uses **STL containers** and follows best C++ practices.  
- Hides **global variables and helper functions** from external access.  
- The eliminated list is **cleared** before each tournament execution or when modifying the tournament.  

## Compilation
The solution will be compiled using:  
  ```sh
  g++ -Wall -Wextra -O2 -std=c++20 *.cpp
  ```
