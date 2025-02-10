# Office Queues Simulation Problem
Short summary/translation of the task:  
This program simulates events that occur during a working day at an office with multiple service windows. Each window has a queue of customers, and when an officer is available, they serve the first customer in the queue. Customers leave the office after being served. The program also handles various operations as described by the following functions.
## Functions:

    otwarcie_urzedu(int m) – Called once at the start. Informs that there are m windows at the office,  
        and no customers are in the queues yet.

    *interesant nowy_interesant(int k) – A new customer enters the office and joins the queue for window k.  
        This function creates and returns a pointer to the customer.

    int numerek(interesant i)* – Returns the customer's number. Numbers start from 0 and increment sequentially.

    *interesant obsluz(int k) – An officer serves the next customer at window k. If the queue is not empty,  
        the first customer is served and leaves the office. If the queue is empty, the function returns NULL.

    *void zmiana_okienka(interesant i, int k) – The customer i moves from their current window to the queue of window k.
        They leave their current queue and join the new one at the end.

    void zamkniecie_okienka(int k1, int k2) – The officer at window k1 goes on a break.  
        All customers in the k1 queue are transferred to the k2 queue in the same order they were standing at k1.

    *std::vector<interesant> fast_track(interesant i1, interesant i2) – Opens a special window that serves all customers  
        between i1 and i2 (inclusive). This group of customers is served and immediately leaves the office.

    void naczelnik(int k) – The office manager orders the customers at window k to reverse the order of their queue.  
        If there is at most one customer, this operation has no effect.

    *std::vector<interesant > zamkniecie_urzedu() – Called at the end of the office day.  
        All remaining customers in the queues are served, and the office closes.  
        The function returns a vector of all customers who are still in the office at that time.

## Requirements:

    Each customer is created dynamically with malloc, and their memory is freed by the user after leaving the office.
    The time complexity for each function must be proportional to the size of the parameters and outputs,  
    except for otwarcie_urzedu and zamkniecie_urzedu, which can use O(m) time complexity.
    The program should be free of memory leaks (checked using valgrind).

## Example:

### Input:

4 5  
P 0 0 2.5 1  
Z 1 0.5 0 0.5 1  
K 0 1 5  
Z 3 0 1 -1 0  
1 1.5 0.5  
2 1.5 0.5  
2 0 0.5  
3 4 4  
4 4 4  

### Output:

1  
0  
2  
1  
2  
