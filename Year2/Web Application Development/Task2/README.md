
# Labs 5–12: Interactive Route/Path Editor

The goal of this project was to gradually build a full-stack web application: an interactive editor for creating, storing, and managing routes/paths on images and dynamic boards. The work evolved from a simple Django app into a multi-user system with real-time updates.

## Overview of Labs

### Lab 5 – Web Application Basics (Django)
We started with setting up a **Django-based backend** to handle user accounts and data models.  
Key elements:
- User authentication and authorization.
- Basic data models (`User`, `BackgroundImage`, `Route`, `RoutePoint`).
- CRUD operations for routes and points.  
This established the foundation for later interactive features.

---

### Lab 6 – REST API
In this stage, we extended the backend with a **REST API** using Django REST Framework (DRF).  
Key elements:
- API endpoints for managing routes and points.
- Token-based authentication for API access.
This enabled asynchronous interaction with the application.

---

### Lab 7 – Application Testing
We added **automatic testing** to ensure correctness and stability of the system.  
Key elements:
- Unit and integration tests for data models.
- Tests for authentication and authorization (both web and API).
- CRUD tests for routes and points.
- Validation of API responses and error handling.  
This stage guaranteed reliability as the application grew more complex.

---

### Lab 8 – Interactive Frontend (TypeScript)
We introduced an **interactive frontend written in TypeScript**.  
Key elements:
- Editing routes directly on a static background image.
- Adding points by clicking on the image.
- Displaying routes dynamically.  
This was the first step towards a more user-friendly interface.

---

### Lab 9 – Dynamic Boards
Instead of static images, we added support for **dynamic boards** resembling “Connect the Dots.”  
Key elements:
- Boards defined as grids with customizable size and dot placement.
- Storing board configurations in the database.
- Replacing static backgrounds with reusable, interactive boards.  
This opened the way for multi-user scenarios.

---

### Lab 10 – Drawing Paths on Boards
Users could now draw their own **individual paths on shared boards**.  
Key elements:
- Each logged-in user can load any board created by any user.
- A user’s path is private to them and not visible to others.
- Paths are stored and reloaded per user and board combination.  
This created a collaborative yet personalized experience.

---

### Lab 11 – Real-Time Notifications (Backend, SSE)
We integrated **Server-Sent Events (SSE)** for real-time updates.  
Key elements:
- A dedicated SSE endpoint in Django (`/sse/notifications/`).
- Notifications triggered when:
  - A new board is created.
  - A new path is saved.
- Events delivered in JSON format with type (`newBoard`, `newPath`).  
This allowed active users to stay informed without refreshing.

---

### Lab 12 – Real-Time Notifications (Frontend, SSE)
Finally, we connected the **frontend with SSE** to receive and display live notifications.  
Key elements:
- TypeScript `EventSource` integration.
- Event listeners for `newBoard` and `newPath`.
- User-friendly notifications (e.g., toasts or notification lists).
- Optional interactions, such as redirecting to a board from a notification.  
This completed the real-time experience of the web application.

---

## Final Notes
Together, labs **5–12** form a complete, working **interactive web application**:
- **Backend:** Django + DRF for models, APIs, authentication, and SSE.
- **Frontend:** TypeScript for interactivity and real-time features.
- **Core Features:**
  - User accounts and secure data management.
  - Interactive editing of routes and paths.
  - Dynamic boards as shared environments.
  - Real-time notifications about system events.

This project demonstrates the full development cycle of a modern web app: from database models and APIs, through frontend interactivity, to real-time user experience.
