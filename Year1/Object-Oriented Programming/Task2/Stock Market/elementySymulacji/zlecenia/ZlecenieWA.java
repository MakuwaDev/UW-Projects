package elementySymulacji.zlecenia;

import elementySymulacji.inwestorzy.Inwestor;
import elementySymulacji.systemTransakcyjny.SystemTransakcyjny;
import wyjatki.IllegalStockId;

public class ZlecenieWA extends Zlecenie {

    public ZlecenieWA(Inwestor inwestor, TypZlecenia typZlecenia, String idAkcji, int limitCeny,
                      int liczbaSztuk, int tura, SystemTransakcyjny systemTransakcyjny) throws IllegalStockId {
        super(inwestor, typZlecenia, idAkcji, limitCeny, liczbaSztuk, tura, systemTransakcyjny);
    }
}
