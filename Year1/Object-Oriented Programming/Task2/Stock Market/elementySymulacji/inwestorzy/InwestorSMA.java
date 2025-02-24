package elementySymulacji.inwestorzy;

import elementySymulacji.zlecenia.TypZlecenia;
import elementySymulacji.zlecenia.Zlecenie;
import elementySymulacji.zlecenia.ZlecenieNE;
import wyjatki.IllegalStockId;
import wyjatki.NotEnoughFunds;
import elementySymulacji.systemTransakcyjny.SystemTransakcyjny;
import utils.Losowanie;
import utils.Pair;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;

public class InwestorSMA extends Inwestor {
    private static HashSet<String> akcjeWObiegu;
    private static final HashMap<String, ArrayList<Integer>> SMA5 = new HashMap<>();    // Ostatnie 5 cen.
    private static final HashMap<String, Integer> SMA5Suma = new HashMap<>();   // Suma ostatnich 5 cen.
    private static final HashMap<String, Integer> SMA5Poprzednie = new HashMap<>();     // Poprzednia wartość SMA5.
    private static final HashMap<String, ArrayList<Integer>> SMA10 = new HashMap<>();   // Ostatnie 10 cen.
    private static final HashMap<String, Integer> SMA10Suma = new HashMap<>();      // Suma ostatnich 10 cen.
    private static final HashMap<String, Integer> SMA10Poprzednie = new HashMap<>();    // Poprzednia wartość SMA10.
    private static int tura;

    static public void ustawPoczatkoweDane(HashSet<String> akcje) {
        akcjeWObiegu = akcje;
        for(String idAkcji : akcjeWObiegu) {
            SMA5.put(idAkcji, new ArrayList<>());
            SMA5Suma.put(idAkcji, 0);
            SMA5Poprzednie.put(idAkcji, 0);
            SMA10.put(idAkcji, new ArrayList<>());
            SMA10Suma.put(idAkcji, 0);
            SMA10Poprzednie.put(idAkcji, 0);
        }
        tura = -1;
    }

    @Override
    public void zlozZlecenie(SystemTransakcyjny systemTransakcyjny) {
        if(systemTransakcyjny.aktualnaTura() != tura) {     // Jeśli jeszcze żaden inwestor nie aktualizował bazy SMA.
            ++tura;

            // Dla każdej akcji w obiegu.
            for(String idAkcji : akcjeWObiegu) {
                Pair<Integer, Integer> ostatniaTransakcja = systemTransakcyjny.ostatniaTransakcja(idAkcji);

                if(SMA5.get(idAkcji).size() < 5) {
                    // Jeśli jeszcze nie da się policzyć SMA5 aktualizujemy sumę i dodajemy aktualną cenę do tablicy.
                    SMA5Suma.put(idAkcji, SMA5Suma.get(idAkcji) + ostatniaTransakcja.getArg2());
                    SMA5.get(idAkcji).add(ostatniaTransakcja.getArg2());
                }
                else {
                    // Jeśli już da się policzyć SMA5 aktualizujemy poprzednią i aktualną sumę SMA5.
                    SMA5Poprzednie.put(idAkcji, SMA5Suma.get(idAkcji));
                    SMA5Suma.put(idAkcji, SMA5Suma.get(idAkcji)
                            + ostatniaTransakcja.getArg2() - SMA5.get(idAkcji).get(0));

                    // Przesuwamy tablicę w lewo i ustawiamy ostatnią cenę.
                    for(int i = 0; i < 4; ++i)
                        SMA5.get(idAkcji).set(i, SMA5.get(idAkcji).get(i + 1));
                    SMA5.get(idAkcji).set(4, ostatniaTransakcja.getArg2());
                }

                if(SMA10.get(idAkcji).size() < 10) {
                    // Jeśli jeszcze nie da się policzyć SMA10 aktualizujemy sumę i dodajemy aktualną cenę do tablicy.
                    SMA10Suma.put(idAkcji, SMA10Suma.get(idAkcji) + ostatniaTransakcja.getArg2());
                    SMA10.get(idAkcji).add(ostatniaTransakcja.getArg2());
                }
                else {
                    // Jeśli już da się policzyć SMA10 aktualizujemy poprzednią i aktualną sumę SMA10.
                    SMA10Poprzednie.put(idAkcji, SMA10Suma.get(idAkcji));
                    SMA10Suma.put(idAkcji, SMA10Suma.get(idAkcji)
                            + ostatniaTransakcja.getArg2() - SMA10.get(idAkcji).get(0));

                    // Przesuwamy tablicę w lewo i ustawiamy ostatnią cenę.
                    for(int i = 0; i < 9; ++i)
                        SMA10.get(idAkcji).set(i, SMA10.get(idAkcji).get(i + 1));
                    SMA10.get(idAkcji).set(9, ostatniaTransakcja.getArg2());
                }
            }
        }

        // Dla każdej akcji.
        for(String idAkcji : akcjeWObiegu) {
            if(tura < 10) return;   // Jeśli nie da się policzyć SMA10, to nie zgłaszamy zlecenia.
            Pair<Integer, Integer> ostatniaTransakcja = systemTransakcyjny.ostatniaTransakcja(idAkcji);

            // Jeśli dostajemy dla tej akcji sygnał sprzedaży.
            if(SMA10Suma.get(idAkcji) / 10 - SMA5Suma.get(idAkcji) / 5 > 0 &&
                    SMA10Poprzednie.get(idAkcji) / 10 - SMA5Poprzednie.get(idAkcji) / 5 < 0) {
                if(portfelAkcji.get(idAkcji) > 0) {
                    // Jeśli posiadamy akcje tej firmy losujemy cenę i liczbę.
                    int cena = Losowanie.losuj(Math.max(ostatniaTransakcja.getArg2() - 10, 0),
                            ostatniaTransakcja.getArg2() + 10);
                    int liczbaAkcji = Losowanie.losuj(1, portfelAkcji.get(idAkcji));

                    // Zgłaszamy zlecenie.
                    try {
                        Zlecenie zlecenie = new ZlecenieNE(this, TypZlecenia.sprzedaz,
                                idAkcji, cena, liczbaAkcji, tura, systemTransakcyjny);
                        systemTransakcyjny.zlozZlecenie(zlecenie);
                    } catch (IllegalStockId e) {
                        System.err.println("Inwestor próbował złożyć zlecenie na nieistniejącą akcję.");
                    } catch (NotEnoughFunds e) {
                        System.err.println("Inwestor próbował złożyć zlecenie sprzeczne ze stanem jego portfela.");
                    }
                    return;
                }
            }

            // Jeśli dostajemy dla tej akcji sygnał kupna.
            if(SMA10Suma.get(idAkcji) / 10 - SMA5Suma.get(idAkcji) / 5 < 0 &&
                SMA10Poprzednie.get(idAkcji) / 10 - SMA5Poprzednie.get(idAkcji) / 5 > 0) {

                // Jeśli nie stać nas na jedną akcję, to nie zgłaszamy zlecenia.
                if(gotowka < ostatniaTransakcja.getArg2() - 10) return;

                // Losujemy cenę i liczbę akcji.
                int cena = Losowanie.losuj(Math.max(ostatniaTransakcja.getArg2() - 10, 1),
                        Math.min(ostatniaTransakcja.getArg2() + 10, gotowka));
                int liczbaAkcji = Losowanie.losuj(1, Math.floorDiv(gotowka, cena));

                // Jeśli nas nie stać, to nie zgłaszamy zlecenia.
                if(liczbaAkcji * cena > gotowka) return;

                // Zgłaszamy zlecenie.
                try {
                    Zlecenie zlecenie = new ZlecenieNE(this, TypZlecenia.kupno,
                            idAkcji, cena, liczbaAkcji, tura, systemTransakcyjny);
                    systemTransakcyjny.zlozZlecenie(zlecenie);
                } catch (IllegalStockId e) {
                    System.err.println("Inwestor próbował złożyć zlecenie na nieistniejącą akcję.");
                } catch (NotEnoughFunds e) {
                    System.err.println("Inwestor próbował złożyć zlecenie sprzeczne ze stanem jego portfela.");
                }
                return;
            }
        }
    }
}
