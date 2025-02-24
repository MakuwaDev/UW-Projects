package ruchMiejski;

public abstract class Pojazd {
    private final int numerBoczny;
    private final Linia linia;

    public Pojazd(int numerBoczny, Linia linia) {
        this.numerBoczny = numerBoczny;
        this.linia = linia;
    }

    public int getNumerBoczny() {
        return numerBoczny;
    }

    public Linia getLinia() {
        return linia;
    }
}
