package test.java;

import java.io.ByteArrayOutputStream;
import java.io.PrintStream;

// Klasa do przechwytywania standardowego wyjścia w celu testowania.
public class ConsoleCaptor {
    private final ByteArrayOutputStream byteArrayOutputStream;
    private final PrintStream originalOut;

    public ConsoleCaptor() {
        byteArrayOutputStream = new ByteArrayOutputStream();
        originalOut = System.out;
        System.setOut(new PrintStream(byteArrayOutputStream));
    }

    public String getOutput() {
        System.out.flush();
        return byteArrayOutputStream.toString();
    }

    public void restoreOut() {
        System.setOut(originalOut);
    }
}
