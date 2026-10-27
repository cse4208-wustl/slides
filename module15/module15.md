---
marp: true
theme: default
paginate: false
size: 16:9
style: |
  section {
    font-size: 28px;
  }
  h1 {
    color: #1a3c6e;
  }
  h2 {
    color: #1a3c6e;
  }
  table {
    font-size: 24px;
  }
---

# Parameter Packs and Variadic Templates

---

## Warm-Up: Lab 1 Code Review

![h:540](images/lab1_play.png)

---

## Problem 1: Duplicated Game Loop

- Both `play()`s repeat the **same skeleton**: shuffle → deal → print hands → collect → ask
- The "print each player's hand" loop is copied **verbatim**
- The "Do you want to end?" prompt is copied, and the copies have **already drifted**: one has a trailing space, one doesn't
- A third game (e.g., Gin Rummy) would mean a **third copy**
- `Game` holds `player_names` but does none of the work that uses them

**Fix:** put the loop in `Game` **once**; derived classes supply only what differs (the *Template Method* pattern).

---

## Problem 2: Input Failure Is an Infinite Loop

```c++
std::cin >> response;          // at EOF: fails, response unchanged
} while (response != "yes");   // still != "yes"  →  loop again
```

- If input ends (Ctrl-D, redirected file, closed pipe), `cin` enters a **fail state**
- Every later `>>` fails **immediately**, so the game shuffles, deals, and prints **forever**
- **Live check:** `./lab1 Pinochle a b c d < /dev/null`
- Also: `"Yes"`, `"y"`, `"YES"` are all treated as *keep playing*

**Fix:** test the stream (`if (!(std::cin >> response))`) and stop on failure.

---

## Problem 3: A Return Value That Says Nothing

- `play()` **always** returns `EXIT_SUCCESS`, so the caller learns nothing
- Process exit codes (`EXIT_SUCCESS`) leak into a **game** member function
- If the spec requires `int`, make it **meaningful**: success when the user says "yes", failure when input breaks

---

## Problem 4: Parallel Vectors Across the Hierarchy

```c++
std::cout << player_names[i] << ": ";   // names: in Game
hands[i].print(std::cout, 100);         // hands: in the derived class
```

- Indexing `player_names` by **`hands.size()`** assumes the sizes always match
- That invariant is split across **two classes**; nothing enforces it
- If they ever differ: **out-of-bounds read**, undefined behavior
- **Fix:** check both bounds, or (better) keep a player's name and hand **together**

---

## Problem 5: Readability

- **Magic number** `100` appears 5 times: what is it? Name it (`line_width`)
- **Hidden state machine:** `deal()` is called 4 times; each call does something different based on `state`. The reader must open `deal()` to understand `play()`
- **Copy-paste stages:** flop / turn / river are the same 3 lines with a different label
- Inconsistent formatting: `do{` vs. `do {`, `cout<<` vs. `cout <<`

---

## A Refactored `Game`

```c++
class Game {
public:
    virtual ~Game() = default;
    int play();                                 // the loop lives here, once
protected:
    std::vector<std::string> player_names;
    static constexpr std::size_t line_width = 100;
    virtual void play_round() = 0;              // shuffle, deal, print
    virtual void collect_all() = 0;             // return all cards to the deck
    template <typename Hand>
    void print_hands(std::vector<Hand> &hands) const;
private:
    enum class Answer { keep_playing, end_game, input_failed };
    Answer ask_to_end() const;
};
```

---

## The Loop, Written Once

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
    if (!(std::cin >> response)) return Answer::input_failed;  // EOF: stop
    return response == "yes" ? Answer::end_game : Answer::keep_playing;
}
```

---

## Shared Printing: A Member Template

```c++
template <typename Hand>
void Game::print_hands(std::vector<Hand> &hands) const {
    for (std::size_t i = 0; i < hands.size() && i < player_names.size(); ++i) {
        std::cout << player_names[i] << ": ";
        hands[i].print(std::cout, line_width);
    }
}
```

- Pinochle and Hold'em hands are **different types**
- A **member template** lets one function in `Game` print either
- Both bounds are checked

---

## Derived Classes: Only What Differs

```c++
void HoldemGame::play_round() {
    deck.shuffle();
    state = HoldemState::preflop;
    deal();                                       // hole cards
    print_hands(hands);
    for (const char *stage : {"Flop: ", "Turn: ", "River: "}) {
        deal();                                   // advances state
        std::cout << stage;
        board.print(std::cout, line_width);
    }
}

void HoldemGame::collect_all() {
    for (auto &hand : hands) deck.collect(hand);
    deck.collect(board);
}
```

`PinochleGame::play_round()` shrinks to 3 lines: `shuffle`, `deal`, `print_hands`.

---

## From Code Review to Today's Topic

- `print_hands` works for **any type** of hand: one template, many types
- What if we want one function that takes **any number** of arguments, each of **any type**?

```c++
print(std::cout, "Flop: ", 3, 'c', 2.5, player_names[0]);
```

That's a **variadic template**.

---

## Vocabulary

- **Variadic template:** a function or class template that takes a **varying number** of parameters
- **Parameter pack:** the varying parameters
  - **Template parameter pack:** zero or more **template** parameters
  - **Function parameter pack:** zero or more **function** parameters
- An **ellipsis** (`...`) marks a pack

```c++
// Args: template parameter pack     rest: function parameter pack
template <typename T, typename... Args>
void foo(const T &t, const Args &... rest);
```

---

## Why Not `initializer_list`?

`initializer_list<T>` (Section 6.2.6) already handles a **varying number** of arguments:

```c++
void error_msg(initializer_list<string> il) {
    for (const auto &msg : il) cout << msg << " ";
    cout << endl;
}

error_msg({"functionX", "expected", "foo", "actual", "bar"});  // 5 strings
error_msg({"functionY", "okay"});                              // 2 strings
error_msg({"functionX", "error code", 42});    // error: 42 is not a string
```

- Every element must have (or convert to) the **same type** `T`
- To print the `42`, you'd have to convert it to a `string` yourself

---

## Why Not `initializer_list`? (cont.)

| Tool | Number of args | Types of args |
|---|---|---|
| Overloads | fixed per overload | fixed per overload |
| `initializer_list<T>` | varies | **one** type `T` |
| **Variadic template** | **varies** | **vary** |

Use variadic templates when you know **neither** the number **nor** the types of the arguments.

---

## Self-Assessment Questions

---

### Q1: Deducing a Pack

```c++
template <typename T, typename... Args>
void foo(const T &t, const Args &... rest);

int i = 0; double d = 3.14; std::string s = "how now brown cow";

foo(i, s, 42, d);    // (A)
foo(s, 42, "hi");    // (B)
foo(d, s);           // (C)
foo("hi");           // (D)
```

***Question***: For each call, what are `T`, `Args`, `sizeof...(Args)` and `sizeof...(rest)`? What signature is instantiated?

---

### Q1 Answer

| Call | `T` | `Args` | `sizeof...` |
|---|---|---|---|
| (A) `foo(i, s, 42, d)` | `int` | `string, int, double` | 3 |
| (B) `foo(s, 42, "hi")` | `string` | `int, char[3]` | 2 |
| (C) `foo(d, s)` | `double` | `string` | 1 |
| (D) `foo("hi")` | `char[3]` | *(empty)* | 0 |

- `T` comes from the **first** argument; the rest fill the pack
- The compiler deduces the **types** *and* the **number** of pack elements
- `sizeof...(Args)` (types) always equals `sizeof...(rest)` (values) here

---

### Q1 Answer (cont.)

Four **different** functions are instantiated:

```c++
void foo(const int&, const string&, const int&, const double&);  // (A)
void foo(const string&, const int&, const char(&)[3]);           // (B)
void foo(const double&, const string&);                          // (C)
void foo(const char(&)[3]);                                      // (D)
```

- `"hi"` stays an **array** (`char[3]`): it binds to a reference, so no decay
- `sizeof...` is a **compile-time constant**; it does not evaluate its argument
- An **empty pack** is legal: (D) is fine

---

### Q2: Tracing a Recursive Variadic Function

```c++
template <typename T>                                         // (1)
ostream &print(ostream &os, const T &t) { return os << t; }

template <typename T, typename... Args>                       // (2)
ostream &print(ostream &os, const T &t, const Args &... rest) {
    os << t << ", ";
    return print(os, rest...);
}

print(cout, i, s, 42);
```

***Question***: Trace the recursion. Which version runs at each step, and why?

---

### Q2 Answer

| Call | `t` | `rest...` | Viable | Runs |
|---|---|---|---|---|
| `print(cout, i, s, 42)` | `i` | `s, 42` | (2) only | (2) |
| `print(cout, s, 42)` | `s` | `42` | (2) only | (2) |
| `print(cout, 42)` | `42` | *(empty)* | (1) **and** (2) | **(1)** |

- Each call **peels off** the first pack element into `t`
- Output: `0, how now brown cow, 42`

---

### Q2 Answer (cont.): Why (1) Wins the Last Call

- (2) is viable with an **empty pack**; (1) is viable too
- Both are **exact** matches, so it's a tie
- **Module 14 rule:** among tied templates, the **more specialized** wins
- A **non-variadic** template is more specialized than a variadic one, so **(1)** runs
- (1) is the **base case**: it stops the recursion and prints no trailing `", "`

---

### Q3: Declaration Order

Swap the order: define the **variadic** `print` (2) **first**, then the base case (1).

```c++
template <typename T, typename... Args>
ostream &print(ostream &os, const T &t, const Args &... rest) {
    os << t << ", ";
    return print(os, rest...);      // which print is visible here?
}
template <typename T>
ostream &print(ostream &os, const T &t) { return os << t; }

print(cout, 1, 2);
```

***Question***: What happens?

---

### Q3 Answer

- Inside (2), ordinary lookup sees **only (2)**; (1) isn't declared yet
- `print(cout, 2)` therefore matches (2) with an **empty pack**
- That instance calls `print(os)`: **no `T`** to bind → **compile error**

```
error: no matching function for call to 'print(std::ostream&)'
```

- The textbook says it "recurses indefinitely": the compiler keeps instantiating until it **can't** find a match, so in practice you get a **compile error**, not a run-time loop

---

### Q3 Answer (cont.): A Sneaky Exception

```c++
struct P { int x; };
ostream &operator<<(ostream &o, const P &p) { return o << p.x; }

print(cout, P{1}, P{2});    // compiles and works, even in the wrong order!
```

- **Argument-dependent lookup (ADL)** runs at **instantiation**, and `P` lives in the global namespace, so (1) is found
- With `int`s there is no namespace for ADL to search, so (1) is invisible
- **Lesson:** don't rely on luck. **Declare the base case before** the variadic version

---

### Q4: Pack Expansion Patterns

```c++
template <typename... Args>
ostream &errorMsg(ostream &os, const Args &... rest) {
    return print(os, debug_rep(rest)...);     // (X)
 // return print(os, debug_rep(rest...));     // (Y)
}

errorMsg(cerr, s, 42, d, "other");
```

***Question***: What does each version expand to? Which compiles? Also, what does `const Args &...` expand to?

---

### Q4 Answer

**Expanding** a pack = splitting it into elements and applying a **pattern** to each. The `...` goes to the **right** of the pattern.

| Code | Pattern | Expands to |
|---|---|---|
| `const Args &... rest` | `const Args &` | `const string&, const int&, ...` |
| `rest...` | `rest` | `s, 42, d, "other"` |
| (X) `debug_rep(rest)...` | `debug_rep(rest)` | `debug_rep(s), debug_rep(42), ...` |
| (Y) `debug_rep(rest...)` | `rest` | `debug_rep(s, 42, d, "other")` |

- (X) compiles: **one** call **per** element
- (Y) fails: **one** call with **four** arguments; `debug_rep` isn't variadic

---

### Q4 Answer (cont.)

- The pattern applies **separately to each element**
- Where you put `...` decides **what** gets repeated
- Patterns can be **types** (`const Args &`), or **expressions** (`debug_rep(rest)`, `sizeof(rest)`)

```c++
print(os, sizeof(rest)...);   // prints the byte size of each argument
```

---

### Q5: When a Type Can't Be Printed

```c++
struct Point { int x, y; };          // no operator<<

print(cout, 1, "two", Point{3, 4});
```

***Question***: Does this compile? If not, where does the error point, and how do you fix it?

---

### Q5 Answer

- **Compile error**, but only when the instantiation reaches `Point`:

```
error: no match for 'operator<<' (operand types are
       'std::ostream' and 'const Point')
```

- The error points **inside** `print` (`return os << t;`), several instantiations deep. Read the **"required from"** lines to find **your** call
- `print` places an **implicit requirement** on every argument type: it must support `<<`
- **Fix:** provide the operator

```c++
ostream &operator<<(ostream &os, const Point &p) {
    return os << "(" << p.x << ", " << p.y << ")";
}
```

---

## Surprise: You've Already Used One!

```c++
auto p = make_shared<Player>("Ann", 5, 100.0);   // Player(string, int, double)
auto s = make_shared<string>(10, 'c');           // "cccccccccc"
```

`make_shared` is a **variadic function template**:

```c++
template <typename T, typename... Args>
shared_ptr<T> make_shared(Args&&... args) {          // simplified
    return shared_ptr<T>(new T(std::forward<Args>(args)...));
}
```

- `T` is given **explicitly**; `Args` is **deduced** from the constructor arguments
- The pack is **expanded** into `T`'s constructor call, so **any** constructor works
- Same idea: `vector::emplace_back`, `make_unique`, `std::thread`

---

## Studio 15

- Declare and define variadic function templates **recursively**
- Use them to print arbitrary lists of arguments of arbitrary types
- Apply **patterns** to pack elements during expansion, e.g., to collect metadata like how many **bytes** each contains

**Predict** what each call does before compiling, then check the output.
