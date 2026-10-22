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

# Overloading and Templates

---

## Overloading: A Quick Refresher

- Same name, different **number or types** of parameters
- Can be non-members, members, or operators
  - A member and a non-member `operator<` can both match one call
- The compiler picks the **best match** by ranking conversions (Section 6.4)
- No single best match: the call is **ambiguous** (usually an error)

---

## Templates Join the Overload Set

Templates can be overloaded by other templates **and** by ordinary functions.

- **Candidates:** every template whose argument deduction succeeds
- So template candidates are **always viable**
- All viable functions are **ranked by conversions**
- Templates allow few conversions (Section 16.2.1), so they usually need an **exact match**

---

## Breaking a Tie

One best match wins. If several match **equally well**:

1. Exactly **one non-template** among them: it wins
2. **All templates:** the **most specialized** wins
3. Otherwise: **ambiguous**

---

## Rule: Non-Templates Win Ties

`debug_rep(const T&)` vs. `debug_rep(const string&)`

- `debug_rep(s)` with a `string`: two **exact** matches, so the **non-template** wins
- Only for **ties**: if the template is exact and the non-template needs a **conversion**, the **template wins**

---

## Rule: More Specialized Templates Win Ties

`debug_rep(const T&)` vs. `debug_rep(T*)`

- `const T&` accepts **anything**; `T*` accepts **only pointers**, so it's **more specialized**
- With a `const string*` argument, both are exact, and `T*` wins
- Without this rule, pointer-to-`const` calls would be **ambiguous**

---

## Conversions Can Surprise You

`debug_rep("hi world!")` with three overloads:

| Overload | Match |
|---|---|
| `debug_rep(const T&)` with `T = char[10]` | exact |
| `debug_rep(T*)` with `T = const char` | exact (array → pointer) |
| `debug_rep(const string&)` | **user-defined** conversion |

- The templates tie, so **`T*` wins**; the `string` version never runs
- Fix: add **non-template** `char*` and `const char*` overloads

---

## Missing Declarations Can Misbehave

- Calling an undeclared function is normally a **compile error**
- But if a template with that name is visible, it's **silently instantiated** instead
- The program compiles and runs, but calls the **wrong function**

**Tip:** declare all overloads **before** defining any.

---

## Scope and Explicit Choices

- Only overloads **visible at the call** are candidates
- Namespaces and `::` select a specific overload
- `name<Type>(...)` skips the non-template and forces the **template**

---

## Predicting Which Overload Is Called

1. **Visible?** Non-templates, plus templates whose deduction succeeds
2. **Conversions?** Exact beats conversion; user-defined ranks last
3. **Tie?** Non-template first, then the most specialized template

---

## Ranking Conversions

| Rank | Kind | Example |
|---|---|---|
| 1 | **Exact match** | same type; array → pointer; top-level `const` |
| 2 | **Const conversion** | `int*` → `const int*` |
| 3 | **Promotion** | `char` → `int`, `float` → `double` |
| 4 | **Arithmetic / pointer conversion** | `char` → `long`, `int` → `double`; `Derived*` → `Base*` |
| 5 | **Class-type conversion** | `const char*` → `std::string` |

---

## Promotion vs. Arithmetic Conversion

**Rule of thumb:** a promotion moves **up to the default size**: `int` or `double`.

**Promotions:**
- `bool`, `char`, `short` (and `unsigned` versions) → `int`
- `float` → `double`

**Everything else is a conversion**, even widening:
- `char` → `long` (skips past `int`)
- `int` → `long`, `int` → `double`

---

## Self-Assessment Questions

---

### Q1: Overloaded Functions

```c++
void f(int x);                   // (1)
void f(double x);                // (2)
void f(long x);                  // (3)
void f(const char* s);           // (4)
void f(const std::string& s);    // (5)

f('a');
```

***Question***: Which overloads are viable? Which one runs?

---

### Q1 Answer

**(1) `f(int)` runs.**

| Overload | Viable? | `char` needs |
|---|---|---|
| (1) `f(int)` | yes | **promotion** |
| (2) `f(double)` | yes | arithmetic conversion |
| (3) `f(long)` | yes | arithmetic conversion |
| (4) `f(const char*)` | no | no `char` → pointer conversion |
| (5) `f(const std::string&)` | no | no `string(char)` constructor |

Promotion (rank 3) beats arithmetic conversion (rank 4).

---

### Q1 Answer (cont.): Variations

- **Remove (1):** `f('a')` is **ambiguous**; `char` → `double` and `char` → `long` are both rank 4
- `f(3.14f)` calls **(2)**: promotion
- `f(42L)` calls **(3)**: exact match
- `f("hi")` calls **(4)**, not (5): array → pointer is exact; → `string` is user-defined
- **Remove (4):** `f("hi")` calls **(5)**

---

### Q2: Overloaded Templates

```c++
template <typename T> void g(const T& t);   // (A)
template <typename T> void g(T* p);         // (B)

int x = 42;
int* p = &x;

g(p);
```

***Question***: Which templates are viable? What is `T` in each? Which is called?

---

### Q2 Answer

**Both are viable; (B) is called.**

| Template | Deduced `T` | Parameter type | Match |
|---|---|---|---|
| (A) `g(const T&)` | `int*` | `int* const&` | exact |
| (B) `g(T*)` | `int` | `int*` | exact |

- Both exact, so it's a tie
- (B) accepts only pointers, so it's **more specialized** and wins

---

### Q2 Answer (cont.): A Note on `const`

- In (A), `const` applies to the **reference** (`int* const&`), not the `int`
- Still an exact match, so the tie-breaker decides
- **Live check:** print `__PRETTY_FUNCTION__` (GCC/Clang) in each template

---

### Q3: Templates and a Non-Template

```c++
template <typename T> void g(const T& t);   // (A)
template <typename T> void g(T* p);         // (B)
void g(std::string* p);                      // (C) non-template

std::string s = "hi";
std::string* sp = &s;
int i = 7;
const int* cip = &i;

g(sp);    // call 1
g(i);     // call 2
g(cip);   // call 3
```

***Question***: Which function does each call resolve to?

---

### Q3 Answer

| Call | (A) `const T&` | (B) `T*` | (C) `string*` | Result |
|---|---|---|---|---|
| `g(sp)` | exact, `T = string*` | exact, `T = string` | exact | **(C)** |
| `g(i)` | exact, `T = int` | deduction fails | not viable | **(A)** |
| `g(cip)` | exact, `T = const int*` | exact, `T = const int` | not viable | **(B)** |

- `g(sp)`: three-way tie with one **non-template**, so **(C)**
- `g(i)`: (B) can't deduce, (C) can't convert, so **(A)**
- `g(cip)`: (C) can't convert; the templates tie, so the **more specialized (B)**

---

### Q3 Answer (cont.): Variations

- `g(nullptr)` calls **(A)** with `T = std::nullptr_t`
  - (B): `nullptr_t` isn't a pointer, so deduction fails
  - (C): needs a **conversion**; (A) is **exact**
- `g<std::string>(sp)` calls **(B)**: explicit arguments skip (C)
- Change (C) to `g(const std::string*)`, call `g(sp)`: **(B)** wins, since (C) now needs a const conversion

---

## Studio 14

- Member vs. non-member `operator<`: what if **both** exist?
- Template vs. non-template with the **same name**
- Adding a more specialized **pointer** template
- C-style strings, `std::string`, and a **missing declaration**

**Predict** which function is called before compiling, then check the output.
