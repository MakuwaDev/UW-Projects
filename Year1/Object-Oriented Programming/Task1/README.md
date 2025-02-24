# Urban Traffic Simulation
## Short task summary/translation:
This project is a simplified simulation of urban tram traffic, designed with object-oriented principles for easy extension.
## Key Elements in the Simulation:

    Passengers: Travel between stops, following a daily routine.
    Vehicles (Trams only): Each tram has a unique ID, follows a fixed route, and operates between 6:00 and 23:00.
    Tram Lines: Defined by a sequence of stops and travel times.
    Stops: Have a fixed capacity and serve passengers waiting for trams.

## Simulation Mechanics:

    Trams start running at 6:00 and stop departing at 23:00, completing their routes before shutting down.
    Passengers travel randomly between stops and may need to wait for available space.
    The simulation runs for a given number of days, with events processed in chronological order using an event queue.
    A custom priority queue implementation is required (e.g., sorted list, linked list, or heap).

## Program Output:

    Input parameters.
    Detailed event log, including timestamps and descriptions.
    Statistics, such as:
        Total passenger trips.
        Average waiting time at stops.
        Daily breakdown of trips and waiting times.

## Input Format:

The program reads structured data from standard input, defining the number of days, stop capacities, passenger counts, tram capacities, and tram line routes.
## Additional Notes:

    The queue must support inserting events, retrieving the earliest event, and checking if it's empty.
    Passengers and trams operate independently, interacting through events.
    At the end of each day, passengers "return home" (not simulated), and trams reset to their starting positions.
