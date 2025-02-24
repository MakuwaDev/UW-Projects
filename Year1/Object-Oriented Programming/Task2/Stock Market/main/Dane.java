package main;

import wyjatki.IllegalFileStructure;
import wyjatki.IllegalStockId;
import elementySymulacji.inwestorzy.Inwestor;
import elementySymulacji.inwestorzy.InwestorLosowy;
import elementySymulacji.inwestorzy.InwestorSMA;

import java.io.File;
import java.io.IOException;
import java.nio.file.Files;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;

public class Dane{
    private final List<String> linie;
    private final ArrayList<Inwestor> inwestorzy;
    private final HashMap<String, Integer> dostepneAkcje;
    private final HashMap<String, Integer> poczatkowePortfolio;
    private final int poczatkowaGotowka;
    private int numerLini;

    // Wczytaj z pliku następną linię, która nie zaczyna się na '#'.
    private String nastepnaLinia() throws IllegalFileStructure {
        if (numerLini == linie.size())
            throw new IllegalFileStructure("Podczas wczytywania danych, plik niespodziewanie się zakończył.");

        while (linie.get(numerLini).charAt(0) == '#') {
            ++numerLini;
            if (numerLini == linie.size())
                throw new IllegalFileStructure("Podczas wczytywania danych, plik niespodziewanie się zakończył.");
        }

        return linie.get(numerLini++);
    }

    public Dane(String nazwaPliku) throws IOException {
        File plik = new File(nazwaPliku);
        linie = Files.readAllLines(plik.toPath());
        numerLini = 0;

        // Ustawianie odpowiednie wartości.
        String linia;
        int i = 0, next;
        dostepneAkcje = new HashMap<>();
        inwestorzy = new ArrayList<>();
        poczatkowePortfolio = new HashMap<>();

        // Wczytywanie danych inwestorów.
        linia = nastepnaLinia();
        if(linia.length() % 2 == 0) throw new IllegalFileStructure("Wystąpił błąd podczas wczytywania inwestorów.");
        while(i < linia.length()) {
            if(i != linia.length() - 1 && linia.charAt(i + 1) != ' ')
                throw new IllegalFileStructure("Wystąpił błąd podczas wczytywania inwestorów.");

            if(linia.charAt(i) == 'R') inwestorzy.add(new InwestorLosowy());
            else if(linia.charAt(i) == 'S') inwestorzy.add(new InwestorSMA());
            else throw new IllegalFileStructure("Wystąpił błąd podczas wczytywania inwestorów.");

            i += 2;
        }

        // Przygotowanie do wczytywania danych akcji.
        i = 0;
        linia = nastepnaLinia();

        // Wczytywanie danych akcji.
        while(i < linia.length()) {
            next = linia.indexOf(' ', i);
            if(next == -1) next = linia.length();

            String s = linia.substring(i, next);
            int indeks = s.indexOf(':');
            if(indeks == -1) throw new IllegalFileStructure("Wystąpił błąd podczas wczytywania akcji.");

            String akcja = s.substring(0, indeks);
            String cenaString = s.substring(indeks + 1);
            int cena;

            try {
                cena = Integer.parseInt(cenaString);
            } catch (NumberFormatException e) {
                throw new NumberFormatException("Cena akcji musi być dodatnią liczbą całkowitą.");
            }

            if (cena <= 0) throw new NumberFormatException("Cena akcji musi być dodatnią liczbą całkowitą.");

            if(dostepneAkcje.containsKey(akcja)) throw new IllegalStockId("Ta akcja jest już częścią systemu.");
            dostepneAkcje.put(akcja, cena);
            i = next + 1;
        }

        // Przygotowanie do wczytania początkowych wartości portfeli inwestorów.
        HashSet<String> akcjeWObiegu = new HashSet<>(dostepneAkcje.keySet());
            InwestorSMA.ustawPoczatkoweDane(akcjeWObiegu);
        linia = nastepnaLinia();

        // Wczytywanie początkowej liczby pieniędzy inwestorów.
        next = linia.indexOf(' ');
        if(next == -1) throw new IllegalFileStructure("Wystąpił błąd podczas wczytywania początkowych portfeli.");

        try {
            poczatkowaGotowka = Integer.parseInt(linia.substring(0, next));
        } catch (NumberFormatException e) {
            throw new NumberFormatException("Początkowa zawartość portfeli musi być nieujemną liczbą całkowitą.");
        }

        if(poczatkowaGotowka < 0)
            throw new NumberFormatException("Początkowa zawartość portfeli musi być nieujemną liczbą całkowitą.");

        // Przygotowywanie do wczytywania danych o startowych portfoliach inwestorów.
        i = next + 1;

        // Wczytywanie startowych portfoliów inwestorów.
        while(i < linia.length()) {
            next = linia.indexOf(' ', i);
            if(next == -1) next = linia.length();

            String s = linia.substring(i, next);
            int indeks = s.indexOf(':');
            if(indeks == -1) throw new IllegalFileStructure("Wystąpił błąd podczas wczytywania zawartości portfeli.");

            String akcja = s.substring(0, indeks);
            String ileString = s.substring(indeks + 1);
            int ile;

            try {
                ile = Integer.parseInt(ileString);
            } catch (NumberFormatException e) {
                throw new NumberFormatException("Liczba akcji musi być dodatnią liczbą całkowitą.");
            }

            if (ile <= 0) throw new NumberFormatException("Liczba akcji musi być dodatnią liczbą całkowitą.");
            if(!dostepneAkcje.containsKey(akcja))
                throw new IllegalStockId("Akcja o identyfikatorze " + akcja + " nie należy do systemu");
            if(poczatkowePortfolio.containsKey(akcja))
                throw new IllegalStockId("Liczba tych akcji w początkowym portfolio inwestorów już była specyfikowana.");

            poczatkowePortfolio.put(akcja, ile);
            i = next + 1;
        }

        // Sprawdzenie czy plik zawiera "śmieci" na końcu, czyli linie niebędące komentarzami.
        for(i = numerLini; i < linie.size(); ++i)
            if(linie.get(i).charAt(0) != '#') throw new IllegalFileStructure("Plik zawiera za dużo danych.");
    }

    public ArrayList<Inwestor> getInwestorzy() {
        return inwestorzy;
    }

    public HashMap<String, Integer> getDostepneAkcje() {
        return dostepneAkcje;
    }

    public HashMap<String, Integer> getPoczatkowePortfolio() {
        return poczatkowePortfolio;
    }

    public int getPoczatkowaGotowka() {
        return poczatkowaGotowka;
    }
}
