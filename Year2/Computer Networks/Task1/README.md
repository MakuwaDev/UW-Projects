# Peer-to-Peer Clock Synchronization  

## Task Overview  
Implement a **peer-to-peer clock synchronization system** in C or C++, using **UDP sockets and IPv4** (no external networking libraries, single-threaded). The system consists of equal peers (nodes), each maintaining its own natural clock (milliseconds since startup). Peers communicate to synchronize their clocks, accounting for **packet transmission delays**.  

## Key Features  

- **Joining the network**  
  - If started without peers, a node listens for new participants.  
  - If started with `-a` (peer address) and `-r` (peer port), it initiates contact via `HELLO`.  

- **Leader election**  
  - At least one node must act as a **leader** (synchronization source, level 0).  
  - Leaders are chosen and managed with `LEADER` messages.  

- **Synchronization process**  
  - Based on `SYNC_START`, `DELAY_REQUEST`, and `DELAY_RESPONSE` messages.  
  - Nodes calculate clock offsets using four timestamps (T1–T4) and adjust their synchronization level:  
    - `0` → leader  
    - `1` → directly synced to leader  
    - higher numbers → further hops  
    - `255` → unsynchronized  
  - Nodes periodically send sync messages (every 5–10s).  

- **Time queries**  
  - Any node can request current time via `GET_TIME`.  
  - Replies (`TIME`) include the adjusted (synchronized) or raw clock.  

- **Networking & protocol**  
  - Communication is message-based, with strict binary formats (fields for type, addresses, ports, timestamps, sync level).  
  - Nodes discover each other via `HELLO`, `HELLO_REPLY`, `CONNECT`, `ACK_CONNECT`.  
  - Incorrect or unexpected messages must be logged as errors but not crash the node.  

- **Error handling**  
  - Must check all system call results.  
  - Errors should be printed to stderr, prefixed with `ERROR` (or `ERROR MSG …` with hex dump for invalid messages).  

## Deliverables  

- Single-threaded C/C++ program `peer-time-sync`.  
- Buildable via provided `Makefile`:  
  - `make` → creates the executable  
  - `make clean` → removes build artifacts  
- Packaged as a `.zip`, `.tar.gz`, `.7z`, or `.rar` with **no subdirectories**.  
- Program will be tested under rigorous protocol checks.

## ⚠️ Disclaimer
This summary glosses over a lot of important specification and requirements, as the original task is way too complex to compromise thoroughness and simplicity when summarizing it. With any doubts, consult the original task text in Polish.
