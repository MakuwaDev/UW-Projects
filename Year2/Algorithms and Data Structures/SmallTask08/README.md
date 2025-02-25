# Optimal Steering Plan for Captain Bajtazar  
## Short task summary/translation:
Captain Bajtazar and his first officer Bajtek are sailing the Bajtocian Sea. There are **n** islands, numbered from **1** to **n**. The captain's ship starts at **island 1** and must reach **island n**.  

The ship moves in one of **four cardinal directions** (north, south, east, or west). When making a **90° turn**, the captain and the first officer switch control. After stopping at an island, the captain can choose who takes the helm first.  

The goal is to **minimize the captain's steering time**, regardless of the total travel distance. The ship moves at a **constant speed of one unit per hour**.  

## Input  
- **n** (`2 ≤ n ≤ 200 000`) – Number of islands.  
- **n** lines: Each contains two integers **xᵢ yᵢ** (`0 ≤ xᵢ, yᵢ ≤ 1 000 000 000`), representing the coordinates of island **i**.  
- No two islands share the same coordinates.  

## Output  
- A single integer: The **minimum number of hours** the captain must spend steering the ship from **island 1** to **island n**.  
