package ruchMiejski;

import java.text.DecimalFormat;

public class Statystyka {
    private static final DecimalFormat df = new DecimalFormat("#.00");
    private final int liczbaDni;
    private final int[] dziennePrzejazdy;
    private final double[] dzienneSrednieOczekiwanie;
    private int dzisiaj;
    private int dzisiejszaLiczbaPrzejazdow;
    private int dzisiejszaliczaOczekiwan;
    private int dzisiejszyCzasOczekiwan;

    public Statystyka(int liczbaDni) {
        this.liczbaDni = liczbaDni;
        dziennePrzejazdy = new int[liczbaDni];
        dzienneSrednieOczekiwanie = new double[liczbaDni];
        dzisiaj = 0;
        dzisiejszaLiczbaPrzejazdow = 0;
        dzisiejszaliczaOczekiwan = 0;
        dzisiejszyCzasOczekiwan = 0;
    }

    // Pasażer raz przejechał tramwajem.
    public void dodajPrzejazd() {
        ++dzisiejszaLiczbaPrzejazdow;
    }

    // Pasażer czekał dlugoscOczekiwania na tramwaj.
    // To czy się doczekał czy nie, nie ma znaczenia.
    public void dodajOczekiwanie(int dlugoscOczekiwania) {
        dzisiejszyCzasOczekiwan += dlugoscOczekiwania;
        ++dzisiejszaliczaOczekiwan;
    }

    // Podsumowywanie danego dnia oraz
    // resetowanie wartości w preparacji do następnego dnia.
    public void nowyDzien() {
        dziennePrzejazdy[dzisiaj] = dzisiejszaLiczbaPrzejazdow;
        dzienneSrednieOczekiwanie[dzisiaj] = (double)dzisiejszyCzasOczekiwan / (double)dzisiejszaliczaOczekiwan;
        dzisiejszaLiczbaPrzejazdow = 0;
        dzisiejszaliczaOczekiwan = 0;
        dzisiejszyCzasOczekiwan = 0;
        ++dzisiaj;
    }

    // Podsumowywanie symulacji i wypisywanie statystyk.
    public void zakonczSymulacje() {
        int liczbaPrzejazdow = 0;
        double sredniCzasOczekiwania = 0.0;

        for(int i = 0; i < liczbaDni; ++i) {
            liczbaPrzejazdow += dziennePrzejazdy[i];
            sredniCzasOczekiwania += dzienneSrednieOczekiwanie[i];
        }
        sredniCzasOczekiwania /= liczbaDni;

        System.out.println("Sumaryczna liczba przejazdów: " + liczbaPrzejazdow);
        System.out.println("Średni czas oczekiwania: " + df.format(sredniCzasOczekiwania));

        for(int i = 0; i < liczbaDni; ++i) {
            System.out.println("Dzień " + i + ": ");
            System.out.println("Sumaryczna liczba przejazdów: " + dziennePrzejazdy[i]);
            System.out.println("Średni czas oczekiwania: " + df.format(dzienneSrednieOczekiwanie[i]));
        }
    }
}
