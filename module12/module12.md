# Types and Generic Programming
---

## Self-Assessment Questions

---
### Q1: Top-Level vs. Low-Level `const`

```c++
int i = 0;
int *const p1 = &i;
const int ci = 42;
const int *p2 = &ci;
const int *const p3 = p2;
const int &r = ci;

++p1;           // (A)
++*p1;          // (B)
++*p2;          // (C)
p2 = p3;        // (D)
int *p4 = p3;   // (E)
int j = ci;     // (F)
int &r2 = ci;   // (G)
```

***Question***: Label every `const` above as top-level or low-level. Which of (A)–(G) compile?

---
### Q1 Answer

```c++
int i = 0;                  // no const
int *const p1 = &i;         // top-level: p1 itself can't change
const int ci = 42;          // top-level: ci itself can't change
const int *p2 = &ci;        // low-level: what p2 points to can't change
const int *const p3 = p2;   // left const: low-level; right const: top-level
const int &r = ci;          // low-level: const in a reference always is

++p1;           // (A) p1's top-level const            -> error
++*p1;          // (B) p1 has no low-level const        -> ok
++*p2;          // (C) p2's low-level const             -> error
p2 = p3;        // (D) p3's top-level const ignored     -> ok
int *p4 = p3;   // (E) would drop p3's low-level const  -> error
int j = ci;     // (F) ci's top-level const ignored     -> ok
int &r2 = ci;   // (G) would drop ci's const via r2     -> error
```

---
### Q1 Answer (cont.)
- **Top-level**: the object **itself** can't change (any type). **Low-level**: the object **pointed to / referred to** can't change (pointers and references only).
- Read right to left: `const int *const p3` → `p3` is a **const pointer** to an **int that is const**.

---
### Q1 Answer (cont.)
- When a value is **copied** (initialization, assignment, pass/return by value):
   - **Top-level `const` of the source is ignored** — the copy is a separate object:
      - `int j = ci;` OK: changing `j` can't change `ci`.
      - `p2 = p3;` OK: copies the address; `p3` itself isn't changed.
      - `void f(int x);  f(ci);` OK: `x` is a copy of `ci`.
   - **Low-level `const` must match (or be added)** — the copy points to the *same* object:
      - `const int *p5 = &i;` OK: adding `const` (we just promise less).
      - `int *p4 = p3;` Error: `p4` would point to `ci` without `const`.
- Binding a reference is not a copy, but the low-level rule applies: `int &r2 = ci;` Error.
- These rules explain everything about `auto` and `decltype` that follows.

---
### Q2: Type Aliases and `const`

```c++
typedef double wages;
typedef wages base, *p;      // ???
using SI = double;

typedef char *pstring;
char buf[] = "hello";
const pstring cstr = buf;    // what is the type of cstr?
*cstr = 'J';                 // (A)
cstr = nullptr;              // (B)

const char *s = buf;         // "same thing"?
*s = 'J';                    // (C)
s = nullptr;                 // (D)
```

***Question***: What are `base` and `p`? Which of (A)–(D) compile? Is `const pstring` the same as `const char *`?

---
### Q2 Answer

```c++
typedef double wages;
typedef wages base, *p;      // base: double;  p: double*
using SI = double;           // SI: double (alias declaration)

typedef char *pstring;
char buf[] = "hello";
const pstring cstr = buf;    // char *const: const applies to the POINTER
*cstr = 'J';                 // (A) ok: the chars are not const
cstr = nullptr;              // (B) error: cstr itself is const (top-level)

const char *s = buf;         // const char*: NOT the same type!
*s = 'J';                    // (C) error: the chars are const (low-level)
s = nullptr;                 // (D) ok: s itself is not const
```

---
### Q2 Answer (cont.)
- `typedef` follows declaration syntax: in `typedef wages base, *p;` the `*` belongs to the declarator `p`.
- `using` (C++11 **alias declaration**) puts the new name on the left, which is easier to read:
   - `typedef int (*Fn)(int);` vs. `using Fn = int (*)(int);`
- An alias is **not textual substitution**:
   - `const char *s`: the base type is `const char`; `*` is part of the declarator → **low-level** `const`.
   - `const pstring cstr`: the base type is already "pointer to `char`", and `const` modifies that whole type → **top-level** `const`.
- Want a pointer to `const char`? Say so in the alias: `using pcstring = const char *;`

---
### Q2 Answer (cont.): Why use aliases?
- Complex types are tedious, error-prone, and **hide intent**:
   - `vector<CardSet<PinochleRank, Suit>>::iterator` → `pinochle_hand_iterator`
- A good alias name says **what the type is for**, not just what it is (`wages`, not `dbl`).
- One place to change the type later (`using wages = long double;`).
- Only `using` can define an **alias template** (`typedef` can't be a template):

```c++
template <typename T> using Table = map<string, vector<T>>;
Table<int> scores;   // map<string, vector<int>>
```

---
### Q3: `auto` Type Deduction

```c++
int i = 0, &r = i;
const int ci = i, &cr = ci;

auto a = r;
auto b = ci;
auto c = cr;
auto d = &i;
auto e = &ci;
auto &g = ci;
auto &h = 42;             // (A)
const auto &j = 42;       // (B)
auto k = ci, &l = i;      // (C)
auto &n = i, *p2 = &ci;   // (D)

a = 42;  b = 42;  c = 42;  d = 42;  e = 42;  g = 42;   // (E)
```

***Question***: What type does each variable get? Which lines don't compile? Which assignments in (E) compile?

---
### Q3 Answer

```c++
int i = 0, &r = i;
const int ci = i, &cr = ci;

auto a = r;               // int: r is just an alias for i
auto b = ci;              // int: top-level const dropped
auto c = cr;              // int: const dropped (cr refers to ci)
auto d = &i;              // int*
auto e = &ci;             // const int*: low-level const kept
auto &g = ci;             // const int&: const kept for a reference
auto &h = 42;             // (A) error: plain reference to a literal
const auto &j = 42;       // (B) ok: const int&
auto k = ci, &l = i;      // (C) ok: k is int, l is int&
auto &n = i, *p2 = &ci;   // (D) error: deduces int, then const int

a = 42;  b = 42;  c = 42; // (E) ok: plain ints (copies; i unchanged)
d = 42;  e = 42;          // (E) error: int -> pointer
g = 42;                   // (E) error: g is a const int&
```

---
### Q3 Answer (cont.)
- The compiler deduces the type **from the initializer**, but adjusts it to normal initialization rules — the result is **not always the initializer's type**:
   - **References are aliases**: the type of the referred-to object is used.
   - **Top-level `const` is dropped** (the variable is a copy).
   - **Low-level `const` is kept**.
- When you ask for a **reference** (`auto &`), the `const` is no longer top-level, so it's kept.
- Want a top-level `const`? Ask for it: `const auto f = ci;` → `const int`.

---
### Q3 Answer (cont.): Multiple Variables
- One declaration has **one base type**; `*` and `&` belong to each declarator.
- With `auto`, every initializer must deduce the **same base type**:
   - `auto &m = ci, *p = &ci;` **OK** — both deduce `const int`.
   - `auto sz = 0, pi = 3.14;` **Error** — `int` vs. `double`.
- Tip: to see a deduced type, provoke an error: `template <typename T> class TD;` (never defined), then `TD<decltype(e)> td;` → error mentions `TD<const int*>`.

---
### Q4: `decltype`

```c++
int f();                          // prints "f called"
int i = 42, *p = &i, &r = i;
const int ci = 0, &cj = ci;

decltype(f()) sum = 0;   // (A) is f called?
decltype(ci) x = 0;      // (B)
decltype(cj) y = x;      // (C)
decltype(cj) z;          // (D)
decltype(r + 0) b;       // (E)
decltype(*p) c;          // (F)
decltype((i)) d;         // (G)
decltype(i) e;           // (H)
```

***Question***: What type does each variable get, and which lines don't compile? How does this differ from `auto`?

---
### Q4 Answer

```c++
int f();                          // prints "f called"
int i = 42, *p = &i, &r = i;
const int ci = 0, &cj = ci;

decltype(f()) sum = 0;   // (A) int -- f is NOT called
decltype(ci) x = 0;      // (B) const int
decltype(cj) y = x;      // (C) const int&
decltype(cj) z;          // (D) error: const int& must be initialized
decltype(r + 0) b;       // (E) int: r + 0 is a value
decltype(*p) c;          // (F) error: int& must be initialized
decltype((i)) d;         // (G) error: int& must be initialized
decltype(i) e;           // (H) int
```

---
### Q4 Answer (cont.)
- `decltype(expr)` gives the **type of an expression without evaluating it**.
- Applied to a **variable** (no parentheses): the variable's **declared type**, including top-level `const` and references.
   - `decltype` is the **only** context where a reference isn't treated as a synonym for its object.
- Applied to an **expression**: a reference if the expression is an **lvalue** (something that could be on the left of `=`):
   - `*p` and `(i)` are lvalues → `int&`; `r + 0` is a value → `int`.
- `decltype((variable))` is **always** a reference; `decltype(variable)` only if the variable is one.

---
### Q4 Answer (cont.): `auto` vs. `decltype`

| | `auto x = ci;` | `decltype(ci) x = …;` |
|---|---|---|
| type comes from | the initializer | any expression (not evaluated) |
| top-level `const` | dropped | **kept** |
| reference | dropped | **kept** |
| needs an initializer | yes | only if the result is a reference/`const` |

- Where it shines: generic code. The return type is exactly what `*it` yields:
   - `template <typename It> auto first_of(It it) -> decltype(*it);`
   - For `vector<int>::iterator`, that is `int&`.

---
### Q5: When (Not) to Use `auto`

```c++
vector<string> names = {"ada", "alan"};
for (auto s : names) s += "!";                          // (A)

auto total = 0;
for (double x : {1.5, 2.5}) total += x;                 // (B)

auto n = names.size();
for (auto k = 0; k < n; ++k) { /* ... */ }              // (C)

map<string, vector<int>> m;
auto it = m.find("ada");                                // (D)
auto twice = [](int v) { return v * 2; };              // (E)
auto price = get_price();                               // (F)
```

***Question***: Which uses of `auto` are helpful, and which hide a bug or obscure the code?

---
### Q5 Answer

```c++
vector<string> names = {"ada", "alan"};
for (auto s : names) s += "!";              // (A) bug: s is a copy; names unchanged

auto total = 0;
for (double x : {1.5, 2.5}) total += x;     // (B) bug: total is int -> prints 3

auto n = names.size();
for (auto k = 0; k < n; ++k) { /* ... */ }  // (C) warning: int k vs. unsigned n

map<string, vector<int>> m;
auto it = m.find("ada");                    // (D) good: long iterator type
auto twice = [](int v) { return v * 2; };   // (E) good: only auto can name it
auto price = get_price();                   // (F) questionable: which type?
```

---
### Q5 Answer (cont.)
- (A) Use `auto &s` to modify the elements, `const auto &s` to read them without copying.
- (B) Write `0.0` or `double total`. No warning with `-Wall`; only `-Wconversion` catches it.
- (C) `size()` returns `size_t` (unsigned). Use `decltype(n) k = 0`.
- (D) The real type is `map<string, vector<int>>::iterator`; nothing is gained by spelling it.
- (E) A lambda's type has no name you could write.
- (F) Is `price` a `double`? `int` cents? A `Money` class? When the type **is the point**, `auto` hides it from the reader.

---
### Q5 Answer (cont.): Guidelines
- **Use `auto`** when the type is:
   - **obvious from the initializer**:
      - `auto sp = make_shared<Widget>();`
      - `auto x = static_cast<int>(y);`
   - **tedious and irrelevant**: iterators, nested template types
   - **unknowable**: lambdas, results of complicated template expressions:
      - `auto op = compose(sn, cs);`
- **Avoid `auto`** when:
   - the exact type matters to correctness (numeric precision, signedness, ownership)
   - it would **obscure key semantics** — e.g. long chains of template parameters where the reader needs to know what each step produces
   - you need `const` or `&` that `auto` would silently drop

---
### Q5 Answer (cont.): Guidelines
- Overusing `auto`: the code compiles, but the *reader* (and you, next month) must re-derive every type in their head.

---
### Q6: Run-Time Type Identification (RTTI)

```c++
struct Shape  { virtual ~Shape() = default; virtual double area() const = 0; };
struct Circle : Shape { /* ... */ };
struct Square : Shape { /* ... */ void rotate(); };   // rotate() not in Shape

Shape *ps = new Circle(1.0);
Square *sq = dynamic_cast<Square*>(ps);           // (A)
Square &rs = dynamic_cast<Square&>(*ps);          // (B)
cout << typeid(*ps).name() << endl;               // (C)
cout << typeid(ps).name() << endl;                // (D)

double area(const Shape &s) {                     // (E)
    if (typeid(s) == typeid(Circle)) { /* pi r^2 */ }
    else if (typeid(s) == typeid(Square)) { /* side^2 */ }
    return 0;
}
```

***Question***: What happens at (A)–(D)? What is wrong with (E)?

---
### Q6 Answer

```c++
struct Shape  { virtual ~Shape() = default; virtual double area() const = 0; };
struct Circle : Shape { /* ... */ };
struct Square : Shape { /* ... */ void rotate(); };   // rotate() not in Shape

Shape *ps = new Circle(1.0);
Square *sq = dynamic_cast<Square*>(ps);      // (A) nullptr: *ps is a Circle
Square &rs = dynamic_cast<Square&>(*ps);     // (B) throws std::bad_cast
cout << typeid(*ps).name() << endl;          // (C) Circle: dynamic type
cout << typeid(ps).name() << endl;           // (D) Shape*: static type

double area(const Shape &s) {                // (E) bad design: if-else over types;
    if (typeid(s) == typeid(Circle)) { /* pi r^2 */ }     // a new Shape silently
    else if (typeid(s) == typeid(Square)) { /* side^2 */ } // falls through to 0
    return 0;
}
```

---
### Q6 Answer (cont.)
- `dynamic_cast`: base → derived pointer/reference, checked **at run time**:
   - Pointer form fails with `nullptr`:
      - `if (Square *sq = dynamic_cast<Square*>(ps)) sq->rotate();`
   - Reference form throws (there is no null reference).
   - Base must be **polymorphic** (have a virtual function), else a compile error.
- `typeid` gives the type of an expression:
   - Polymorphic object → **dynamic** type (g++/clang print mangled `6Circle`).
   - Pointer or non-polymorphic type → **static** type.
   - Ignores top-level `const` and `&`: `typeid(cr) == typeid(int)`.

---
### Q6 Answer (cont.): RTTI is Usually a Sign of Bad Design
- If-else chains over types, e.g. `if (typeid(s) == typeid(Circle))`:
   - every new `Shape` needs a new branch **in every such function**.
- **Prefer virtual functions**: a missing `area()` is a **compile error**.
- Use `dynamic_cast` only for derived-only operations that **can't be virtual**.
- `static_cast`: conversions checked at compile time:
   - `static_cast<int>(Suit::hearts)`

---
## OOP vs. Generic Programming

- Both deal with **types not known when the code is written**.
- **OOP**: the type is resolved at **run time** — virtual functions, `dynamic_cast`, `typeid`.
- **Generic programming**: the type is resolved at **compile time** — templates, `auto`, `decltype`.

```c++
template <typename T>
void describe(const T &x) { cout << x.name() << " " << x.area() << endl; }

Circle c(1.0);  Square s(2.0);  Shape &rs = s;
describe(c);     // T = Circle  (one function generated per type)
describe(rs);    // T = Shape   (static type!) ... but name() is virtual -> "Square"
describe(42);    // compile-time error: int has no member name()
```

- Errors show up when the template is **instantiated** (compile time), not when the program runs.

---
## Types — Quick Reference

<style scoped>
table { font-size: 0.7em; }
table th, table td { padding: 3px 10px; }
table td:first-child, table td:first-child code { white-space: nowrap; }
</style>

| Syntax | Meaning |
|---|---|
| `int *const p` | top-level: `p` can't change |
| `const int *p` / `const int &r` | low-level: object read-only via `p` / `r` |
| `typedef T Name;` / `using Name = T;` | type alias |
| `template <class T> using A = X<T>;` | alias template (`using` only) |
| `auto x = e;` | `e`'s type minus `&` and top-level `const` |
| `const auto &x = e;` | keep `const`, bind by reference |
| `decltype(var)` | declared type, incl. `const` and `&` |
| `decltype((var))`, `decltype(*p)` | lvalue → reference type |
| `dynamic_cast<D*>(bp)` | `nullptr` on failure |
| `dynamic_cast<D&>(br)` | throws `std::bad_cast` on failure |
| `typeid(e)` | dynamic type if polymorphic object, else static |
