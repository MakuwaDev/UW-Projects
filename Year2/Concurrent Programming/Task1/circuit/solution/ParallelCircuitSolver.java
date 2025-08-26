package cp2024.solution;

import cp2024.circuit.*;

import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.locks.ReadWriteLock;
import java.util.concurrent.locks.ReentrantReadWriteLock;
import java.util.ArrayList;
import java.util.List;

public class ParallelCircuitSolver implements CircuitSolver {
    // Number of available processors for optimal thread pool size.
    private final static int nThreads = Runtime.getRuntime().availableProcessors();

    // Executor service with a fixed thread pool.
    private final ExecutorService pool = Executors.newFixedThreadPool(64 * nThreads);

    // Read-Write lock for synchronizing solves and stops.
    private final ReadWriteLock lock = new ReentrantReadWriteLock(true);

    // Flag to track if computations are allowed.
    private Boolean acceptComputations = true;
    
    // Method to solve a circuit.
    @Override
    public CircuitValue solve(Circuit c) {
        lock.readLock().lock();     // Acquire read lock to allow concurrent solve calls.
        try {
            LinkedBlockingQueue<BoolWrapper> channel;
            CircuitValue result;

            if (!acceptComputations) {  // If solver was stopped, return a broken value.
                return new BrokenCircuitValue();
            }

            channel = new LinkedBlockingQueue<>(1); // Create a channel for result.
            result = new ParallelCircuitValue(channel);

            pool.submit(new RecursiveSolve(channel, c.getRoot(), 0));
            return result;
        } finally {
            lock.readLock().unlock();   // Release read lock.
        }
    }

    // Method to stop computations and shutdown the thread pool.
    @Override
    public void stop() {
        // Check if already stopped.
        if (!acceptComputations) {
            return;
        }

        // Acquire write lock, wait for already running solves and block any new solves.
        lock.writeLock().lock();    
        try {
            // Check again if not already stopped while waiting on lock.
            if (!acceptComputations) {
                return;
            }

            // Shutdown pool and stop accepting computations.
            acceptComputations = false;
            pool.shutdownNow();
        } finally {
            lock.writeLock().unlock();  // Release write lock.
        }
    }

    // Inner class responsible for recursively and concurrently solving a circuit.
    private class RecursiveSolve implements Runnable {
        // Current node to be solved.
        private final CircuitNode node;

        // Channel to communicate with parent, by offering current value and index.
        private final LinkedBlockingQueue<BoolWrapper> parentChannel;

        // Current index.
        private final int index;

        // Channel to communicate with children, by taking their value and index.
        private LinkedBlockingQueue<BoolWrapper> myChannel;

        // Simple constructor to initialize communication channel with parent, current node and index.
        private RecursiveSolve(LinkedBlockingQueue<BoolWrapper> parentChannel, CircuitNode node, int index) {
            this.parentChannel = parentChannel;
            this.node = node;
            this.index = index;
        }

        @Override
        public void run() {
            CircuitNode[] args;
            List<Runnable> tasks;

            try {
                // Leaf node, return its value directly.
                if (node.getType() == NodeType.LEAF) {
                    parentChannel.offer(new BoolWrapper(((LeafNode) node).getValue(), index));
                    return;
                }

                // For non-leaf nodes get arguments and initialize the channel for child results.
                args = node.getArgs();
                myChannel = new LinkedBlockingQueue<>(args.length);
            } catch (InterruptedException e) {
                parentChannel.offer(new BoolWrapper(false, -1)); // Signal failure if interrupted.
                return;
            }

            // Prepare a list of tasks to be called recursively.
            tasks = new ArrayList<>();
            for (int i = 0; i < args.length; ++i) {
                tasks.add(new RecursiveSolve(myChannel, args[i], i));
            }
            
            // Depending on the node type, call the appropriate solving method.
            switch (node.getType()) {
                case IF -> solveIF(tasks);
                case AND -> solveAND(tasks);
                case OR -> solveOR(tasks);
                case GT -> solveGT(tasks, ((ThresholdNode) node).getThreshold());
                case LT -> solveLT(tasks, ((ThresholdNode) node).getThreshold());
                case NOT -> solveNOT(tasks);
                default -> throw new RuntimeException("Illegal type " + node.getType());
            }
        }

        // Helper method to cancel running child tasks when interrupted or finished early.
        private void cancelEvaluation(List<Future<?>> futures) {
            for (Future<?> future : futures)
                future.cancel(true);
        }

        // Logic to solve NOT node in the circuit.
        private void solveNOT(List<Runnable> tasks) {
            Future<?> future = pool.submit(tasks.get(0));

            try {
                parentChannel.offer(new BoolWrapper(!myChannel.take().value(), index)); // Send negated value.
            } catch (InterruptedException e) {
                future.cancel(true);
                parentChannel.offer(new BoolWrapper(false, -1)); // Signal failure if interrupted.
            }
        }

        // Logic to solve LT node in the circuit.
        private void solveLT(List<Runnable> tasks, int threshold) {
            int gotTrue = 0;
            List<Future<?>> futures = new ArrayList<>();

            // Early termination if threshold is trivially satisfied.
            if (threshold > tasks.size()) {
                parentChannel.offer(new BoolWrapper(true, index));
                return;
            }

            // Early termination if threshold is trivially not satisfied.
            if (threshold <= 0) {
                parentChannel.offer(new BoolWrapper(false, index));
                return;
            }

            try {
                // Submit tasks for concurrent execution.
                for (Runnable task : tasks) {
                    futures.add(pool.submit(task));
                }

                for (int i = 1; i <= tasks.size(); ++i) {
                    BoolWrapper b = myChannel.take(); // Get result from children.

                    if (b.index() == -1) {
                        throw new InterruptedException(); // Signal failure if child interrupted.
                    }
                    else if (b.value()) {
                        gotTrue++;
                    }

                    // Check conditions for lazy evaluation.
                    if (gotTrue >= threshold) {
                        cancelEvaluation(futures);
                        parentChannel.offer(new BoolWrapper(false, index));
                        return;
                    }
                    
                    if (gotTrue + tasks.size() - i < threshold) {
                        cancelEvaluation(futures);
                        parentChannel.offer(new BoolWrapper(true, index));
                        return;
                    }
                }

                // If loop didn't exit early, condition is satisfied.
                parentChannel.offer(new BoolWrapper(true, index));
            } catch (InterruptedException e) {
                cancelEvaluation(futures);
                parentChannel.offer(new BoolWrapper(false, -1)); // Signal failure if interrupted.
            }
        }

        // Logic to solve GT node in the circuit.
        private void solveGT(List<Runnable> tasks, int threshold) {
            int gotTrue = 0;
            List<Future<?>> futures = new ArrayList<>();

            // Early termination if threshold is trivially satisfied.
            if (threshold < 0) {
                parentChannel.offer(new BoolWrapper(true, index));
                return;
            }

            // Early termination if threshold is trivially not satisfied.
            if (threshold >= tasks.size()) {
                parentChannel.offer(new BoolWrapper(false, index));
                return;
            }

            try {
                // Submit tasks for concurrent execution.
                for (Runnable task : tasks) {
                    futures.add(pool.submit(task));
                }

                for (int i = 1; i <= tasks.size(); ++i) {
                    BoolWrapper b = myChannel.take(); // Get result from children.

                    if (b.index() == -1) {
                        throw new InterruptedException(); // Signal failure if child interrupted.
                    }
                    else if (b.value()) {
                        gotTrue++;
                    }

                    // Check conditions for lazy evaluation.
                    if (gotTrue > threshold) {
                        cancelEvaluation(futures);
                        parentChannel.offer(new BoolWrapper(true, index));
                        return;
                    }

                    if (tasks.size() - i + gotTrue <= threshold) {
                        cancelEvaluation(futures);
                        parentChannel.offer(new BoolWrapper(false, index));
                        return;
                    }
                }

                // If loop didn't exit early, condition is not satisfied.
                parentChannel.offer(new BoolWrapper(false, index));
            } catch (InterruptedException e) {
                cancelEvaluation(futures);
                parentChannel.offer(new BoolWrapper(false, -1)); // Signal failure if interrupted.
            }
        }

        // Logic to solve OR node in the circuit.
        private void solveOR(List<Runnable> tasks) {
            List<Future<?>> futures = new ArrayList<>();

            // Submit tasks for concurrent execution.
            for (Runnable task : tasks) {
                futures.add(pool.submit(task));
            }
            
            try {
                for (int i = 0; i < tasks.size(); ++i) {
                    BoolWrapper b = myChannel.take(); // Get result from children.

                    if (b.index() == -1)
                        throw new InterruptedException(); // Signal failure if child interrupted.
                    else if (b.value()) { // If any child returns true, cancel all computations and send true.
                        cancelEvaluation(futures);
                        parentChannel.offer(new BoolWrapper(true, index));
                        return;
                    }
                }
                
                parentChannel.offer(new BoolWrapper(false, index)); // No child returned true, send false.
            } catch (InterruptedException e) {
                cancelEvaluation(futures);
                parentChannel.offer(new BoolWrapper(false, -1)); // Signal failure if interrupted.
            }
        }

        // Logic to solve AND node in the circuit.
        private void solveAND(List<Runnable> tasks) {
            List<Future<?>> futures = new ArrayList<>();

            // Submit tasks for concurrent execution.
            for (Runnable task : tasks) {
                futures.add(pool.submit(task));
            }
            
            try {
                for (int i = 0; i < tasks.size(); ++i) {
                    BoolWrapper b = myChannel.take(); // Get result from children.

                    if (b.index() == -1)
                        throw new InterruptedException(); // Signal failure if child interrupted.
                    else if (!b.value()) { // If any child returns false, cancel all computations and send false.
                        cancelEvaluation(futures);
                        parentChannel.offer(new BoolWrapper(false, index)); // Signal failure if interrupted.
                        return;
                    }
                }

                parentChannel.offer(new BoolWrapper(true, index)); // No child returned false, send true.
            } catch (InterruptedException e) {
                cancelEvaluation(futures);
                parentChannel.offer(new BoolWrapper(false, -1)); // Signal failure if interrupted.
            }
        }

        // Logic to solve IF node in the circuit.
        private void solveIF(List<Runnable> tasks) {
            List<Future<?>> futures = new ArrayList<>();
            BoolWrapper condition = null, trueBranch = null, falseBranch = null;

            // Submit tasks for concurrent execution.
            for (Runnable task : tasks) {
                futures.add(pool.submit(task));
            }

            try {
                for (int i = 0; i < tasks.size(); ++i) {
                    BoolWrapper b = myChannel.take(); // Get result from children.

                    // Depending on which child evaluates set values accordingly.
                    switch (b.index()) {
                        case 0 -> condition = b;
                        case 1 -> trueBranch = b;
                        case 2 -> falseBranch = b;
                        default -> { // Ignore signals of child interruption, since it doesn't implicate error.
                            continue;
                        }
                    }

                    if (condition != null) { // If condition evaluated.
                        if (condition.value()) { // If condition evaluated to true.
                            futures.get(2).cancel(true); // Interrupt computations of the false branch.

                            if (trueBranch != null) { // If true branch evaluated, send its value.
                                parentChannel.offer(new BoolWrapper(trueBranch.value(), index));
                                return;
                            }
                        }
                        else { // If condition evaluated to false.
                            futures.get(1).cancel(true); // Interrupt computations of the true branch.

                            if (falseBranch != null) { // If false branch evaluated, send its value.
                                parentChannel.offer(new BoolWrapper(falseBranch.value(), index));
                                return;
                            }
                        }
                    }

                    // If both branches evaluated to the same value.
                    if (falseBranch != null && trueBranch != null
                            && falseBranch.value() == trueBranch.value()) {
                        futures.get(0).cancel(true); // Cancel computations of the condition.
                        parentChannel.offer(new BoolWrapper(trueBranch.value(), index)); // And send according value.
                        return;
                    }
                }
            } catch (InterruptedException e) {
                cancelEvaluation(futures);
                parentChannel.offer(new BoolWrapper(false, -1)); // Signal failure if interrupted.
            }

        }
    }
}
