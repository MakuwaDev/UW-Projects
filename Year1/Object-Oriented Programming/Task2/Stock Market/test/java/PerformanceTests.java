package test.java;

import main.GPWSimulation;
import org.junit.jupiter.api.Test;

import java.time.Duration;

import static org.junit.jupiter.api.Assertions.assertTimeout;

// Testy sprawdzające wydajność symulacji.
public class PerformanceTests {
    @Test
    void example() {
        assertTimeout(Duration.ofSeconds(30), () -> GPWSimulation.main(new String[] {"src/test/resources/example.txt", "100000"}));
    }

    @Test
    void manyRandom() {
        assertTimeout(Duration.ofSeconds(30), () -> GPWSimulation.main(new String[] {"src/test/resources/manyrandom.txt", "10000"}));
    }
}
