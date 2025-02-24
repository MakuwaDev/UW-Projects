package elementySymulacji.zlecenia;

import elementySymulacji.inwestorzy.Inwestor;
import wyjatki.IllegalStockId;
import elementySymulacji.systemTransakcyjny.SystemTransakcyjny;

public abstract class Zlecenie {
    protected final Inwestor inwestor;
    protected final TypZlecenia typZlecenia;
    protected final String idAkcji;
    protected final int limitCeny;
    protected final int tura;
    protected int idZlecenia;
    protected int liczbaSztuk;

    public Zlecenie(Inwestor inwestor, TypZlecenia typZlecenia, String idAkcji, int limitCeny,
                    int liczbaSztuk, int tura, SystemTransakcyjny systemTransakcyjny) throws IllegalStockId {

        if(!systemTransakcyjny.dostepneAkcje().contains(idAkcji))
            throw new IllegalStockId("Dana akcja nie należy do systemu.");

        this.inwestor = inwestor;
        this.typZlecenia = typZlecenia;
        this.idAkcji = idAkcji;
        this.limitCeny = limitCeny;
        this.liczbaSztuk = liczbaSztuk;
        this.tura = tura;
    }

    public Inwestor getInwestor() {
        return inwestor;
    }

    public TypZlecenia getTypZlecenia() {
        return typZlecenia;
    }

    public String getIdAkcji() {
        return idAkcji;
    }

    public int getIdZlecenia() {
        return idZlecenia;
    }

    public int getTura() {
        return tura;
    }

    public int getLiczbaSztuk() {
        return liczbaSztuk;
    }

    public int getLimitCeny() {
        return limitCeny;
    }

    public void setIdZlecenia(int idZlecenia) {
        this.idZlecenia = idZlecenia;
    }

    public void setLiczbaSztuk(int liczbaSztuk) {
        this.liczbaSztuk = liczbaSztuk;
    }

    @Override
    public String toString() {
        return "Zlecenie " + typZlecenia + " złożone w turze " + tura + " z numerem " + idZlecenia + " na "
                + liczbaSztuk + " akcji " + idAkcji + " z limitem ceny " + limitCeny + ", rodzaju ";
    }
}
