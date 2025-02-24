package main;

import elementySymulacji.inwestorzy.Inwestor;
import elementySymulacji.systemTransakcyjny.SystemTransakcyjny;
import elementySymulacji.systemTransakcyjny.SystemTransakcyjnyGPW;

import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;

public class GPWSimulation {

    @SuppressWarnings("unchecked")
    public static void main(String[] args) throws RuntimeException {
        String plik;
        int liczbaDni;
        Dane dane;
        SystemTransakcyjny systemTransakcyjny;
        ArrayList<Inwestor> inwestorzy;
        HashMap<String, Integer> akcjeWObwodzie;
        HashMap<String, Integer> poczatkowePortfolio;
        int poczatkowaGotowka;

        // Sprawdzanie liczby parametrów.
        if (args.length != 2) throw new IllegalArgumentException("Nieprawidłowa liczba argumentów.");

        // Otwieranie pliku i wczytywanie danych z pomocą klasy Dane.
        plik = args[0];
        try {
            liczbaDni = Integer.parseInt(args[1]);
            dane = new Dane(plik);
        } catch (NumberFormatException e) {
            throw new NumberFormatException("Liczba dni musi być dodatnią liczbą całkowitą.");
        } catch (IOException e) {
            throw new RuntimeException("Wystąpił błąd podczas odczytu pliku: " + e.getMessage());
        }

        // Sprawdzanie poprawności drugiego parametru (liczby dni symulacji).
        if (liczbaDni < 0) throw new NumberFormatException("Liczba dni musi być nieujemną liczbą całkowitą.");

        // Wczytywanie wartości z klasy Dane.
        inwestorzy = dane.getInwestorzy();
        akcjeWObwodzie = dane.getDostepneAkcje();
        poczatkowaGotowka = dane.getPoczatkowaGotowka();
        poczatkowePortfolio = dane.getPoczatkowePortfolio();

        // Tworzenie systemu transakcyjnego. Klonujemy listę inwestorów, aby system mógł ich permutować,
        // ale aby przy wypisywaniu byli w orginalnej kolejności.
        systemTransakcyjny = new SystemTransakcyjnyGPW((ArrayList<Inwestor>) inwestorzy.clone());

        // Dodawanie akcji do systemu.
        for(String idAkcji : akcjeWObwodzie.keySet())
            systemTransakcyjny.dodajAkcje(idAkcji, akcjeWObwodzie.get(idAkcji));

        // Ustawianie początkowych zawartości portfeli inwestorów.
        // Klonujemy początkowe portfolio, by były niezależne.
        for(Inwestor inwestor : inwestorzy) {
            inwestor.setGotowka(poczatkowaGotowka);
            inwestor.setPortfelAkcji((HashMap<String, Integer>) poczatkowePortfolio.clone());
        }

        // Przeprowadzanie właściwej symulacji.
        for(int i = 0; i < liczbaDni; ++i)
            systemTransakcyjny.przetworzTure();

        // Wypisywanie wyników symulacji.
        for (Inwestor inwestor : inwestorzy) {
            System.out.print(inwestor.getGotowka() + " ");
            for(String idAkcji : inwestor.getAkcje().keySet())
                System.out.print(idAkcji + ":" + inwestor.getAkcje().get(idAkcji) + " ");
            System.out.print("\n");
        }
    }
}
