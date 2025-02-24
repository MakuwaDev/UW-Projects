package elementySymulacji.zlecenia;

import wyjatki.IllegalStockId;
import elementySymulacji.inwestorzy.Inwestor;
import elementySymulacji.systemTransakcyjny.SystemTransakcyjny;

// Zlecenie natychmiastowe.
public class ZlecenieNE extends ZlecenieDOT {
    public ZlecenieNE(Inwestor inwestor, TypZlecenia typZlecenia, String idAkcji, int liczbaSztuk, int limitCeny,
                      int tura, SystemTransakcyjny systemTransakcyjny) throws IllegalStockId {
        super(inwestor, typZlecenia, idAkcji, liczbaSztuk, limitCeny, tura, tura, systemTransakcyjny);
    }

    @Override
    public String toString() {
        return super.toString() + ", a dokładniej to zlecenie NE";
    }
}
