package ruchMiejski;

import main.Losowanie;

import java.util.Scanner;

public class DaneSymulacji {
    private final Scanner sc;
    private int liczbaDni;
    private int liczbaPrzystankow;
    private int liczbaPasazerow;
    private int liczbaLinii;
    private Przystanek[] przystanki;
    private Pasazer[] pasazerowie;
    private Linia[] linie;

    public DaneSymulacji(Scanner sc) {
        this.sc = sc;
    }

    public void wczytajLiczbeDni() {
        liczbaDni = sc.nextInt();
    }

    public void wczytajDanePrzystankow() {
        int pojemnoscPrzystankow = sc.nextInt();
        Przystanek.setPojemnosc(pojemnoscPrzystankow);
        liczbaPrzystankow = sc.nextInt();
        przystanki = new Przystanek[liczbaPrzystankow];

        for(int i = 0; i < liczbaPrzystankow; ++i)
            przystanki[i] = new Przystanek(sc.next());
    }

    public void wczytajDanePasazerow() {
        liczbaPasazerow = sc.nextInt();
        pasazerowie = new Pasazer[liczbaPasazerow];

        for(int i = 0; i < liczbaPasazerow; ++i)
            pasazerowie[i] = new Pasazer(przystanki[Losowanie.losuj(0, liczbaPrzystankow - 1)]);
    }

    public void wczytajDaneTramwajow() {
        int pojemnoscTramwaju = sc.nextInt();
        Tramwaj.setPojemnosc(pojemnoscTramwaju);
    }

    public void wczytajDaneLinii() {
        liczbaLinii = sc.nextInt();
        linie = new Linia[liczbaLinii];
        for(int i = 0; i < liczbaLinii; ++i) {
            int liczbaTramwajow = sc.nextInt();
            int dlugosc = sc.nextInt();
            int[] przejazdy = new int[dlugosc];
            Przystanek[] przystankiLini = new Przystanek[dlugosc];

            for(int j = 0; j < dlugosc; ++j) {
                String nazwa = sc.next();
                przejazdy[j] = sc.nextInt();
                for(int k = 0; k < liczbaPrzystankow; ++k)
                    if(nazwa.equals(przystanki[k].toString())) {
                        przystankiLini[j] = przystanki[k];
                        break;
                    }
            }

            linie[i] = new Linia(liczbaTramwajow, dlugosc, przejazdy, przystankiLini);
        }
    }

    // Wypisywanie parametrów symulacji.
    public void wypiszDane() {
        System.out.println("Dni symulacji: " + liczbaDni
                + "\nPojemność przystanku: " + Przystanek.getPojemnosc()
                + "\nLiczba przystanków: " + liczbaPrzystankow
                + "\nNazwy przystanków: ");

        for(int i = 0; i < liczbaPrzystankow; ++i)
            System.out.println(przystanki[i]);

        System.out.println("Liczba pasażerów: " + liczbaPasazerow
                + "\nPojemność tramwaju: " + Tramwaj.getPojemnosc()
                + "\nLiczba linii: " + liczbaLinii);

        for(int i = 0; i < liczbaLinii; ++i) {
            System.out.println("Linia " + i +
                    ":\nLiczba tramwajów: " + linie[i].getLiczbaTramwajow()
                    + "\nDługość trasy: " + linie[i].getLiczbaPrzystankow());

            for(int j = 0; j < linie[i].getLiczbaPrzystankow(); ++j) {

                System.out.print(linie[i].getPrzystanek(j) + " "
                                + linie[i].getPrzejazd(j) + " ");
            }

            System.out.println();
        }

        System.out.println();
    }

    public int getLiczbaDni() {
        return liczbaDni;
    }

    public int getLiczbaPasazerow() {
        return liczbaPasazerow;
    }

    public int getLiczbaLinii() {
        return liczbaLinii;
    }

    public Pasazer[] getPasazerowie() {
        return pasazerowie;
    }

    public Przystanek[] getPrzystanki() {
        return przystanki;
    }

    public Linia[] getLinie() {
        return linie;
    }
}
