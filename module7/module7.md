# Function Pointers
---

## Self-Assessment Questions
---
### Q1: Function Type vs. Pointer to Function (Matching)

```c++
bool lengthCompare(const string &a, const string &b) {
    return a.size() < b.size();
}

decltype(lengthCompare) Func;      // (1)
decltype(lengthCompare) *FuncP;    // (2)
```

***Question***: Match each declaration to what it actually names — one is a **function type**, the other is a **pointer to function**.

| Declaration | Names a... |
|---|---|
| (1) `decltype(lengthCompare) Func;` | A. Function type |
| (2) `decltype(lengthCompare) *FuncP;` | B. Pointer to function |

---
### Q1 Answer
**(1) → A, Function type. (2) → B, Pointer to function.**

- `decltype(f)` is f's exact type — a **function type**, not a pointer. No auto-conversion here.
- Function type: describes the call's shape only. No object, no variable, no assignment.
- Adding `*` turns it into real object type: a **pointer to function** — assignable, reassignable, callable.

---
### Q2: Why the Parentheses Matter

```c++
bool (*pf)(const string &, const string &);   // (A)
bool *pf(const string &, const string &);     // (B)
```

***Question***: For each declaration, show one line of code demonstrating how `pf` can actually be used.

---
### Q2 Answer

```c++
// (A): pf is a variable - assign it a function, then call through it
bool (*pf)(const string &, const string &) = lengthCompare;
bool same = pf("hi", "there");           

// (B): pf is a function - assuming it is implemented, you call it like any function
bool *pf(const string &s1, const string &s2) {
    bool *result = new bool;
    result = s1.size() < s2.size();
    return result;
}
bool *r = pf("hi", "there");                      // ordinary function call, returns bool*
```

- (A): `bool (*pf) ...`: **pointer to a function** returning `bool`. A variable — assign it, reassign it, call through it.
- (B): `bool *pf ...`: **function** returning `bool*`. Not a variable — define once, call.
- Parentheses decide which one you get.

---
### Q3: Calling Through a Function Pointer (True/False)

```c++
bool lengthCompare(const string &a, const string &b);

bool (*pf)(const string &, const string &) = lengthCompare;

bool b1 = pf("hi", "there");
bool b2 = (*pf)("hi", "there");
bool b3 = lengthCompare("hi", "there");
```

***True or False***: `b1`, `b2`, and `b3` all call `lengthCompare` and produce the same result.

---
### Q3 Answer
**True.**

- Function name used as a value → auto-converted to a pointer. `&` optional.
- `pf(...)` and `(*pf)(...)` — equivalent, no explicit dereference needed.
- `b1`, `b2`, `b3`: same function, same arguments, same result.

---
### Q4: Pointers to Overloaded Functions

```c++
void ff(int*);
void ff(unsigned int);

void (*pf1)(unsigned int) = ff;   // (1)
void (*pf2)(int) = ff;            // (2)
```

***Question***: Which line(s) fail to compile, and why?

---
### Q4 Answer
Only line (2) fails.

- Pointer's declared type must match an overload **exactly** — no implicit conversions.
- `pf1`: `void(*)(unsigned int)` — exact match for `ff(unsigned int)`. Compiles.
- `pf2`: `void(*)(int)` — no exact match (`ff(int*)`, `ff(unsigned int)` both differ). Error.

---
### Q5: Simplifying with Type Aliases and `decltype`

```c++
void useBigger(const string &s1, const string &s2,
               bool (*pf)(const string&, const string&));
```

***Question***: Give one way to define a type alias for "pointer to a function that takes two `const string&` and returns `bool`".

---
### Q5 Answer

```c++
typedef bool (*FuncP)(const string&, const string&);   // spelled out by hand
using FuncP2 = decltype(lengthCompare) *;               // derived from an existing function
```

- `typedef`: spelled out by hand — correct, verbose for complex signatures.
- `decltype(lengthCompare)`: the function type, not a pointer (Q1) — add `*` yourself.
- `FuncP` and `FuncP2`: same type either way.
- Parameter written as bare function type (no `*`) auto-converts to a pointer. Parameters only — not return types (Q6).

---
### Q6: Declaring a Function That Returns a Pointer to `sumLength`

```c++
string::size_type sumLength(const string&, const string&);
```

***Question***: Declare a function `getFcn` that takes a single `const string&` parameter and can return a pointer to `sumLength`.

---
### Q6 Answer

Any of these declare a valid `getFcn`:

```c++
decltype(sumLength) *getFcn(const string &);              // (1) decltype + explicit *
auto getFcn(const string &) -> decltype(sumLength) *;     // (2) trailing return, same meaning

using PF = decltype(sumLength) *;
PF getFcn(const string &);                                // (3) via a type alias
```

- Return type must be a **pointer**, not the bare function type — no auto-conversion for return types (like arrays; see Q1).
- `decltype(sumLength)`: exact signature; add `*` → legal return type.
- (2): same as (1), trailing-return form — reads better for complex signatures.
- `return sumLength;` — bare name auto-converts to pointer, same as Q1/Q3.

---

## Live Demo

Exampe code/scenarios in `demo.cpp`.
