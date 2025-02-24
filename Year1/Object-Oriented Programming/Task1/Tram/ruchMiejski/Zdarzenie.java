package ruchMiejski;

public abstract class Zdarzenie {
    private final Czas czas;

    public Zdarzenie(Czas czas) {
        this.czas = czas;
    }
    public Czas getCzas() {
        return czas;
    }

    public abstract void przetworz(KolejkaZdarzen kolejkaZdarzen, Statystyka statystyka);
}
