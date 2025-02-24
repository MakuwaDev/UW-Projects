package ruchMiejski;

public class Czas {
    private final int dzien;
    private final int minuty;

    public Czas(int dzien, int minuty) {
        assert minuty <= 1440 : "dana liczba minut nie mieści się w ciągu jednej doby";
        assert minuty >= 0 : "liczba minut musi być nieujemna";
        this.dzien = dzien;
        this.minuty = minuty;
    }

    public int getMinuty() {
        return minuty;
    }

    public int getDzien() {
        return dzien;
    }

    public String toString() {
        return dzien + ", " + ((minuty / 60 < 10) ? "0" : "") + (minuty / 60) + ":"
                + ((minuty % 60 < 10) ? "0" : "") + (minuty % 60);
    }
}
