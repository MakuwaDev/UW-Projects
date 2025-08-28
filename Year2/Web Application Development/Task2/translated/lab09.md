# Interactive Route Editor – Game Board Definition

## Context

This lab continues the **"Interactive Route Editor on an Image"** project.  
In previous stages, we built a Django web application with an API for managing routes (sequences of points) on a static background image, and we used TypeScript to add interactivity on the client side (e.g., adding points by clicking).

In this task, we move away from static raster images and instead introduce a **dynamically generated "background"** in the form of a logic puzzle board similar to **"Connect The Dots" / "Flow Free"**.  
The user will be able to define the board’s size and place pairs of colored dots on it.  
This defined board becomes a new kind of "background," which will later be used for drawing connecting paths.

## Objective

The goal of this lab is to implement a **user interface (TypeScript frontend, Django backend)** that allows the user to:

1. Define the dimensions of a rectangular grid (board).
2. Interactively place pairs of colored dots (start/end points for future paths) on the grid.
3. Save the configuration of the created board in the Django database.

## Functionality to Implement

1. **Board Definition:**
   - The user should be able to specify the board’s dimensions (number of rows and columns).
   - Based on these dimensions, the interface should dynamically generate and display an empty grid (e.g., using an HTML table or a `div` structure with CSS Grid/Flexbox).

2. **Placing Dot Pairs:**
   - The user needs a mechanism to choose a color for the next dot pair (from a predefined palette or a simple color selector).
   - After choosing a color, the user can click on two *different* and *empty* cells to place dots of that color.
   - The system must enforce:
     - Each color is used for exactly two dots.
     - A dot cannot be placed on an already occupied cell.
   - Visual feedback should show placed dots (e.g., cell background color, a circle in the center, etc.).

3. **Board Management:**
   - The user should be able to edit an existing board (change dimensions, add or remove dots).
   - The user should be able to assign a name/title to the board.
   - A **Save** button should send the configuration (dimensions, name, list of placed dots with their positions and colors) to the Django backend for database storage.
   - The user should be able to delete a board (e.g., via a **Delete** button), removing it from the database.
   - The user should be able to browse their boards (e.g., in a table) and select one to edit.

## Tasks

1. **Django Models:**
   - Either replace or extend the existing `BackgroundImage` model.  
     A new model can be created, e.g., `GameBoard` or `DotGridDefinition`.
   - The model should store:
     - Reference to the user (`ForeignKey` to `User`).
     - Board name/title (`CharField`).
     - Dimensions: `rows` (number of rows, `IntegerField`), `cols` (number of columns, `IntegerField`).
     - Dot placement:
       - Use a `JSONField` to store a list of objects like:
         ```json
         [
           {"row": r1, "col": c1, "color": "#FF0000"},
           {"row": r2, "col": c2, "color": "#FF0000"}
         ]
         ```
       - Alternatively, create a separate `Dot` model with `ForeignKey` to `GameBoard` and fields `row`, `col`, `color`.
   - Update the Django admin panel to manage the new models (optional, for preview).

2. **Django Views & Templates:**
   - Create a view and template for creating/editing a `GameBoard`.
   - The template should include:
     - A form for the board’s name and dimensions (rows, columns).
     - A container (`div`) where TypeScript will generate the grid.
     - UI elements for color selection (palette, buttons, etc.).
     - A **Save** button.

3. **TypeScript Interface:**
   - **Grid Generation:**  
     Generate an HTML grid dynamically after entering dimensions (or clicking a **Generate** button).  
     Use `data-*` attributes (e.g., `data-row`, `data-col`) to identify cells.
   - **Color Selection:**  
     Implement logic to choose an active color for placing dots.
   - **Cell Click Handling:**  
     Add `click` listeners to grid cells.
   - **Dot Placement Logic:**  
     - Track the board state (occupied cells, placed dots, pairs per color).
     - On click:
       - If a color is selected and fewer than two dots of that color exist:
         - Place the dot (store row, col, color in state and visualize it).
         - Block placing more than two dots per color.
         - Block placing dots on occupied cells.
     - Update grid visualization accordingly.
   - **Data Collection for Saving:**  
     Write a function to gather the current board configuration (dimensions, name, dots list) from the TypeScript state before saving.

4. **Frontend–Backend Communication (Saving the Board):**
   - **Django Endpoint:**  
     Create a view (same as the form view or a dedicated API view) to handle POST requests with board configuration (JSON).
   - **TypeScript Save Logic:**  
     Add a `click` listener to the **Save** button.  
     On save:
     - Collect the board name and configuration.
     - Send a POST request (e.g., with `fetch`) to the Django endpoint, including CSRF token if needed.
   - **Django Save Logic:**  
     - Receive and validate data (dimensions, dot validity, user permissions).
     - Save/update a `GameBoard` instance in the database.
     - Return a success/error response.

## Tools & Tips

- **Django Models:** `JSONField` is flexible for storing dot configurations.
- **CSS:** Use CSS Grid or Flexbox for the dynamic grid.
- **TypeScript:** Use strong typing for board state (e.g., `Dot`, `BoardState` interfaces).
- **DOM Manipulation:** Use `document.createElement`, `addEventListener`, `dataset` attributes for grid handling.
- **Fetch API:** Use for async POST requests to Django.
- **CSRF Protection:** Add `X-CSRFToken` header if needed.
- **Validation:** Enforce rules both in TypeScript (no clicks on occupied cells) and in Django (data integrity checks).

## Notes

- This lab focuses on *defining* the "Connect The Dots" board.  
  Path drawing and validation logic (ensuring no crossings, paths through cell centers) will be addressed in later labs.
- Think about UX: how to make color selection and placing dot pairs convenient?
- Start with a small set of predefined colors.
