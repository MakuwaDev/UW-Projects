package ruchMiejski;

public class Pasazer {
    private static int liczbaPasazerow = 0;
    private final int numer;
    private final Przystanek dom;
    private Przystanek przystanekDocelowy;
    private Czas poczatekOczekiwania;

    public Pasazer(Przystanek dom) {
        this.dom = dom;
        numer = liczbaPasazerow++;

    }

    public int getNumer() {
        return numer;
    }

    public Przystanek getDom() {
        return dom;
    }

    public Przystanek getPrzystanekDocelowy() {
        return przystanekDocelowy;
    }

    public void setPrzystanekDocelowy(Przystanek przystanekDocelowy) {
        this.przystanekDocelowy = przystanekDocelowy;
    }

    public void setPoczatekOczekiwania(Czas poczatekOczekiwania) {
        this.poczatekOczekiwania = poczatekOczekiwania;
    }

    public Czas getPoczatekOczekiwania() {
        return poczatekOczekiwania;
    }

    @Override
    public String toString() {
        return "pasażer " + numer;
    }
}
