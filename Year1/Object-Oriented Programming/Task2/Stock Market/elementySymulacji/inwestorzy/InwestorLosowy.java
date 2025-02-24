package elementySymulacji.inwestorzy;

import elementySymulacji.zlecenia.*;
import wyjatki.IllegalStockId;
import wyjatki.NotEnoughFunds;
import elementySymulacji.systemTransakcyjny.SystemTransakcyjny;
import utils.Losowanie;

import java.util.HashSet;
import java.util.Map;
import java.util.stream.Collectors;

public class InwestorLosowy extends Inwestor {
    @Override
    public void zlozZlecenie(SystemTransakcyjny systemTransakcyjny) {
        if(Losowanie.losuj(0, 1) == 0) return;  // 50% szans na brak zgłoszenia

        // Zmienne potrzebne do złożenia zlecenia.
        HashSet<String> akcjeWObwodzie = systemTransakcyjny.dostepneAkcje();
        TypZlecenia typZlecenia = Losowanie.losujZEnuma(TypZlecenia.class);
        Zlecenie zlecenie = null;
        int ostatniaCena, cena, liczbaSztuk;
        String idAkcji;

        if(typZlecenia == TypZlecenia.kupno) {
            if(gotowka == 0) return;    // Nie da się nic kupić bez pieniędzy.

            idAkcji = Losowanie.losujZeZbioru(akcjeWObwodzie);
            ostatniaCena = systemTransakcyjny.ostatniaTransakcja(idAkcji).getArg2();
            if(gotowka < ostatniaCena - 10) return; // Nie da się nic kupić, jeśli nie starczy nawet na jedną akcję.
            cena = Losowanie.losuj(Math.max(ostatniaCena - 10, 1), Math.min(ostatniaCena + 10, gotowka));
            liczbaSztuk = Losowanie.losuj(1, Math.floorDiv(gotowka, cena));

            if(liczbaSztuk * cena > gotowka) return;    // Brak zgłoszenia, jeśli nie ma funduszy.
        }
        else {
            // Jeśli nie się ma żadnej akcji, to nie da się żadnej sprzedać.
            if(portfelAkcji.values().stream().mapToInt(Integer::intValue).sum() == 0) return;

            // Dostępne akcje.
            HashSet<String> mojeAkcje = portfelAkcji.entrySet().stream()
                    .filter(entry -> entry.getValue() != 0).map(Map.Entry::getKey)
                    .collect(Collectors.toCollection(HashSet::new));

            idAkcji = Losowanie.losujZeZbioru(mojeAkcje);
            ostatniaCena = systemTransakcyjny.ostatniaTransakcja(idAkcji).getArg2();
            cena = Losowanie.losuj(Math.max(ostatniaCena - 10, 0), ostatniaCena + 10);
            liczbaSztuk = Losowanie.losuj(1, portfelAkcji.get(idAkcji));
        }

        int rodzajZlecenia = Losowanie.losuj(1, 100);

        if(rodzajZlecenia <= 60) {  // 60% na zlecenie natychmiastowe.
            try {
                zlecenie = new ZlecenieNE(this, typZlecenia, idAkcji, cena,
                        liczbaSztuk, systemTransakcyjny.aktualnaTura(), systemTransakcyjny);
            } catch (IllegalStockId e) {
                System.err.println("Inwestor próbował złożyć zlecenie na nieistniejącą akcję.");
            }
        }
        else if(rodzajZlecenia <= 90) {     // 30% na zlecenie do określonej tury.
            int termin;
            if(Losowanie.losuj(0, 1) == 0) {    // 50% na termin w przeciągu 5 tur.
                termin = Losowanie.losuj(systemTransakcyjny.aktualnaTura(),
                        systemTransakcyjny.aktualnaTura() + 5);
            }
            else {  // 50% na termin w przedziale [6,50] tur od aktualnej.
                termin = Losowanie.losuj(systemTransakcyjny.aktualnaTura() + 6,
                        systemTransakcyjny.aktualnaTura() + 50);
            }
            try {
                zlecenie = new ZlecenieDOT(this, typZlecenia, idAkcji, cena, liczbaSztuk,
                        systemTransakcyjny.aktualnaTura(), termin, systemTransakcyjny);
            } catch (IllegalStockId e) {
                System.err.println("Inwestor próbował złożyć zlecenie na nieistniejącą akcję.");
            }
        }
        else if(rodzajZlecenia <= 95) {     // 5% na zlecenie bezterminowe.
            try {
                zlecenie = new ZlecenieBTW(this, typZlecenia, idAkcji, cena,
                        liczbaSztuk, systemTransakcyjny.aktualnaTura(), systemTransakcyjny);
            } catch (IllegalStockId e) {
                System.err.println("Inwestor próbował złożyć zlecenie na nieistniejącą akcję.");
            }
        }
        else {  // 5% na zlecenie wykonaj lub anuluj.
            try {
                zlecenie = new ZlecenieWA(this, typZlecenia, idAkcji, cena, liczbaSztuk,
                        systemTransakcyjny.aktualnaTura(), systemTransakcyjny);
            } catch (IllegalStockId e) {
                System.err.println("Inwestor próbował złożyć zlecenie na nieistniejącą akcję.");
            }
        }

        try {
            systemTransakcyjny.zlozZlecenie(zlecenie);
        } catch (NotEnoughFunds e) {
            System.err.println("Inwestor próbował złożyć zlecenie sprzeczne ze stanem jego portfela.");
        }
    }
}
