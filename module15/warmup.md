# Module 15 Warm-Up: Lab 1 Code Review

Name: ______________________________  Partner: ______________________________

## The Setup

- `Game` is the **base class** of `PinochleGame` and `HoldemGame`
- `Game` owns `std::vector<std::string> player_names`
- Each derived class owns its **own** `deck` and `std::vector<...> hands`
- `HoldemGame` also has a `board` and a `state`
- Each derived class overrides `int play()`

## The Code

```c++
int PinochleGame::play(){
    std::string response;
    do{
        deck.shuffle();
        deal();
        for (size_t i = 0; i < hands.size(); ++i){
            std::cout << player_names[i]<< ": ";
            hands[i].print(std::cout, 100);
        }

        for (auto &hand: hands){
            deck.collect(hand);
        }

        std::cout << "Do you want to end the game? ";
        std::cin >> response;
    } while (response != "yes");

    return EXIT_SUCCESS;
}
```

```c++
int HoldemGame::play(){
    std::string response;
    do {
        deck.shuffle();
        state = HoldemState::preflop;
        deal();
        for (size_t i = 0; i < hands.size(); ++i) {
            std::cout << player_names[i] << ": ";
            hands[i].print(std::cout, 100);
        }
        deal();
        std::cout << "Flop: ";
        board.print(std::cout, 100);

        deal();
        std::cout << "Turn: ";
        board.print(std::cout, 100);

        deal();
        std::cout<< "River: ";
        board.print(std::cout, 100);

        for (auto &hand: hands){
            deck.collect(hand);
        }

        deck.collect(board);

        std::cout << "Do you want to end the game?";
        std::cin >> response;
    }while (response != "yes");

    return EXIT_SUCCESS;
}
```

## Your Task (5 minutes, in pairs)

This code compiles and runs. Review it as if it were submitted for code review: identify as many problems as you can, and propose a fix for each one.

