# Drawing Paths on Boards

## Description

This lab is the next step in the development of the *Interactive Route/Path Editor* project. In the previous stages:

* In **Lab 8**, we introduced an interactive frontend in TypeScript. It allowed editing points on a static background image, with functions for adding points by clicking and visualizing points on the image.  
* In **Lab 9**, the static background was replaced with a dynamically defined grid-based board for a "Connect the Dots" style game. This enabled creating and saving board configurations (dimensions, placement of dots) in the database.  

The current goal is to use the boards from **Lab 9** as the basis for drawing **individual user-specific paths**. Each logged-in user should be able to load a board created by *any* user, then draw and save *their own* unique path on it. Paths drawn by one user on a given board should remain invisible to other users working on the same board.  

## Task Goal

The aim of this lab is to implement functionality that allows a user to:

1. Load an existing board defined in **Lab 9**.  
2. Display this board (grid with dots).  
3. Draw an **individual, sequential path** on the grid by clicking on its cells, using a mechanism similar to point-adding in **Lab 8**, but based on grid coordinates (row, column).  
4. Save the drawn path, **linked to a specific board and a specific user**.  
5. When reloading the same board as the same user, load and display the previously saved path.  
