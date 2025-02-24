package elementySymulacji.systemTransakcyjny;

import elementySymulacji.zlecenia.ZlecenieWA;
import wyjatki.IllegalStockId;
import wyjatki.NotEnoughFunds;
import elementySymulacji.inwestorzy.Inwestor;
import utils.Pair;
import elementySymulacji.zlecenia.TypZlecenia;
import elementySymulacji.zlecenia.Zlecenie;
import elementySymulacji.zlecenia.ZlecenieDOT;

import java.util.HashSet;
import java.util.HashMap;
import java.util.TreeSet;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.Comparator;
import java.util.Collections;

public class SystemTransakcyjnyGPW implements SystemTransakcyjny {
    private final HashSet<String> akcjeWObrocie;
    private final HashMap<String, Integer> ostatniaTura;
    private final HashMap<String, Integer> ostatniaCena;
    private final HashMap<String, TreeSet<Zlecenie>> zleceniaKupna;
    private final HashMap<String, TreeSet<Zlecenie>> zleceniaSprzedazy;
    private final ArrayList<Inwestor> inwestorzy;
    private final HashMap<String, ArrayList<Transakcja>> transakcje;
    // Komparator zleceń w kolejności: najmniejsza cena, tura, idZlecenia.
    private final Comparator<Zlecenie> cmpKupno = Comparator.comparing(Zlecenie::getLimitCeny)
            .thenComparing(Zlecenie::getTura).thenComparing(Zlecenie::getIdZlecenia);
    //Komparator zleceń w kolejności: największa cena, najmniejsza tura, idZlecenia.
    private final Comparator<Zlecenie> cmpSprzedaz = Comparator.comparing(Zlecenie::getLimitCeny).reversed()
            .thenComparing(Zlecenie::getTura).thenComparing(Zlecenie::getIdZlecenia);

    private int tura;
    private int aktualneId;

    public SystemTransakcyjnyGPW(ArrayList<Inwestor> inwestorzy) {
        tura = 0;
        aktualneId = 0;
        this.inwestorzy = inwestorzy;
        akcjeWObrocie = new HashSet<>();
        ostatniaTura = new HashMap<>();
        ostatniaCena = new HashMap<>();
        zleceniaKupna = new HashMap<>();
        zleceniaSprzedazy = new HashMap<>();
        transakcje = new HashMap<>();
    }

    public void dodajAkcje(String idAkcji, int cena) {
        // Sprawdzanie poprawności akcji.
        if(idAkcji.length() > 5) throw new IllegalStockId("Identyfikator akcji przekracza dozwoloną długość.");
        if(idAkcji.isEmpty()) throw new IllegalStockId("Identyfikator akcji jest pusty.");
        for(int i = 0; i < idAkcji.length(); ++i)
            if(idAkcji.charAt(i) < 65 || idAkcji.charAt(i) > 90)
                throw new IllegalStockId("Identyfikator akcji zawiera znak spoza A-Z");
        if(akcjeWObrocie.contains(idAkcji)) throw new IllegalStockId("Identyfikator akcji już należy do systemu.");

        // Ustawianie początkowych wartości dla danej akcji.
        akcjeWObrocie.add(idAkcji);
        ostatniaTura.put(idAkcji, -1);
        ostatniaCena.put(idAkcji, cena);
        zleceniaKupna.put(idAkcji, new TreeSet<>(cmpKupno));
        zleceniaSprzedazy.put(idAkcji, new TreeSet<>(cmpSprzedaz));
        transakcje.put(idAkcji, new ArrayList<>());
    }

    // Realizuje transakcję pomiędzy dwoma kompatybilnymi zleceniami.
    // Zwraca:
    //      3, jeśli w wyniku transakcji oba zlecenia powinny zostać usunięte z sytemu.
    //      2, jeśli w wyniku transakcji zlecenie kupna powinno zostać usunięte z systemu.
    //      1, jeśli w wyniku transakcji zlecenie sprzedaży powinno zostać usunięte z sytemu.
    //      0, jeśli żadne zlecenie nie powinno zostać usunięte z systemu.
    private int dodajTransakcje(Zlecenie kupno, Zlecenie sprzedaz) {
        String idAkcji = kupno.getIdAkcji();
        Inwestor kupujacy = kupno.getInwestor();
        Inwestor sprzedajacy = sprzedaz.getInwestor();
        int liczbaAkcji = Math.min(kupno.getLiczbaSztuk(), sprzedaz.getLiczbaSztuk());
        int cena = Comparator.comparing(Zlecenie::getTura).thenComparing(Zlecenie::getIdZlecenia)
                .compare(kupno, sprzedaz) < 0 ? kupno.getLimitCeny() : sprzedaz.getLimitCeny();

        // Sprawdzanie możliwych braków funduszy.
        if(kupujacy.getGotowka() < cena * liczbaAkcji && sprzedajacy.getAkcje().get(idAkcji) < liczbaAkcji) return 3;
        else if(kupujacy.getGotowka() < cena * liczbaAkcji) return 2;
        else if(sprzedajacy.getAkcje().get(idAkcji) < liczbaAkcji) return 1;

        // Tworzenie, realizacja i zapis odpowiedniej transakcji.
        Transakcja transakcja = new Transakcja(sprzedajacy, sprzedaz, kupujacy, kupno, liczbaAkcji, idAkcji, cena);
        transakcja.zrealizuj();
        transakcje.get(idAkcji).add(transakcja);

        // Sprawdzanie czy któreś ze zleceń zostało zrealizowane.
        if(kupno.getLiczbaSztuk() == 0 && sprzedaz.getLiczbaSztuk() == 0) return 3;
        else if(kupno.getLiczbaSztuk() == 0) return 2;
        else if(sprzedaz.getLiczbaSztuk() == 0) return 1;
        else return 0;
    }

    // Symuluje transakcje dla danego zbioru zleceń pojedynczej akcji.
    // Jeśli symulacja się powiedzie (wszystkie zlecenia wykonaj lub anuluj zostaną w pełni zrealizowane),
    // zwraca null, a jeśli jakieś nie zostanie zrealizowane, zwraca wskaźnik na to zlecenie.
    private Zlecenie symulujTransakcje(String idAkcji,
                                       TreeSet<Zlecenie> zleceniaSprzedazyAkcji, TreeSet<Zlecenie> zleceniaKupnaAkcji) {

        // Dopóki istnieją zlecenia obu typów.
        while (!zleceniaSprzedazyAkcji.isEmpty() && !zleceniaKupnaAkcji.isEmpty()) {
            Zlecenie kupno = zleceniaKupnaAkcji.first();
            Zlecenie sprzedaz = zleceniaSprzedazyAkcji.first();

            // Jeśli najlepsze zlecenia są niekompatybilne, to inne też nie będą.
            if(kupno.getLimitCeny() > sprzedaz.getLimitCeny()) break;

            // Dodawanie transakcji pomiędzy zleceniami i odpowiednie usuwanie zlecen z systemu.
            int wyn = dodajTransakcje(kupno, sprzedaz);
            switch (wyn) {
                case 1 -> zleceniaSprzedazyAkcji.pollFirst();
                case 2 -> zleceniaKupnaAkcji.pollFirst();
                case 3 -> {
                    zleceniaKupnaAkcji.pollFirst();
                    zleceniaSprzedazyAkcji.pollFirst();
                }
            }
        }

        // Sprawdzanie czy jakieś zlecenie typu wykonaj lub anuluj nie zostało zrealizowane,
        // jeśli takie istnieje, zwracamy wskaźnik na nie.
        for(Zlecenie zlecenie : zleceniaKupna.get(idAkcji))
            if(zlecenie instanceof ZlecenieWA && zlecenie.getLiczbaSztuk() != 0) {
                for(Transakcja transakcja : transakcje.get(idAkcji))
                    transakcja.cofnij();

                transakcje.get(idAkcji).clear();
                return zlecenie;
            }

        for(Zlecenie zlecenie : zleceniaSprzedazy.get(idAkcji))
            if(zlecenie instanceof ZlecenieWA && zlecenie.getLiczbaSztuk() != 0) {
                for(Transakcja transakcja : transakcje.get(idAkcji))
                    transakcja.cofnij();

                transakcje.get(idAkcji).clear();
                return zlecenie;
            }

        // Jako, że w tym momencie już wiemy, że ta symulacja jest poprawna, ustawiamy wartości ostatnich transakcji.
        for(Transakcja transakcja : transakcje.get(idAkcji)) {
            ostatniaCena.put(transakcja.getIdAkcji(), transakcja.getCena());
            ostatniaTura.put(transakcja.getIdAkcji(), tura);
        }

        // Usuwanie niezrealizowanych zleceń kupna z terminem.
        Iterator<Zlecenie> iterator = zleceniaKupna.get(idAkcji).iterator();
        while(iterator.hasNext()) {
            Zlecenie z = iterator.next();
            if(z instanceof ZlecenieDOT && !((ZlecenieDOT) z).czyAktywne(tura)) {
                iterator.remove();
            }
        }

        // Usuwanie niezrealizowanych zleceń sprzedaży z terminem.
        iterator = zleceniaSprzedazy.get(idAkcji).iterator();
        while(iterator.hasNext()) {
            Zlecenie z = iterator.next();
            if(z instanceof ZlecenieDOT && ((ZlecenieDOT) z).getTerminWaznosci() == tura) {
                iterator.remove();
            }
        }

        // Symulacja się powiodła.
        return null;
    }

    @Override
    public int aktualnaTura() {
        return tura;
    }

    @Override
    public HashSet<String> dostepneAkcje() {
        return akcjeWObrocie;
    }

    @Override
    public Pair<Integer, Integer> ostatniaTransakcja(String idAkcji) {
        if(!akcjeWObrocie.contains(idAkcji)) throw new IllegalStockId("Akcja o podanym identyfikatorze nie istnieje.");
        return new Pair<>(ostatniaTura.get(idAkcji), ostatniaCena.get(idAkcji));
    }

    @Override
    public void zlozZlecenie(Zlecenie zlecenie) throws NotEnoughFunds {
        if(zlecenie == null) return;

        // Dodwanie zlecenia do odpowiedniej kolejki.
        Inwestor inwestor = zlecenie.getInwestor();
        if (zlecenie.getTypZlecenia() == TypZlecenia.sprzedaz) {
            if (inwestor.getAkcje().get(zlecenie.getIdAkcji()) < zlecenie.getLiczbaSztuk())
                throw new NotEnoughFunds("Inwestor nie ma takich akcji.");
            TreeSet<Zlecenie> kolejka = zleceniaSprzedazy.get(zlecenie.getIdAkcji());
            zlecenie.setIdZlecenia(aktualneId++);
            kolejka.add(zlecenie);
        }
        else {
            if (inwestor.getGotowka() < zlecenie.getLimitCeny() * zlecenie.getLiczbaSztuk())
                throw new NotEnoughFunds("Inwestor nie ma tyle pieniedzy.");
            TreeSet<Zlecenie> kolejka = zleceniaKupna.get(zlecenie.getIdAkcji());
            zlecenie.setIdZlecenia(aktualneId++);
            kolejka.add(zlecenie);
        }
    }

    @Override
    public void przetworzTure() {
        // Losowanie kolejności inwestorów i odpytywanie ich o zlecenia.
        Collections.shuffle(inwestorzy);
        for(Inwestor inwestor : inwestorzy)
            inwestor.zlozZlecenie(this);

        // Używane przy pzywracaniu stanu giełdy w razie symulacji, która się nie powiedzie.
         TreeSet<Zlecenie> zleceniaKupnaPrzedTura;
         TreeSet<Zlecenie> zleceniaSprzedazyPrzedTura;

        // Dla każdej akcji oddzielnie.
        for(String idAkcji : akcjeWObrocie) {
            transakcje.get(idAkcji).clear();

            // Skończymy gdy usuniemy tyle zleceń wykonaj lub anuluj, aby symulacja zakończyła się sukcesem.
            while(true) {
                // Zapisujemy stan sprzed symulacji.
                zleceniaKupnaPrzedTura = zleceniaKupna.get(idAkcji);
                zleceniaSprzedazyPrzedTura = zleceniaSprzedazy.get(idAkcji);

                // Przeprowadzamy symulację.
                Zlecenie doUsuniecia = symulujTransakcje(idAkcji,
                        zleceniaSprzedazy.get(idAkcji), zleceniaKupna.get(idAkcji));

                if(doUsuniecia != null) {   // Jeśli symulacja się nie powiodła.
                    // Resetujemy stan giełdy sprzed symulacji.
                    zleceniaSprzedazy.put(idAkcji, zleceniaSprzedazyPrzedTura);
                    zleceniaKupna.put(idAkcji, zleceniaKupnaPrzedTura);

                    // Usuwamy akcję wykonaj lub anuluj, która nie została zrealizowana.
                    zleceniaSprzedazy.get(idAkcji).remove(doUsuniecia);
                    zleceniaKupna.get(idAkcji).remove(doUsuniecia);
                }
                else break;     // Jesli symulacja się powiodła, wychodzimy z pętli.
            }
         }

        // Aktualizowanie wartości.
        aktualneId = 0;
        ++tura;
    }
}
