# Highway Painting Tracker  

Professor Makary is painting a **n-kilometer** highway at night using white paint. Initially, the entire highway is black. Each night, he paints a segment of the highway. However, during the day, road repairs may occur, covering sections with asphalt and turning them black again.  

The goal is to keep track of how many kilometers of the highway are painted white after each operation.  

## Input  
- An integer **n** (`1 ≤ n ≤ 1 000 000`): the highway length.  
- An integer **m** (`1 ≤ m ≤ 1 000 000`): the number of painting and repair events.  
- **m** lines, each containing:  
  - Two integers **a, b** (`1 ≤ a ≤ b ≤ n`): the affected segment.  
  - A character **c**:  
    - `'B'` (painted white)  
    - `'C'` (covered with asphalt, turning it black)  

## Output  
- After each operation, print the number of kilometers currently painted white.  
