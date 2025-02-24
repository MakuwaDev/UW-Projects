package wyjatki;

// Wyjątek błędnej nazwy akcji.
public class IllegalStockId extends RuntimeException {
    public IllegalStockId(String s) {
        super(s);
    }
}
