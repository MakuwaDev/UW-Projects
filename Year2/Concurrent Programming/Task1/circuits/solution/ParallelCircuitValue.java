package cp2024.solution;

import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.Semaphore;

import cp2024.circuit.CircuitValue;

public class ParallelCircuitValue implements CircuitValue {
    // Channel to communicate results between threads.
    private final LinkedBlockingQueue<BoolWrapper> channel;

    // Holds the result once computed.
    private BoolWrapper result = null;

    // Flag to track whether the computation is done.
    private boolean isDone = false;

    // Semaphore to ensure only one thread can wait for computations.
    private final Semaphore mutex = new Semaphore(1);

    // Simple constructor initializing the communication channel.
    ParallelCircuitValue(LinkedBlockingQueue<BoolWrapper> channel) {
        this.channel = channel;
    }

    @Override
    public boolean getValue() throws InterruptedException {
        if (!isDone) {  // Acquire result if computation is not done.
            try {
                mutex.acquire();    // Ensure only one thread handles the result.
                if (!isDone) {      // Double-check to avoid handling the result if another thread already did.
                    result = channel.take();
                    isDone = true;
                }
            } catch (InterruptedException e) {
                isDone = true;
                result = new BoolWrapper(false, -1);    // Signal failure if interrupted.
                throw new InterruptedException();
            } finally {
                mutex.release();    // Release the mutex.
            }
        }

        // If interrupted, throw exception.
        if (result.index()  == -1)
            throw new InterruptedException();

        return result.value(); // Return the computed value.
    }
}
