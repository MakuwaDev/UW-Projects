# Odd Shopping  
## Short task summary/translation:
Bajtek spends his summer at his grandmother's house. Every morning, she goes to the market to buy products, always spending an odd total amount. One day, she asks Bajtek how much money she should bring if she wants to buy exactly **k** products, ensuring the total cost is odd.  

Bajtek decides to write a program that, given a list of product prices, determines the maximum possible odd sum for **k** selected items. If no such selection is possible, the program should return `-1`.  

## Input  
- **n** – Number of available products (1 ≤ n ≤ 1,000,000).  
- **n integers** – Prices of the products in non-decreasing order (1 ≤ price ≤ 10⁹).  
- **m** – Number of days Bajtek helps his grandmother (1 ≤ m ≤ 1,000,000).  
- **m integers** – Each representing the number of products **k** to be bought on a given day (1 ≤ k ≤ n).  

## Output  
For each query, print the maximum odd sum of **k** products or `-1` if it's not possible.  
