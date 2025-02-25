# Minimum Programmers for Project Execution  

Bajtazar, the new IT manager of a government institution, must oversee project execution. Each project requires a **minimum number of programmers** and may depend on the completion of other projects. Due to budget cuts, only **k** projects can be executed. The goal is to **minimize the number of programmers needed** while ensuring at least **k** projects are completed.  

## Input  
- **n** (`1 ≤ n ≤ 100 000`) – Number of projects.  
- **m** (`0 ≤ m ≤ 500 000`) – Number of project dependencies.  
- **k** (`0 ≤ k ≤ n`) – Minimum number of projects to complete.  
- **n** lines: Each contains an integer **pᵢ** (`1 ≤ pᵢ ≤ 100 000 000`), the number of programmers required for project **i**.  
- **m** lines: Each contains two integers **a b** (`1 ≤ a, b ≤ n`), meaning project **a** depends on project **b**.  

## Output  
- A single integer: **the minimum number of programmers needed** to complete at least **k** projects.  
