package ruchMiejski;

public class Odjazd extends Zdarzenie {
    private final Tramwaj tramwaj;
    private final int idPrzystanku;
    public Odjazd(Czas czas, Tramwaj tramwaj, int idPrzystanku) {
        super(czas);
        this.tramwaj = tramwaj;
        this.idPrzystanku = idPrzystanku;
    }

    // Wpuszcza pasażerów z przystanku do tramwaju oraz
    // dodaje do kolejki przyjazd na następny przystanek.
    @Override
    public void przetworz(KolejkaZdarzen kolejkaZdarzen, Statystyka statystyka) {
        Linia linia = tramwaj.getLinia();
        Przystanek przystanek = linia.getPrzystanek(idPrzystanku);
        int idNastepnego;
        Czas czasNastepnego;

        // Obliczanie danych przyjazdu na następny przystanek.
        if(tramwaj.getKierunek().equals("prawo")) {
            // Tramwaj jedzie w "prawo".
            idNastepnego = idPrzystanku + 1;
            czasNastepnego = new Czas(getCzas().getDzien(),
                    getCzas().getMinuty() + linia.getPrzejazd(idPrzystanku));
        }
        else {
            // Tramwaj jedzie w "lewo".
            idNastepnego = idPrzystanku - 1;
            czasNastepnego = new Czas(getCzas().getDzien(),
                    getCzas().getMinuty() + linia.getPrzejazd(idPrzystanku - 1));
        }

        // Wpuszczanie pasażerów, dodanie do kolejki następnego przyjazdu
        // oraz wypisanie logu zdarzenia.
        tramwaj.wpuscPasazerow(przystanek, idPrzystanku, getCzas(), statystyka);
        kolejkaZdarzen.dodaj(new Przyjazd(czasNastepnego, tramwaj, idNastepnego));
        System.out.println(this);
    }

    @Override
    public String toString() {
        return getCzas().toString() + ": tramwaj linii " + tramwaj.getLinia().getNumer()
                + " (nr. boczny " + tramwaj.getNumerBoczny() + ") "
                + " odjechał z przystanku " + tramwaj.getLinia().getPrzystanek(idPrzystanku);
    }
}
