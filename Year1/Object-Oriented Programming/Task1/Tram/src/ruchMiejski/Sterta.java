package ruchMiejski;

public class Sterta implements KolejkaZdarzen {

    // Aby zachować stabilność kolejki implementowanej na stercie,
    // każdemu zdarzeniu przypisujemy numer identyfikacyjny
    // równy "czasowi" wrzucenia na kolejkę.
    private class NumerowaneZdarzenie {
        private final Zdarzenie zdarzenie;
        private final int id;

        public NumerowaneZdarzenie(Zdarzenie zdarzenie) {
            this.zdarzenie = zdarzenie;
            this.id = maxId++;
        }

        // Przy porównywaniu zdarzeń nie uwzględniamy dnia zdarzenia,
        // gdyż zadanie nie przewiduje sytuacji w której
        // na kolejce są dwa zdarzenia z innych dni.
        public boolean mniejszy(NumerowaneZdarzenie b) {
            if(this.zdarzenie.getCzas().getMinuty() != b.zdarzenie.getCzas().getMinuty())
                return this.zdarzenie.getCzas().getMinuty() < b.zdarzenie.getCzas().getMinuty();
            return this.id < b.id;
        }

        public Zdarzenie getZdarzenie() {
            return zdarzenie;
        }
    }
    private int maxId;
    private int liczbaElementow;
    private NumerowaneZdarzenie[] zdarzenia;

    public Sterta() {
        liczbaElementow = 0;
        maxId = 0;
        zdarzenia = new NumerowaneZdarzenie[2];
    }

    // Tablic przechowująca stertę jest za małą,
    // rozszerzamy ją podwójnie, tak jak vector.
    private void rozszerz() {
        NumerowaneZdarzenie[] tmp = new NumerowaneZdarzenie[2 * zdarzenia.length];
        for(int i = 1; i < zdarzenia.length; ++i)
            tmp[i] = zdarzenia[i];
        zdarzenia = tmp;
    }

    // Rodzic elementu z indeksem i w tablicy trzymającej stertę.
    private int rodzic(int i) {
        return i / 2;
    }

    // Lewy syn elementu z indeksem i w tablicy trzymającej stertę.
    private int lewySyn(int i) {
        return 2 * i;
    }

    // Prawy syn elementu z indeksem i w tablicy trzymającej stertę.
    private int prawySyn(int i) {
        return 2 * i + 1;
    }

    // Aktualizowanie struktury sterty, poprzez przerzucanie elementu w górę
    // struktury, tak by zachowana była własność sterty.
    private void przerzucWGore(int i) {
        while(i > 1 && zdarzenia[i].mniejszy(zdarzenia[rodzic(i)])) {
          NumerowaneZdarzenie tmp = zdarzenia[rodzic(i)];
          zdarzenia[rodzic(i)] = zdarzenia[i];
          zdarzenia[i] = tmp;

          i = rodzic(i);
        }
    }

    // Aktualizowanie struktury sterty, poprzez przerzucanie elementu w dół
    // struktury, tak by zachowana była własność sterty.
    private void przerzucWDol(int i) {
        int najmniejszy = i;

        if(lewySyn(i) <= liczbaElementow && zdarzenia[lewySyn(i)].mniejszy(zdarzenia[najmniejszy]))
            najmniejszy = lewySyn(i);
        if(prawySyn(i) <= liczbaElementow && zdarzenia[prawySyn(i)].mniejszy(zdarzenia[najmniejszy]))
            najmniejszy = prawySyn(i);

        if(i != najmniejszy) {
            NumerowaneZdarzenie tmp = zdarzenia[i];
            zdarzenia[i] = zdarzenia[najmniejszy];
            zdarzenia[najmniejszy] = tmp;
            przerzucWDol(najmniejszy);
        }
    }

    // Dodawanie elementu na stertę
    @Override
    public void dodaj(Zdarzenie zdarzenie) {
        NumerowaneZdarzenie numerowaneZdarzenie = new NumerowaneZdarzenie(zdarzenie);

        // Tablica jest za mała, należy ją rozszerzyć.
        if(liczbaElementow == zdarzenia.length - 1)
            rozszerz();

        // Wrzucamy zdarzenie na koniec struktury i aktualizujemy ją,
        // żeby zachować własność sterty.
        zdarzenia[++liczbaElementow] = numerowaneZdarzenie;
        przerzucWGore(liczbaElementow);
    }

    // Zdejmowanie najmniejszego elementu z kolejki.
    @Override
    public Zdarzenie wezPierwsze() {
        assert liczbaElementow != 0;
        Zdarzenie wyn = zdarzenia[1].getZdarzenie();

        // Nadpisujemy korzeń struktury ostatnim elementem
        // i aktualizujemy strukturę, żeby zachować własność sterty.
        zdarzenia[1] = zdarzenia[liczbaElementow--];
        przerzucWDol(1);

        return wyn;
    }

    @Override
    public boolean czyPusta() {
        return liczbaElementow == 0;
    }
}
