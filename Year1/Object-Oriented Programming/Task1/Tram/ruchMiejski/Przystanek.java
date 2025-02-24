package ruchMiejski;

public class Przystanek {
    private static int pojemnosc;
    private final String nazwa;
    private int liczbaOsob;
    private final Pasazer[] oczekujacy;

    public Przystanek(String nazwa) {
        this.nazwa = nazwa;
        this.liczbaOsob = 0;
        assert pojemnosc != 0: "pojemność przystanków nie została ustawiona";
        this.oczekujacy = new Pasazer[pojemnosc];
    }

    public static void setPojemnosc(int pojemnosc) {
        Przystanek.pojemnosc = pojemnosc;
    }

    public static int getPojemnosc() {
        return pojemnosc;
    }

    public int getLiczbaOsob() {
        return liczbaOsob;
    }

    public String toString() {
        return nazwa;
    }

    // Na przystanku pojawia się nowy pasażer.
    public void dodajPasazera(Pasazer pasazer) {
        assert liczbaOsob != pojemnosc : "przystanek jest pełny!";
        oczekujacy[liczbaOsob++] = pasazer;
    }

    // Pasażer opuszcza przystanek.
    Pasazer wezPasazera() {
        assert liczbaOsob != 0 : "przystanek jest pusty!";
        return oczekujacy[--liczbaOsob];
    }

    // Opróżnianie przystanku,
    // pasażerowie nie doczekali się na tramwaj i wracają do domu.
    public void oproznij(Czas czas, Statystyka statystyka) {
        while(liczbaOsob != 0) {
            Pasazer pasazer = wezPasazera();
            statystyka.dodajOczekiwanie(czas.getMinuty()
                    - pasazer.getPoczatekOczekiwania().getMinuty());
        }
    }
}
