# Part 1: Real-Time Notifications Using SSE – Backend

## Context

So far, our *Interactive Route/Path Editor* has evolved from a simple editor on a static image into a tool for defining dynamic "Connect the Dots" boards and drawing individual user paths on them. We have implemented an interactive frontend in TypeScript and a Django backend for managing data.  

In this lab, we introduce a new element: **real-time notifications**. We want users to be informed about certain events in the system without needing to refresh the page. The ideal tool for this purpose, given the one-way server-to-client communication, is **Server-Sent Events (SSE)**.  

### Task Goal

The goal of this part of the lab is to **implement a server-side (Django) mechanism for sending SSE events** that notify active clients about two types of events:

1. Creation of a new game board by any user.  
2. Saving of a new path on an existing board by any user.  

### Functionality to Implement (Backend – Django)

1. **Create a dedicated SSE view:**
   * Create a new Django view (e.g., at `/sse/notifications/`) responsible for streaming events to the client.  
   * This view should return a `StreamingHttpResponse` with the Content-Type set to `text/event-stream`.  
   * The connection should be kept alive, and the server should periodically send keep-alive comments to prevent proxies from closing the connection.  
   * **HACK:** Start with a simple view that only sends keep-alive comments every few seconds. Later, extend it to check periodically if there are events to send. This is not a concurrency assignment, so you don’t need to worry about immediate delivery.  

2. **Implement event-sending logic:**
   * **Using Django Signals:** This is the preferred way to react to model events.  
     * Define a signal (or use the built-in `post_save`) for the `Board` model (from Lab 9). When a new `Board` object is created, the server should send an SSE event.  
     * Define a signal (or use `post_save`) for the `Path` model (from Lab 10). When a new `Path` object is created, the server should send an SSE event.  
   * **Event data format:**  
     * For a **new board event**, send at least:  
       * Event type (e.g., `event: newBoard`)  
       * Data in JSON (e.g., `data: {"board_id": 123, "board_name": "Test Board", "creator_username": "userX"}`)  
     * For a **new path event**, send at least:  
       * Event type (e.g., `event: newPath`)  
       * Data in JSON (e.g., `data: {"path_id": 456, "board_id": 789, "board_name": "Puzzle Board", "user_username": "playerY"}`)  
   * **Handling multiple clients:** The SSE mechanism must be able to send notifications to all currently connected clients listening on `/sse/notifications/`. Consider how to manage the list of active connections or an event queue. (Note: for lab purposes, you can start with a simple solution, but keep scalability challenges in mind.)  

### Backend Testing

Before moving to the frontend part, it’s important to test whether your SSE endpoint works correctly. You can do this using `curl` in the terminal:  

* Make sure your Django development server is running.  
* Open a terminal window and run a command to listen to the event stream (assuming the endpoint is `/sse/notifications/` and the server is running on `localhost:8000`):  

    ```bash
    curl -N http://localhost:8000/sse/notifications/
    ```

    The `-N` flag (or `--no-buffer`) is crucial, so that `curl` displays data immediately without buffering.

* To also see the HTTP response headers (including the key `Content-Type: text/event-stream`), use:

    ```bash
    curl -i -N http://localhost:8000/sse/notifications/
    ```

* With `curl` listening in one terminal, trigger an action in the browser (or via the Django admin panel) that should generate an event (e.g., create a new board).
* Watch the output in the terminal with `curl`. You should see data in SSE format, e.g.:

    ```text
    event: newBoard
    data: {"board_id": 1, "board_name": "New Board from Admin", "creator_username": "admin"}

    ```

    If you’ve implemented keep-alive comments, they should also appear.

**Technical Hints:**

* `django.http.StreamingHttpResponse`
* Django Signals (`django.db.models.signals.post_save`, `django.dispatch.Signal`)
* SSE message format (lines `event:`, `data:`, `id:`, `retry:`)
* Avoid buffering issues (e.g., by sending keep-alive comments starting with `:` every few seconds).
