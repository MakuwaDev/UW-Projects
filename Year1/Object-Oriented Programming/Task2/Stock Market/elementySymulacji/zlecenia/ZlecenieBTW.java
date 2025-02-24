package elementySymulacji.zlecenia;

import wyjatki.IllegalStockId;
import elementySymulacji.inwestorzy.Inwestor;
import elementySymulacji.systemTransakcyjny.SystemTransakcyjny;

// Zlecenie bez terminu ważości.
public class ZlecenieBTW extends Zlecenie {
    public ZlecenieBTW(Inwestor inwestor, TypZlecenia typZlecenia, String idAkcji, int liczbaSztuk,
                       int limitCeny, int tura, SystemTransakcyjny systemTransakcyjny) throws IllegalStockId {
        super(inwestor, typZlecenia, idAkcji, liczbaSztuk, limitCeny, tura, systemTransakcyjny);
    }

    @Override
    public String toString() {
        return super.toString() + "BTW ";
    }
}
