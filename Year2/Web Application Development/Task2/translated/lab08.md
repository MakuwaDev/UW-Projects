# Interactive Route Editor – TypeScript Enhancements

## Context

This lab continues the **"Interactive Route Editor on an Image"** project built in Django.  
We assume you already have a working web application from previous labs, which allows:

- Displaying a background image.
- Displaying a list of points for an existing route (e.g., in a table or list next to the image).
- A form for adding new points to the route (with fields for X and Y coordinates).
- The ability to remove points.

## Objective

The goal of this lab is to **enhance the user interface** with dynamic client-side features using **TypeScript**.  
We want to make it easier for users to add points by clicking directly on the image, and to visually locate points on the image based on interactions with the list/form.

## Features to Implement

1. **Add a Point by Clicking on the Image**
   - The user should be able to click anywhere on the displayed background image.
   - After clicking, the coordinates (X, Y) of the point *relative to the image* should automatically be filled into the form fields for adding a new route point.
   - Provide visual feedback for the click, e.g., a temporary marker shown at the clicked location.

2. **Visualize a Selected Point from the Form/List**
   - When the user interacts with an element representing an existing route point (e.g., clicks on a row in the table, hovers over it, or clicks a "Show" button next to it), that point should be **highlighted** on the background image.
   - The visualization could be a temporary circle, a color change of an existing marker (if using SVG or Canvas), or another indicator.
   - The highlight should be temporary or disappear when another point/element is selected.

## Tasks

1. **Set Up TypeScript in the Django Project**
   - Install Node.js and npm (if not already installed).
   - In the Django project root (or a dedicated frontend folder), initialize npm:
     ```bash
     npm init -y
     ```
   - Install TypeScript as a development dependency:
     ```bash
     npm install typescript --save-dev
     ```
   - Create a `tsconfig.json` configuration file. At minimum, configure:
     - Target JavaScript version (e.g., `"target": "ES6"`).
     - Module system (e.g., `"module": "ES6"`).
     - Output directory for compiled JS (e.g., `"outDir": "./static/js/dist/"` – adjust to your Django static files setup).
     - Source maps enabled (`"sourceMap": true`) for easier debugging.
   - Organize TypeScript source files (e.g., in `src/` or `typescript/`).
   - Configure a build process (e.g., add to `package.json`):
     ```json
     "scripts": {
       "build": "tsc"
     }
     ```
   - Run compilation with:
     ```bash
     npm run build
     ```
   - Ensure the compiled JS file is included in your Django templates.  
     Run `python manage.py collectstatic` if needed.

2. **Prepare the HTML Structure**
   - In the Django template (e.g., `route_detail.html`), make sure the following elements have proper `id` or `class` attributes:
     - The image container.
     - The `<img>` element.
     - Input fields for X and Y coordinates in the form.
     - List/table elements representing existing points (each should store coordinates, e.g., in `data-*` attributes).

3. **Implement Image Click Logic (TypeScript)**
   - Add a `click` event listener to the image or its container.
   - In the event handler:
     - Get the click coordinates (`event.offsetX`, `event.offsetY` or calculate using `event.clientX`, `event.clientY` with `getBoundingClientRect()`).
     - Convert them relative to the top-left corner of the image.
     - Update the form’s X and Y input fields.
     - (Optional) Show visual feedback (e.g., a temporary marker `div` at the clicked position).

4. **Implement Point Highlighting Logic (TypeScript)**
   - Add event listeners (`click`, `mouseover`, `mouseout`) to elements representing points in the list/table.
   - In the event handler:
     - Read the X and Y coordinates from `data-*` attributes.
     - Create and display a visual indicator on the image at the given coordinates (e.g., absolutely positioned `div`, or SVG marker).
     - Ensure only one indicator is visible at a time (remove the previous one or hide it on `mouseout`).

## Tools and Tips

- **TypeScript Handbook**: The official guide is a great resource.
- **DOM Manipulation**: Use standard DOM APIs (`document.getElementById`, `querySelector`, `addEventListener`, `getBoundingClientRect`, etc.).
- **Event Handling**: Learn properties of the `Event` object (`target`, `clientX/Y`, `offsetX/Y`).
- **Coordinates**: Carefully calculate click coordinates relative to the image, not the browser window.
- **`data-*` attributes**: Convenient for passing backend data (like coordinates) into the frontend.
- **Debugging**: Use browser dev tools and TypeScript source maps.

## Static Files in Django

1. **Settings (`settings.py`):**
   ```python
   STATIC_URL = '/static/'
   STATICFILES_DIRS = [ BASE_DIR / "static" ]
   STATIC_ROOT = BASE_DIR / "staticfiles"
   ```
   2. **Static File Organization:**
    
    Inside an app:

    ```
    myapp/
    └── static/
        └── myapp/
            └── style.css
    ```
    Or global project-level:
    ```
    static/
    └── global.css
    └── script.js
    ```

3. **Using Static Files in Templates:**

    **Na początku** szablonu (HTML) należy załadować tag static:

    ```
    {% load static %}
    <link rel="stylesheet" href="{% static 'myapp/style.css' %}">
    <script src="{% static 'script.js' %}"></script>
    ```

4. **Development Mode:**

    Set in 'settings.py':
    ```
    DEBUG = True
    ```
    Now Django automatically serves static files and you can test without anny additional configuration.

5. **Production Mode:**
    Set in 'settings.py':
    ```
    DEBUG = False
    ```
    And then run the command:
    ```
    python manage.py collectstatic
    ```
    This command copies all static files from application catalogs and directories listed in **STATICFILES_DIRS** to the directory set by **STATIC_ROOT**.

## Notes

* TypeScript must be compiled to JavaScript to run in the browser (`tsc` or `npm run build`).
* Keep code modular: separate logic for image clicks and point highlighting.
* At this stage, all interactions are client-side only—no server communication is required.

## Optional Extensions

* Allow moving existing points via drag-and-drop and update the form accordingly.
* Visualize the entire route (polyline) on the image using SVG or Canvas, updated dynamically as points are added/removed/moved on the client.
