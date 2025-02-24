package wyjatki;

// Wyjątek braku funduszy.
public class NotEnoughFunds extends Exception {
    public NotEnoughFunds(String s) {
        super(s);
    }
}
