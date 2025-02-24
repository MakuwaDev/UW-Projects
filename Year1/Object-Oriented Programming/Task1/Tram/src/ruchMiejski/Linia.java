package ruchMiejski;

public class Linia {
    private static int liczbaLinii = 0;
    private final int numer;
    private final int liczbaTramwajow;
    private final int liczbaPrzystankow;
    private final int interwal;
    private final int[] przejazdy;
    private final Przystanek[] przystanki;
    private final Tramwaj[] leweTramwaje;
    private final Tramwaj[] praweTramwaje;

    public Linia(int liczbaTramwajow, int liczbaPrzystankow, int[] przejazdy, Przystanek[] przystanki) {
        this.numer = liczbaLinii++;
        this.liczbaTramwajow = liczbaTramwajow;
        this.liczbaPrzystankow = liczbaPrzystankow;
        this.przejazdy = przejazdy;
        this.przystanki = przystanki;

        // Obliczanie interwału pomiędzy wyjazdami tramwajów z pętli.
        int czasPrzejazdu = 0;
        for(int x : przejazdy)
            czasPrzejazdu += x;
        czasPrzejazdu *= 2;
        interwal = czasPrzejazdu / liczbaTramwajow
                + ((czasPrzejazdu % liczbaPrzystankow == 0) ? 0 : 1);

        // Tworzenie tablic odpowiednich rozmiarów na tramwaje zaczynające kursy
        // z "lewej" i "prawej" pętli danej lini.
        leweTramwaje = new Tramwaj[liczbaTramwajow / 2 + ((liczbaTramwajow % 2 == 0) ? 0 : 1)];
        praweTramwaje = new Tramwaj[liczbaTramwajow / 2];

        // Tworzenie tramwajów kursujących na trasie.
        // Zauważmy, że tramwaj wyjeżdżający z "lewej" pętli zaczyna z kierunkiem "prawo"
        // i vice versa.
        for(int i = 0; i < leweTramwaje.length; ++i)
            leweTramwaje[i] = new Tramwaj(this, "prawo", przystanki[0]);
        for(int i = 0; i < praweTramwaje.length; ++i)
            praweTramwaje[i] = new Tramwaj(this, "lewo", przystanki[liczbaPrzystankow - 1]);
    }

    // Dodawanie do kolejki zdarzeń początkowych odjazdów tramwajów z pętli.
    public void rozpocznijKursy(KolejkaZdarzen kolejkaZdarzen, int dzien) {
        Odjazd odjazd;

        // Wypuszczanie na trasę tramwaji z "lewej" pętli.
        for(int i = 0; i < leweTramwaje.length; ++i) {
            odjazd = new Odjazd(new Czas(dzien, 360 + i * interwal),
                                leweTramwaje[i], 0);
            kolejkaZdarzen.dodaj(odjazd);
        }

        // Wypuszczanie na trasę tramwaji z "prawej" pętli.
        for(int i = 0; i < praweTramwaje.length; ++i) {
            odjazd = new Odjazd(new Czas(dzien, 360 + i * interwal),
                            praweTramwaje[i], przystanki.length - 1);
            kolejkaZdarzen.dodaj(odjazd);
        }
    }

    public int getNumer() {
        return numer;
    }

    public Przystanek getPrzystanek(int id) {
        return przystanki[id];
    }

    public int getLiczbaPrzystankow() {
        return liczbaPrzystankow;
    }

    public int getPrzejazd(int i) {
        return przejazdy[i];
    }

    public int getLiczbaTramwajow() {
        return liczbaTramwajow;
    }
}
