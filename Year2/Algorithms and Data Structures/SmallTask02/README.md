# Striped Wallpaper Printing  

Bajtocki Printing House (BZP) has received a large order for striped wallpapers. Each wallpaper consists of **n** vertical stripes of equal width, some of which have predefined colors, while others can be freely chosen.  

BZP prints wallpapers using a **printing matrix**, which contains a sequence of **k** colored stripes and is applied across all possible positions on the wallpaper. If a stripe is printed multiple times with different colors, the final color will be a blend. However, predefined stripes must retain their exact color.  

The goal is to determine the **shortest possible matrix** that can print the entire wallpaper while preserving the constraints on predefined stripes.  

## Input  
- A string of length **n** (1 ≤ n ≤ 1,000,000) consisting of uppercase Latin letters and `*`.  
  - Letters represent predefined colors.  
  - `*` represents stripes with no predefined color.  

## Output  
- A single integer **k**: the minimum length of the printing matrix required to print the wallpaper.  
