package cp2024.solution;

import cp2024.circuit.CircuitValue;

// CircuitValue that throws InterruptedException upon getValue() calls.
// To be returned by the solver if solved() is called after stop().
public class BrokenCircuitValue implements CircuitValue {
    @Override
    public boolean getValue() throws InterruptedException {
        throw new InterruptedException();
    }
}
