package ruchMiejski;

import main.Losowanie;

public class Tramwaj extends Pojazd {
    private static int pojemnosc;
    private static int liczbaTramwajow = 0;
    private final Pasazer[] pasazerowie;
    private final Przystanek petla;
    private String kierunek;
    private int liczbaOsob;

    public Tramwaj(Linia linia, String kierunek, Przystanek petla) {
        super(liczbaTramwajow++, linia);
        this.kierunek = kierunek;
        this.petla = petla;
        liczbaOsob = 0;
        assert pojemnosc != 0 : "pojemność tramwajów nie została ustawiona!";
        pasazerowie = new Pasazer[pojemnosc];
    }

    public static void setPojemnosc(int pojemnosc) {
        Tramwaj.pojemnosc = pojemnosc;
    }

    public static int getPojemnosc() {
        return pojemnosc;
    }

    public String getKierunek() {
        return kierunek;
    }

    public void setKierunek(String kierunek) {
        this.kierunek = kierunek;
    }

    // Pasażer wchodzi do tramwaju.
    private void dodajPasazera(Pasazer pasazer, Czas czas, Statystyka statystyka) {
        liczbaOsob++;

        // Szukanie miejsca dla pasażera.
        for(int i = 0; i < pojemnosc; ++i)
            if(pasazerowie[i] == null) {
                pasazerowie[i] = pasazer;
                break;
            }

        // Dodawanie do statystyki czas oczekiwania pasażera na tramwaj.
        statystyka.dodajOczekiwanie(czas.getMinuty() - pasazer.getPoczatekOczekiwania().getMinuty());

        // Wypisywanie logu zdarzenia.
        System.out.println(czas + ": " + pasazer + " wsiadł do tramwaju linii "
                + getLinia().getNumer() + " (nr. bocz. " + getNumerBoczny()
                + ") z zamiarem dojechania do przystanku " + pasazer.getPrzystanekDocelowy());
    }

    // Pasażer wychodzi z tramwaju na swój przystanek docelowy.
    private void wysadzPasazera(int i, Przystanek przystanek, Czas czas, Statystyka statystyka) {
        --liczbaOsob;
        Pasazer pasazer = pasazerowie[i];

        // Po wyjściu z tramwaju, pasażer zaczyna oczekiwanie na tramwaj.
        pasazer.setPoczatekOczekiwania(czas);

        // Dodawanie przejazdu do statystyki, usuwanie pasażera z tablicy
        // oraz wypisywanie logu zdarzenia.
        statystyka.dodajPrzejazd();
        pasazerowie[i] = null;
        System.out.println(czas + ": " + pasazer + " wysiadł z tramwaju linii "
                + getLinia().getNumer() + " (nr. boczny " + getNumerBoczny()
                + ") na przystanku " + przystanek);
    }

    // Wpuszczanie pasażerów z przystanku.
    public void wpuscPasazerow(Przystanek przystanek, int idPrzystanku, Czas czas, Statystyka statystyka) {
        while(przystanek.getLiczbaOsob() != 0 && this.liczbaOsob != Tramwaj.getPojemnosc()) {
            Pasazer pasazer = przystanek.wezPasazera();
            int idDocelowego;

            // Losowanie przystanku docelowego dla danego pasażera.
            if(this.getKierunek().equals("prawo"))
                idDocelowego = Losowanie.losuj(idPrzystanku + 1, getLinia().getLiczbaPrzystankow() - 1);
            else idDocelowego = Losowanie.losuj(0, idPrzystanku - 1);

            // Ustawianie przystanku docelowego
            // oraz wejście pojedynczego pasażera do tramwaju.
            pasazer.setPrzystanekDocelowy(getLinia().getPrzystanek(idDocelowego));
            dodajPasazera(pasazer, czas, statystyka);
        }
    }

    // Wypuszczanie pasażerów z tramwaju na ich przystanek docelowy.
    public void wypuscPasazerow(Przystanek przystanek, Czas czas, Statystyka statystyka) {
        for(int i = 0; i < getPojemnosc(); ++i) {
            // Jeśli na przystanku już nie ma miejsca lub nie ma pasażerów w tramwaju
            // to już nic nie robimy. Jeśli na tym miejscu nie ma pasażera,
            // to omijamy to miejsce.
            if(przystanek.getLiczbaOsob() == Przystanek.getPojemnosc() || this.liczbaOsob == 0) break;
            if(pasazerowie[i] == null) continue;

            // I-ty pasażer chce wysiąść na aktualnym przystanku.
            if(pasazerowie[i].getPrzystanekDocelowy() == przystanek) {
                Pasazer pasazer = pasazerowie[i];
                przystanek.dodajPasazera(pasazer);
                wysadzPasazera(i, przystanek, czas, statystyka);
            }
        }
    }

    // Funkcja opróżnia tramwaj, gdy po raz ostatni danego dnia przyjedzie na
    // swoją pętlę. Pasażerowie wychodzą z tramwaju,nawet jeśli nie jest to ich
    // przystanek docelowy lub przystanek jest pełny. Wracają oni prosto do domu
    // nie czekając już na następny tramwaj.
    // To może powodować złudne przepełnienie przystanku w logach symulacji.
    public void oproznij(Czas czas, Statystyka statystyka) {
        for(int i = 0; i < pojemnosc; ++i)
            if(pasazerowie[i] != null) {
                --liczbaOsob;
                Pasazer pasazer = pasazerowie[i];
                pasazer.setPoczatekOczekiwania(czas);
                statystyka.dodajPrzejazd();
                pasazerowie[i] = null;
            }
    }

    public Przystanek getPetla() {
        return petla;
    }
}
