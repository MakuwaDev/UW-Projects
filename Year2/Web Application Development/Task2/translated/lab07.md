# Testing the Route Editor Application

## Objective

The goal of this lab is to write a set of **automated tests** for the *“Interactive Route Editor on an Image”* application created in previous tasks.  
The tests should verify the correctness of key functionalities—both the web interface and the REST API—ensuring the application’s stability and reliability.

## Functionality to Test

**Note:** From each area, it is enough to select just a few tests!!

Prepare **unit tests** and **integration tests** (using Django’s test client) covering the following areas:

1. **Data Models:**
   - Correct creation of model instances (`User`, `BackgroundImage`, `Route`, `RoutePoint`).
   - Verification of relationships between models (e.g., `Route` linked to `User` and `BackgroundImage`, `RoutePoint` linked to `Route`).
   - Data validation at the model level (if implemented).

2. **Authentication and Authorization System (Web Interface):**
   - Availability of login-required pages only for authenticated users.
   - Redirecting unauthenticated users to the login page.
   - Ability for users to log in and log out.
   - Verify that a user can manage (view, add points, delete points/routes) *only* their own routes. Attempts to access another user’s routes should fail or return an error (e.g., 404 Not Found or 403 Forbidden).

3. **Managing Routes and Points (Web Interface):**
   - Correct display of the user’s route list.
   - Process of creating a new empty route linked to a selected background image.
   - Correct addition of points to a route (e.g., via a form).
   - Correct deletion of points from a route.
   - Verify that changes (adding/removing points) are properly saved in the database and reflected in the view.

4. **REST API (Django REST Framework):**
   - **Authentication and Authorization:**
     - Verify that API endpoints require proper authentication (e.g., with a token).
     - Ensure that a user can only access and modify *their own* routes and points via the API.
   - **Route Management Endpoints (`/api/routes/`, `/api/routes/{route_id}/`):**
     - Test creating a new route (POST).
     - Test retrieving the list of routes (GET) – does it return only the user’s routes?
     - Test retrieving route details (GET).
     - Test deleting a route (DELETE).
   - **Route Point Management Endpoints (`/api/routes/{route_id}/points/`, `/api/routes/{route_id}/points/{point_id}/`):**
     - Test adding a new point to a route (POST).
     - Test retrieving the list of points for a route (GET).
     - Test deleting a point from a route (DELETE).
   - **Serialization and Validation:**
     - Verify the correctness of JSON data format returned by the API.
     - Test the API’s response to invalid input data (e.g., missing fields, wrong data types).

## Tasks to Implement

1. **Test Environment Setup:**
   - Learn how to run tests in Django (`python manage.py test`).
   - Optionally configure a test database (Django creates a temporary database by default).
2. **Model Tests Implementation:**
   - Create a `tests.py` file (or a dedicated `tests/` module) in the relevant Django app.
   - Write test classes extending `django.test.TestCase`.
   - Implement test methods for creating and saving model objects, and for verifying relationships between them.
3. **Web Interface Tests Implementation:**
   - Use Django’s built-in test client (`self.client`) to simulate HTTP requests (GET, POST).
   - Test authentication and authorization logic (access to views, redirects).
   - Test CRUD operations on routes and points via corresponding views (e.g., submitting form data via POST).
   - Use assertions to check HTTP response codes (`assertRedirects`, `assertEqual(response.status_code, 200)`), response contents (`assertContains`), and database state after operations.
   - Create test users and sample data (e.g., background images, routes) in the `setUp` or `setUpTestData` methods of test classes.
4. **REST API Tests Implementation:**
   - Use Django’s test client (`self.client`) or DRF’s dedicated client (`rest_framework.test.APIClient`) to send requests to API endpoints.
   - Prepare authentication for the test client (e.g., set `Authorization` header with a test user’s token).
   - Test all defined API endpoints (GET, POST, DELETE) for correct behavior, response codes, and returned JSON (`assertJSONEqual`).
   - Test authorization logic – attempts to access another user’s resources should return an error (e.g., 403 Forbidden or 404 Not Found).
   - Test input validation in the API.

## Tools and Tips

- **Django Testing Framework:** The main tool for writing tests.
  - Docs: [Writing and running tests](https://docs.djangoproject.com/en/stable/topics/testing/overview/)
  - Class `django.test.TestCase`: Automatically manages a database for tests.
  - Test client: `django.test.Client` to simulate HTTP requests.
- **Django REST Framework Testing:** Tools to simplify API testing.
  - Docs: [Testing](https://www.django-rest-framework.org/api-guide/testing/)
  - `rest_framework.test.APITestCase`: Similar to `TestCase` with API-specific features.
  - `rest_framework.test.APIClient`: Test client with API conveniences (e.g., easier authentication, JSON handling).
- **Fixtures / setUpData:** Consider using `setUpTestData` (data created once per test class) or `setUp` (data created before each test) to prepare required objects (users, images, routes).
- **Assertions:** Use the rich set of assertion methods from `unittest` and Django (`assertEqual`, `assertTrue`, `assertContains`, `assertRedirects`, `assertFormError`, `assertJSONEqual`).

## Notes

- Remember to test both **“happy paths”** (everything works as expected) and **error/edge cases** (e.g., deleting a non-existent point, accessing without login, modifying another user’s data).
- Ensure tests are **independent** – the result of one test should not affect another.
