package test.java;

import main.GPWSimulation;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.*;

// Testy sprawdzające czy na poprawnych danych symulacja funkcjonuje poprawnie.
public class CorectnessTests {
    private ConsoleCaptor consoleCaptor;
    @BeforeEach
    void prepare() {
        consoleCaptor = new ConsoleCaptor();
    }

    @AfterEach
    void restoreOut() {
        consoleCaptor.restoreOut();
    }

    @Test
    void example() {
        assertDoesNotThrow(() -> GPWSimulation.main(new String[] {"src/test/resources/example.txt", "10"}));
    }

    // Jako, że inwestorzy SMA odczytują te same sygnały oraz składają tylko zgłoszenia natychmiastowe, to
    // gdy nie ma żadniego inwestora losowego, nie powinni być w stanie dokonać żadnej transakcji.
    @Test
    void allSMA() {
        assertDoesNotThrow(() -> GPWSimulation.main(new String[] {"src/test/resources/allSMA.txt", "1000"}));
        String out = consoleCaptor.getOutput();
        String expected = """
                100000 MSFT:15 GOOGL:3 APL:3\s
                100000 MSFT:15 GOOGL:3 APL:3\s
                100000 MSFT:15 GOOGL:3 APL:3\s
                100000 MSFT:15 GOOGL:3 APL:3\s
                100000 MSFT:15 GOOGL:3 APL:3\s
                100000 MSFT:15 GOOGL:3 APL:3\s
                100000 MSFT:15 GOOGL:3 APL:3\s
                100000 MSFT:15 GOOGL:3 APL:3\s
                """;
        assertEquals(expected, out);
    }
}
