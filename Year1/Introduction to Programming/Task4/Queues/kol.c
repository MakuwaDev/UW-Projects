// Author: Krzysztof Hałubek
// Reviewer: Adam Sołtan
#include "kol.h"

#include <cstdlib>

namespace {

// Obowiązuje niezmiennik: head i tail łączą się z listą poprzez l2
// natomiast l1 wskazuje w obu przypadkach na nullptr
struct list {
  interesant *head, *tail;
};

list create_list() {
  list res = {(interesant *)std::malloc(sizeof(interesant)),
              (interesant *)std::malloc(sizeof(interesant))};
  res.head->l1 = nullptr;
  res.head->l2 = res.tail;
  res.tail->l1 = nullptr;
  res.tail->l2 = res.head;
  return res;
}

bool is_empty(const list &l) { 
  return l.head->l2 == l.tail; 
}

interesant *front(const list &l) { 
  return l.head->l2; 
}

// Podpina nw do el, tam gdzie wcześniej byl old
void link(interesant *el, interesant *old, interesant *nw) {
  if (el) {
    if (el->l1 == old)
      el->l1 = nw;
    else
      el->l2 = nw;
  }
}

void remove_elem(interesant *el) {
  link(el->l1, el, el->l2);
  link(el->l2, el, el->l1);
}

void pop_front(list &l) {
  if (!is_empty(l)) remove_elem(l.head->l2);
}

void push_back(list &l, interesant *el) {
  el->l1 = l.tail;
  el->l2 = l.tail->l2;
  link(l.tail->l2, l.tail, el);
  l.tail->l2 = el;
}

// Zwraca wskaźnik na sąsiada el niebędacego prev
interesant *next(interesant *el, interesant *prev) {
  if (el->l1 != prev) return el->l1;
  return el->l2;
}

void reverse(list &l) {
  std::swap(l.head, l.tail); 
}

// Przepina listę na koniec innej listy, "przepięta" lista jest potem
// zdatna do użycia i niezależna od listy, do której została "przepięta"
void append(list &list1, list &list2) {
  interesant *b = list1.tail;
  interesant *f = list2.head;
  link(b->l2, b, f->l2);
  link(f->l2, f, b->l2);
  b->l2 = f;
  f->l2 = b;
  list1.tail = list2.tail;
  list2.head = f;
  list2.tail = b;
}

// Usuwa listę i dealokuje pamięć po head oraz tail
// Zwraca wektor wskaźników na interesantów, będących elementami tej listy
std::vector<interesant *> delete_list(list &l) {
  std::vector<interesant *> res;
  while (!is_empty(l)) {
    res.push_back(front(l));
    pop_front(l);
  }
  std::free(l.head);
  std::free(l.tail);
  return res;
}

std::vector<list> kolejki;

int akt_numerek;  // numerek następnego klienta, który wejdzie do urzędu

}  // namespace

void otwarcie_urzedu(int m) {
  kolejki.resize(m);
  for (int i = 0; i < m; ++i) kolejki[i] = create_list();
  akt_numerek = 0;
}

interesant *nowy_interesant(int k) {
  interesant *el = (interesant *)std::malloc(sizeof(interesant));
  el->numer = akt_numerek++;
  push_back(kolejki[k], el);
  return el;
}

int numerek(interesant *i) {
  return i->numer;
}

interesant *obsluz(int k) {
  if (is_empty(kolejki[k])) return nullptr;
  interesant *i = front(kolejki[k]);
  pop_front(kolejki[k]);
  return i;
}

void zmiana_okienka(interesant *i, int k) {
  remove_elem(i);
  push_back(kolejki[k], i);
}

void zamkniecie_okienka(int k1, int k2) {
  append(kolejki[k2], kolejki[k1]);
}

std::vector<interesant *> fast_track(interesant *i1, interesant *i2) {
  if (i1 == i2) {
    remove_elem(i1);
    return {i1};
  }

  std::vector<interesant *> left = {i1, i1->l1};
  std::vector<interesant *> right = {i1, i1->l2};

  // w tej pętli możemy dodać do któregoś z wektorów head albo tail,
  // ale w takim razie będziemy usuwać ten drugi, więc nie ma problemu
  while (left.back() != i2 && right.back() != i2) {
    interesant *next_left = next(left.back(), left[left.size() - 2]);
    interesant *next_right = next(right.back(), right[right.size() - 2]);
    if (next_left) left.push_back(next_left);
    if (next_right) right.push_back(next_right);
  }

  if (left.back() == i2) std::swap(left, right);  // i2 był po "lewej" stronie i1
  for (interesant *x : right) 
    remove_elem(x);
  return right;
}

void naczelnik(int k) {
  reverse(kolejki[k]);
}

std::vector<interesant *> zamkniecie_urzedu() {
  std::vector<interesant *> res;
  for (list l : kolejki)
    for (interesant *i : delete_list(l)) 
      res.push_back(i);
  return res;
}
