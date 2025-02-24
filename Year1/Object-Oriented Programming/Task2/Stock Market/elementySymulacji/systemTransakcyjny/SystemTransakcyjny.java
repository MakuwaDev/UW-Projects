package elementySymulacji.systemTransakcyjny;

import wyjatki.NotEnoughFunds;
import utils.Pair;
import elementySymulacji.zlecenia.Zlecenie;

import java.util.HashSet;

public interface SystemTransakcyjny {
    /// Dodaj akcję do systemu.
    void dodajAkcje(String idAkcji, int cena);
    // Zapytaj system o aktualną turę.
    int aktualnaTura();
    // Zapytaj system o akcje w obiegu.
    HashSet<String> dostepneAkcje();
    // Zapytaj system o turę (arg1) i cenę (arg2) ostatniej transakcji danej akcji.
    Pair<Integer, Integer> ostatniaTransakcja(String idAkcji);
    // Złóż zlecenie do systemu.
    void zlozZlecenie(Zlecenie zlecenie) throws NotEnoughFunds;
    // Przetwórz aktualną turę i wykonaj możliwe zlecenia.
    void przetworzTure();
}
