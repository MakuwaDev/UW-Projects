package elementySymulacji.zlecenia;

import elementySymulacji.inwestorzy.Inwestor;
import elementySymulacji.systemTransakcyjny.SystemTransakcyjny;

// Zlecenie ważne do końca określonej tury.
public class ZlecenieDOT extends Zlecenie {
    protected int terminWaznosci;
    public ZlecenieDOT(Inwestor inwestor, TypZlecenia typZlecenia, String idAkcji, int liczbaSztuk, int limitCeny,
                       int tura, int terminWaznosci, SystemTransakcyjny systemTransakcyjny) {
        super(inwestor, typZlecenia, idAkcji, liczbaSztuk, limitCeny, tura, systemTransakcyjny);
        this.terminWaznosci = terminWaznosci;
    }

    public int getTerminWaznosci() {
        return terminWaznosci;
    }

    // Sprawdź czy zlecenie powinno zostać usunięte w danej turze.
    public boolean czyAktywne(int tura) {
        return terminWaznosci < tura;
    }

    @Override
    public String toString() {
        return super.toString() + "DOT z terminem ważności " + terminWaznosci;
    }
}
