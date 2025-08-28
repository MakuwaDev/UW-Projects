# Description

In today’s lab, the goal is to prepare a simple server in **Django**.  
Django was demonstrated during the lecture, and the documentation can be found [here](https://docs.djangoproject.com/).

This task is the beginning of a series focused on building web services using Django.

**Task title:** Interactive Route Editor on an Image

## Objective

Create a Django web application that allows users to define, visualize, and edit routes on a chosen background image.  
A route is understood as an ordered sequence of 2D points, represented by coordinates (x, y) on the image plane.

## Functionality Description

1. **Visualization:** The application should display a chosen background image (e.g., a map, a floor plan).
2. **Route Creation:** A logged-in user should be able to define a new route on this image. Defining can be done by interactively clicking on points directly on the image, or alternatively via a form.
3. **Route Display:** The created or selected route should be drawn on the background image, e.g., as a polyline connecting subsequent points in the sequence. Route points should also be visible.
4. **Route Editing:** The user should be able to modify their own routes. The editing interface should allow at least:
   * Adding new points: e.g., by clicking on the image or entering coordinates in a form.
   * Removing existing points: e.g., by clicking on a point or selecting from a list and confirming removal.
   * (Optional / in later stages: reordering points, moving points).

## Tasks for Today’s Lab

In this session, the following core elements of the system should be implemented:

1. **Authentication System:**
   * Implement user login and registration. You may use the built-in Django authentication system (`django.contrib.auth`).
   * Each user should only be able to manage their own routes.
2. **Background Image Management (Admin Panel):**
   * Create a Django model for storing background images.
   * Allow the administrator to add and manage background images via the Django admin panel (`django-admin`). These images will later be available for users to select as backgrounds for their routes.
3. **Basic Route Editing for Users:**
   * Create Django models for storing routes and their points (remembering the relationship to the user and background image).
   * Implement views and templates that allow a logged-in user to:
     * Select a background image from those added by the admin.
     * Create a new empty route associated with that image.
     * Add points to this route (for now, it is enough to add points via a form with X, Y coordinates; simple clicking is optional if time permits).
     * Display the list of points for a given route.
     * Remove points from the route.
     * Ensure changes to the route (added/removed points) are saved in the database.

## Notes

The service must be implemented using the **Django** framework.

The material sufficient to complete this task is mostly contained in the first three parts of the official Django tutorial:

* [Basic project setup](https://docs.djangoproject.com/en/5.0/intro/tutorial01/)
* [Models](https://docs.djangoproject.com/en/5.0/intro/tutorial02/)
* [Templates](https://docs.djangoproject.com/en/5.0/intro/tutorial03/)

Navigation within the service should be designed so that the user can move around using links provided on the pages, without needing to use the browser’s address bar or back button.

For user login and logout, you may use the interface provided by the Django admin panel.  
In this case, users should be flagged as *staff members*, but they must not have permissions to edit anything via the admin panel.

It is not required to protect against attacks such as crafting HTTP requests with external tools to modify another user’s image. It is enough to ensure that such unwanted actions are not explicitly available through the website. However, it is worth considering in which parts of the site this could pose a potential risk.
