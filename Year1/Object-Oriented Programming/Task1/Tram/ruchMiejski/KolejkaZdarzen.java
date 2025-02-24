package ruchMiejski;

public interface KolejkaZdarzen {

    // Dodaje zdarzenie na kolejkę.
    void dodaj(Zdarzenie zdarzenie);

    // Zdejmuje pierwsze w kolejności zdarzenie z kolejki.
    Zdarzenie wezPierwsze();

    // Sprawdza czy kolejka jest pusta.
    boolean czyPusta();
}
