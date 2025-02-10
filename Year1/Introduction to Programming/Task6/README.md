# Guess the Number Game with Lies

## Task Description:
Bolek is trying to guess the number `x` that Ala has chosen. Bolek can ask questions like "Is x < y?", and Ala can answer "Yes" or "No". However, Ala is allowed to lie up to `k` times in a single game. The goal for Bolek is to guess the number `x` using as little questions as possible.

## Library Interface:
- **`void dajParametry(int &n, int &k, int &g);`**
  - Sets the parameters for the game:
    - `n` — the range of numbers (from 1 to `n`)
    - `k` — the maximum number of lies allowed in the game
    - `g` — the number of games to be played.

- **`bool mniejszaNiz(int y);`**
  - Asks the question: "Is x < y?". The answer might be false, but Ala can lie at most `k` times during the game.

- **`void odpowiedz(int x);`**
  - Provides the guess for the number `x` after playing the game.

## Objective:
Bolek must guess the number `x` using the minimal number of questions, considering the possibility of lies from Ala. The program should work for the following parameters:
- `1 ≤ n ≤ 12`
- `0 ≤ k ≤ 3`
- `1 ≤ g ≤ 10,000`
