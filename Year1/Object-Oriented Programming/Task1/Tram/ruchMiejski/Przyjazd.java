package ruchMiejski;

public class Przyjazd extends Zdarzenie {
    private final Tramwaj tramwaj;
    private final int idPrzystanku;
    public Przyjazd(Czas czas, Tramwaj tramwaj, int idPrzystanku) {
        super(czas);
        this.tramwaj = tramwaj;
        this.idPrzystanku = idPrzystanku;
    }

    // Wypuszcza pasażerów z tramwaju oraz ewentualnie dodaje
    // odjazd z przystanku o odopwiedniej godzinie.
    @Override
    public void przetworz(KolejkaZdarzen kolejkaZdarzen, Statystyka statystyka) {
        Linia linia = tramwaj.getLinia();
        Przystanek przystanek = linia.getPrzystanek(idPrzystanku);

        // Wypisywanie logu zdarzenia i wypuszczanie pasażerów.
        System.out.println(this);
        tramwaj.wypuscPasazerow(przystanek, getCzas(), statystyka);

        if(idPrzystanku != 0 && idPrzystanku != linia.getLiczbaPrzystankow() - 1) {
            // Przystanek nie jest pętlą, odjazd w tej samej minucie.
            kolejkaZdarzen.dodaj(new Odjazd(getCzas(), tramwaj, idPrzystanku));
        }
        else {
            // Przystanek jest pętlą, odjazd dopiero po czasie.
            Czas czasOdjazdu = new Czas(getCzas().getDzien(), getCzas().getMinuty()
                    + linia.getPrzejazd(linia.getLiczbaPrzystankow() - 1));

            // Tramwaj zmienia kierunek.
            if(tramwaj.getKierunek().equals("lewo")) tramwaj.setKierunek("prawo");
            else tramwaj.setKierunek("lewo");

            // Dodawanie odjazdu, o ile tramwaj nie jest na pętli domowej po 23.
            // W przeciwnym wypadku opróżnianie tramwaju
            // (nawet jeśli teoretycznie pasażerowie nie zmieściliby się na przystanku)
            if(przystanek != tramwaj.getPetla() || getCzas().getMinuty() <= 23 * 60)
                kolejkaZdarzen.dodaj(new Odjazd(czasOdjazdu, tramwaj, idPrzystanku));
            else tramwaj.oproznij(getCzas(), statystyka);
        }
    }

    @Override
    public String toString() {
        return getCzas().toString() + ": tramwaj linii " + tramwaj.getLinia().getNumer()
                + " (nr. boczny " + tramwaj.getNumerBoczny() + ") "
                + " przyjechał na przystanek " + tramwaj.getLinia().getPrzystanek(idPrzystanku);
    }
}
