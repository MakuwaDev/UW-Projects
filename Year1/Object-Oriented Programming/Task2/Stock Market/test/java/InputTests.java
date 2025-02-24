package test.java;

import main.GPWSimulation;
import org.junit.jupiter.api.Nested;
import org.junit.jupiter.api.Test;
import wyjatki.IllegalFileStructure;
import wyjatki.IllegalStockId;

import static org.junit.jupiter.api.Assertions.assertThrows;

// Testy sprawdzające reakcje na błędne dane.
public class InputTests {
    // Testy sprawdzające reakcje na błędne parametry.
    @Nested
    class IllegalArgumentTests {
        @Test
        void noArguments() {
            assertThrows(IllegalArgumentException.class,
                    () -> GPWSimulation.main(new String[] {}));
        }

        @Test
        void oneArgument() {
            assertThrows(IllegalArgumentException.class,
                    () -> GPWSimulation.main(new String[] {"plik.txt"}));
        }

        @Test
        void threeArguments() {
            assertThrows(IllegalArgumentException.class,
                    () -> GPWSimulation.main(new String[] {"plik.txt", "10", "?"}));
        }
    }

    // Testy sprawdzające reakcje na błędy wartości liczbowych.
    @Nested
    class NumberFormatTests {
        @Test
        void notANumberDayCount() {
            assertThrows(NumberFormatException.class,
                    () -> GPWSimulation.main(new String[] {"plik.txt", "abc"}));
        }

        @Test
        void negativeDayCount() {
            assertThrows(NumberFormatException.class,
                    () -> GPWSimulation.main(new String[] {"src/test/resources/example.txt", "-10"}));
        }

        @Test
        void negativeStockPrice() {
            assertThrows(NumberFormatException.class,
                    () -> GPWSimulation.main(new String[] {"src/test/resources/wrongstocks1.txt", "10"}));
        }

        @Test
        void notANumberStockPrice() {
            assertThrows(NumberFormatException.class,
                    () -> GPWSimulation.main(new String[] {"src/test/resources/wrongstocks2.txt", "10"}));

        }

        @Test
        void notANumberCashAmount() {
            assertThrows(NumberFormatException.class,
                    () -> GPWSimulation.main(new String[] {"src/test/resources/wrongportfolio1.txt", "10"}));
        }

        @Test
        void negativeCashAmount() {
            assertThrows(NumberFormatException.class,
                    () -> GPWSimulation.main(new String[] {"src/test/resources/wrongportfolio2.txt", "10"}));
        }
    }

    // Test sprawdzający reakcje na brak podanego pliku.
    @Test
    void fileNotFound() {
        assertThrows(RuntimeException.class,
                () -> GPWSimulation.main(new String[] {"src/test/resources/tenpliknieistnieje.txt", "10"}));
    }

    // Testy sprawdzające reakcje na błędy w strukturze danych w pliku.
    @Nested
    class IllegalFileStructureTests {
        @Test
        void emptyFile() {
            assertThrows(IllegalFileStructure.class,
                    () -> GPWSimulation.main(new String[] {"src/test/resources/empty.txt", "10"}));
        }

        @Test
        void allCommentsFile() {
            assertThrows(IllegalFileStructure.class,
                    () -> GPWSimulation.main(new String[] {"src/test/resources/comments.txt", "10"}));
        }

        @Test
        void incompleteFile() {
            assertThrows(IllegalFileStructure.class,
                    () -> GPWSimulation.main(new String[] {"src/test/resources/incomplete.txt", "10"}));
        }

        @Test
        void illegalInvestorsStructure1() {
            assertThrows(IllegalFileStructure.class,
                    () -> GPWSimulation.main(new String[] {"src/test/resources/wronginvestors1.txt", "10"}));
        }

        @Test
        void illegalInvestorsStructure2() {
            assertThrows(IllegalFileStructure.class,
                    () -> GPWSimulation.main(new String[] {"src/test/resources/wronginvestors2.txt", "10"}));
        }

        @Test
        void illegalInvestorsStructure3() {
            assertThrows(IllegalFileStructure.class,
                    () -> GPWSimulation.main(new String[] {"src/test/resources/wronginvestors3.txt", "10"}));
        }

        @Test
        void illegalStocksStructure1() {
            assertThrows(IllegalFileStructure.class,
                    () -> GPWSimulation.main(new String[] {"src/test/resources/wrongstocks3.txt", "10"}));
        }

        @Test
        void fileDidntEnd() {
            assertThrows(IllegalFileStructure.class,
                    () -> GPWSimulation.main(new String[] {"src/test/resources/trash.txt", "10"}));
        }

    }

    // Testy sprawdzające reakcje na błędy związane z nazwami akcji.
    @Nested
    class IllegalStockIdTests {
        @Test
        void illegalStockIdCharacters1() {
            assertThrows(IllegalStockId.class,
                    () -> GPWSimulation.main(new String[] {"src/test/resources/wrongstocks4.txt", "10"}));
        }

        @Test
        void emptyStockId1() {
            assertThrows(IllegalStockId.class,
                    () -> GPWSimulation.main(new String[] {"src/test/resources/wrongstocks5.txt", "10"}));
        }

        @Test
        void illegalStockIdLength1() {
            assertThrows(IllegalStockId.class,
                    () -> GPWSimulation.main(new String[] {"src/test/resources/wrongstocks6.txt", "10"}));
        }

        @Test
        void stockAlreadyDeclared1() {
            assertThrows(IllegalStockId.class,
                    () -> GPWSimulation.main(new String[] {"src/test/resources/wrongstocks7.txt", "10"}));
        }

        @Test
        void illegalStockIdCharacters2() {
            assertThrows(IllegalStockId.class,
                    () -> GPWSimulation.main(new String[] {"src/test/resources/wrongportfolio3.txt", "10"}));
        }

        @Test
        void emptyStockId2() {
            assertThrows(IllegalStockId.class,
                    () -> GPWSimulation.main(new String[] {"src/test/resources/wrongportfolio4.txt", "10"}));
        }

        @Test
        void illegalStockIdLength2() {
            assertThrows(IllegalStockId.class,
                    () -> GPWSimulation.main(new String[] {"src/test/resources/wrongportfolio5.txt", "10"}));
        }

        @Test
        void stockAlreadyDeclared2() {
            assertThrows(IllegalStockId.class,
                    () -> GPWSimulation.main(new String[] {"src/test/resources/wrongportfolio6.txt", "10"}));
        }

        @Test
        void stockNotFound() {
            assertThrows(IllegalStockId.class,
                    () -> GPWSimulation.main(new String[] {"src/test/resources/wrongportfolio7.txt", "10"}));
        }
    }
}
