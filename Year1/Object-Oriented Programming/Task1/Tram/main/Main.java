package main;

import ruchMiejski.*;

import java.util.Scanner;

public class Main {
    static void przeprowadzSymulacje(DaneSymulacji dane, Statystyka statystyka) {
        int liczbaDni = dane.getLiczbaDni();
        int liczbaPasazerow = dane.getLiczbaPasazerow();
        int liczbaLinii = dane.getLiczbaLinii();
        Pasazer[] pasazerowie = dane.getPasazerowie();
        Linia[] linie = dane.getLinie();
        Przystanek[] przystanki = dane.getPrzystanki();

        for(int i = 0; i < liczbaDni; ++i) {
            KolejkaZdarzen kolejkaZdarzen = new Sterta();

            // Losowanie godzin pierwszych przyjść pasażerów na przystanki.
            for(int j = 0; j < liczbaPasazerow; ++j)
                kolejkaZdarzen.dodaj(new PobudkaPasazera(pasazerowie[j],
                        new Czas(i, Losowanie.losuj(6 * 60, 12 * 60))));

            // Dodawanie do kolejki zdarzeń pierwszych odjazdów tramwajów z pętli.
            for(int j = 0; j < liczbaLinii; ++j)
                linie[j].rozpocznijKursy(kolejkaZdarzen, i);

            // Przeprowadzanie właściwej symulacji.
            while(!kolejkaZdarzen.czyPusta()) {
                Zdarzenie zd = kolejkaZdarzen.wezPierwsze();
                zd.przetworz(kolejkaZdarzen, statystyka);
            }

            // Pasażerowie opuszczają przystanki o 24:00.
            for(Przystanek p : przystanki)
                p.oproznij(new Czas(i, 24 * 60), statystyka);
            statystyka.nowyDzien();
        }
    }

    public static void main(String[] args) {
        Scanner sc;
        Statystyka statystyka;
        DaneSymulacji dane;

        sc = new Scanner(System.in);
        dane = new DaneSymulacji(sc);

        dane.wczytajLiczbeDni();
        dane.wczytajDanePrzystankow();
        dane.wczytajDanePasazerow();
        dane.wczytajDaneTramwajow();
        dane.wczytajDaneLinii();

        statystyka = new Statystyka(dane.getLiczbaDni());

        dane.wypiszDane();
        przeprowadzSymulacje(dane, statystyka);
        statystyka.zakonczSymulacje();
    }
}
