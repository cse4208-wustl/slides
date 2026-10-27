# Module 15 Warm-Up: Answers

## Problems Found

| # | Problem | Fix |
|---|---|---|
| 1 | The game loop (shuffle → deal → print → collect → ask) is **duplicated** in both `play()`s, and the copies have already drifted (one prompt has a trailing space, one doesn't) | Write the loop **once** in `Game::play()`; derived classes override only what differs (`play_round`, `collect_all`). This is the *Template Method* pattern |
| 2 | If `std::cin` fails (Ctrl-D, input from an empty file), `response` never becomes `"yes"`, so the game shuffles, deals and prints **forever** | Test the stream: `if (!(std::cin >> response))` and stop |
| 3 | `play()` **always** returns `EXIT_SUCCESS`, so the return value tells the caller nothing | Return success when the user ends the game and failure when input breaks |
| 4 | `player_names[i]` is indexed by `hands.size()`; the names live in `Game` and the hands in the derived class, so nothing guarantees the sizes match → possible **out-of-bounds** read | One shared `print_hands` that checks **both** bounds (better still: keep each player's name and hand together) |
| 5 | Readability: magic number `100` (5 times); `deal()` called 4 times with behavior hidden in `state`; flop/turn/river are 3 copies of the same lines; inconsistent formatting | Named constant `line_width`; a loop over the stage labels with a comment on what each `deal()` does |

## Redesigned Code

The type names (`PinochleDeck`, `HoldemDeck`, `CardSet<Rank, Suit>`, `HoldemState`) follow the Lab 1 spec; constructors and `deal()` are unchanged from a typical solution and are omitted.

### `Game.h`

```c++
class Game {
public:
    Game(int argc, const char *argv[]);
    virtual ~Game() = default;
    int play();                                   // the game loop, written once

protected:
    std::vector<std::string> player_names;
    static constexpr std::size_t line_width = 100;

    virtual void play_round() = 0;                // shuffle, deal, print
    virtual void collect_all() = 0;               // return all cards to the deck

    template <typename Hand>
    void print_hands(std::vector<Hand> &hands) const;

private:
    enum class Answer { keep_playing, end_game, input_failed };
    Answer ask_to_end() const;
};

template <typename Hand>
void Game::print_hands(std::vector<Hand> &hands) const {
    for (std::size_t i = 0; i < hands.size() && i < player_names.size(); ++i) {
        std::cout << player_names[i] << ": ";
        hands[i].print(std::cout, line_width);
    }
}
```

### `Game.cpp`

```c++
int Game::play() {
    Answer answer;
    do {
        play_round();
        collect_all();
        answer = ask_to_end();
    } while (answer == Answer::keep_playing);
    return answer == Answer::end_game ? EXIT_SUCCESS : EXIT_FAILURE;
}

Game::Answer Game::ask_to_end() const {
    std::string response;
    std::cout << "Do you want to end the game? ";
    if (!(std::cin >> response)) {                // EOF or bad stream: stop
        return Answer::input_failed;
    }
    return response == "yes" ? Answer::end_game : Answer::keep_playing;
}
```

### `PinochleGame.h`

```c++
class PinochleGame : public Game {
public:
    PinochleGame(int argc, const char *argv[]);

protected:
    PinochleDeck deck;
    std::vector<CardSet<PinochleRank, Suit>> hands;

    void deal();
    void play_round() override;
    void collect_all() override;
};
```

### `PinochleGame.cpp`

```c++
void PinochleGame::play_round() {
    deck.shuffle();
    deal();
    print_hands(hands);
}

void PinochleGame::collect_all() {
    for (auto &hand : hands) {
        deck.collect(hand);
    }
}
```

### `HoldemGame.h`

```c++
class HoldemGame : public Game {
public:
    HoldemGame(int argc, const char *argv[]);

protected:
    HoldemState state;
    HoldemDeck deck;
    std::vector<CardSet<HoldemRank, Suit>> hands;
    CardSet<HoldemRank, Suit> board;

    void deal();                                  // deals for the current state, then advances it
    void play_round() override;
    void collect_all() override;
};
```

### `HoldemGame.cpp`

```c++
void HoldemGame::play_round() {
    deck.shuffle();
    state = HoldemState::preflop;
    deal();                                       // two hole cards per player
    print_hands(hands);

    for (const char *stage : {"Flop: ", "Turn: ", "River: "}) {
        deal();                                   // 3, 1, then 1 card to the board
        std::cout << stage;
        board.print(std::cout, line_width);
    }
}

void HoldemGame::collect_all() {
    for (auto &hand : hands) {
        deck.collect(hand);
    }
    deck.collect(board);
}
```

## What Changed

- `PinochleGame` and `HoldemGame` no longer define `play()` at all; `Game::play()` runs the loop for every game
- Adding a third game means writing only `play_round()` and `collect_all()`
- `print_hands` is a **member template**: Pinochle and Hold'em hands are different types, but one function prints both
- The prompt, the input check and the return value each exist in exactly **one** place
