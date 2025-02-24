package ruchMiejski;

public class PobudkaPasazera extends Zdarzenie {
    private final Pasazer pasazer;

    public PobudkaPasazera(Pasazer pasazer, Czas czasPobudki) {
        super(czasPobudki);
        this.pasazer = pasazer;
    }

    // Pasazer próbuje przyjść na przystanek, po raz pierwszy danego dnia.
    // Jeśli przystanek jest pełny, to wraca do domu
    // i rezygnuje z podróżowania tego dnia.
    @Override
    public void przetworz(KolejkaZdarzen kolejkaZdarzen, Statystyka statystyka) {
        if(pasazer.getDom().getLiczbaOsob() < Przystanek.getPojemnosc()) {
            pasazer.setPoczatekOczekiwania(getCzas());
            pasazer.getDom().dodajPasazera(pasazer);
            System.out.println(this + ", na którym było miejsce");
        }
        else System.out.println(this + ", na którym nie było miejsca");
    }

    @Override
    public String toString() {
        return getCzas().toString() + " " + pasazer + " obudził się i poszedł na przystanek " + pasazer.getDom();
    }
}
