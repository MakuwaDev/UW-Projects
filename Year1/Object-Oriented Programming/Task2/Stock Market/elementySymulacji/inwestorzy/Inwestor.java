package elementySymulacji.inwestorzy;

import elementySymulacji.systemTransakcyjny.SystemTransakcyjny;

import java.util.HashMap;

public abstract class Inwestor {
    protected int gotowka;
    protected HashMap<String, Integer> portfelAkcji;

    public int getGotowka() {
        return gotowka;
    }

    public HashMap<String, Integer> getAkcje() {
        return portfelAkcji;
    }

    public void setGotowka(int gotowka) {
        this.gotowka = gotowka;
    }

    public void setPortfelAkcji(HashMap<String, Integer> portfelAkcji) {
        this.portfelAkcji = portfelAkcji;
    }

    public void dodajAkcje(String idAkcji, int ilosc) {
        portfelAkcji.compute(idAkcji, (k, v) -> v == null ? ilosc : v + ilosc);
    }

    public void zabierzAkcje(String idAkcji, int ilosc) {
        portfelAkcji.compute(idAkcji, (k, v) -> v == null ? ilosc : v - ilosc);
    }

    public abstract void zlozZlecenie(SystemTransakcyjny systemTransakcyjny);
}
