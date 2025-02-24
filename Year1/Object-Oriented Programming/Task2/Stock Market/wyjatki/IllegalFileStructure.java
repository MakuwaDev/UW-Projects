package wyjatki;

// Wyjątek błędnej struktury pliku.
public class IllegalFileStructure extends RuntimeException {
    public IllegalFileStructure(String s) {
        super(s);
    }
}
