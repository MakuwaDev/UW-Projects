package main;

import java.util.Random;

public class Losowanie {
    private final static Random rand = new Random();

    // Losuje losową liczbę z zakresu [dolna, gorna].
    public static int losuj(int dolna, int gorna) {
        return rand.nextInt(dolna, gorna + 1);
    }
}
