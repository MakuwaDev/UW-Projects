package utils;

import java.util.HashSet;
import java.util.List;
import java.util.Random;

// Pomocnicza klasa do wykonywania operacji losowych.
public class Losowanie {
    private final static Random rand = new Random();

    // Losuje liczbę z zakresu [dolna, gorna].
    public static int losuj(int dolna, int gorna) {
        return rand.nextInt(dolna, gorna + 1);
    }

    // Losuje wartość z danego zbioru.
    public static <T> T losujZeZbioru(HashSet<T> zbior) {
        List<T> lista = zbior.stream().toList();
        return lista.get(rand.nextInt(lista.size()));
    }

    // Losuje wartość enumeratora.
    public static <T extends Enum<T>> T losujZEnuma(Class<T> e) {
        T[] wartosci = e.getEnumConstants();
        return wartosci[rand.nextInt(wartosci.length)];
    }
}
