package elementySymulacji.systemTransakcyjny;

import elementySymulacji.inwestorzy.Inwestor;
import elementySymulacji.zlecenia.Zlecenie;

public class Transakcja {
    private final Inwestor sprzedawca;
    private final Zlecenie ofertaSprzedazy;
    private final Inwestor nabywca;
    private final Zlecenie ofertKupna;
    private final int liczbaSztuk;
    private final String idAkcji;
    private final int cena;

    public Transakcja(Inwestor sprzedawca, Zlecenie ofertaSprzedazy, Inwestor nabywca,
                      Zlecenie ofertKupna, int liczbaSztuk, String idAkcji, int cena) {
        this.sprzedawca = sprzedawca;
        this.ofertaSprzedazy = ofertaSprzedazy;
        this.nabywca = nabywca;
        this.ofertKupna = ofertKupna;
        this.liczbaSztuk = liczbaSztuk;
        this.idAkcji = idAkcji;
        this.cena = cena;
    }

    // Zaktualizuj wartości portfeli uczestników oraz ich zgłoszeń.
    public void zrealizuj() {
        nabywca.setGotowka(nabywca.getGotowka() - cena * liczbaSztuk);
        nabywca.dodajAkcje(idAkcji, liczbaSztuk);

        sprzedawca.setGotowka(sprzedawca.getGotowka() + cena * liczbaSztuk);
        sprzedawca.zabierzAkcje(idAkcji, liczbaSztuk);

        ofertKupna.setLiczbaSztuk(ofertKupna.getLiczbaSztuk() - liczbaSztuk);
        ofertaSprzedazy.setLiczbaSztuk(ofertaSprzedazy.getLiczbaSztuk() - liczbaSztuk);
    }

    // Cofnij efekt tej transakcji.
    public void cofnij() {
        nabywca.setGotowka(nabywca.getGotowka() + cena * liczbaSztuk);
        nabywca.zabierzAkcje(idAkcji, liczbaSztuk);

        sprzedawca.setGotowka(sprzedawca.getGotowka() - cena * liczbaSztuk);
        sprzedawca.dodajAkcje(idAkcji, liczbaSztuk);

        ofertKupna.setLiczbaSztuk(ofertKupna.getLiczbaSztuk() + liczbaSztuk);
        ofertaSprzedazy.setLiczbaSztuk(ofertaSprzedazy.getLiczbaSztuk() + liczbaSztuk);
    }

    public String getIdAkcji() {
        return idAkcji;
    }

    public int getCena() {
        return cena;
    }
}
