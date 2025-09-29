# Part 2: Real-Time Notifications Using SSE – Frontend

**Deadline:** This task is graded on a pass/fail basis. It must be submitted during the lab session to the instructor.

## Context

In the first part of this lab, we prepared the Django backend to send notifications about new boards and saved paths using Server-Sent Events (SSE). Now it’s time to integrate this functionality on the client side.

## Task Goal (Part 2)

The goal of this part of the lab is to **implement a client-side (TypeScript) mechanism for receiving and displaying SSE notifications** sent by the server. The user should receive discreet real-time notifications about the creation of new boards or the saving of new paths by others (or themselves).

### Functionality to Implement (Frontend – TypeScript)

**Establishing the SSE connection:**
* In your TypeScript code, use the built-in `EventSource` interface to connect to the SSE endpoint prepared in Part 1 (e.g., `/sse/notifications/`).  
* The connection should be initialized after loading the appropriate page/component of the application (e.g., the main board view or globally, if notifications should be available across the entire app).  

1. **Handling incoming events:**
   * Register listeners for specific event types defined in Part 1 (e.g., `newBoard` and `newPath`).  
   * For each received event:  
     * Parse the JSON data from the `data` field of the SSE message.  
     * Display a notification to the user.  

**Displaying notifications:**
* Design and implement how notifications will be shown. Options include:  
  * **Toast notifications:** Small, temporary messages appearing in a corner of the screen.  
  * **Dedicated notification section/list:** A place in the interface where recent notifications are collected.  
* Each notification should be clear and include key information, e.g.:  
  * For `newBoard`: *"User [creator_username] created a new board: [board_name]."*  
  * For `newPath`: *"User [user_username] saved a path on the board: [board_name]."*  
* Optionally, add interactivity—for example, clicking on a notification about a new board could redirect the user to that board.  

2. **Error handling and connection state (Optional but recommended):**
   * Add error handling for the `EventSource` connection (`onerror` event).  
   * Inform the user about the SSE connection state (e.g., retrying connection).  

### Technical Hints

* The `EventSource` object in JavaScript/TypeScript.  
* Methods: `addEventListener('eventName', callback)`, as well as `onmessage`, `onerror`, and `onopen` of `EventSource`.  
* Good UX practices for displaying notifications (they should not be overly intrusive).  
* You can use existing JS/TS libraries for toast notifications or implement a simple mechanism yourself.  
