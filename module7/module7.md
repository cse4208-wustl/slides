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

- `decltype(f)` = f's exact type — a **function type**, not a pointer. No auto-conversion here.
- Function type: describes the call's shape only. No object, no variable, no assignment.
- Add `*` → a real object type: a **pointer to function** — assignable, reassignable, callable.
- Same idea resurfaces in Q2 (parentheses) and Q6 (return types).

---
### Q1 Demo: Try to Use Each One as a Variable

- Declare `Func` as in (1) and try `Func = lengthCompare;` — compile error, `Func` isn't a variable.
- Declare `FuncP` as in (2) and try the same assignment, then call `FuncP("hi", "there")` — compiles and runs fine.
- Show that a function type is still useful as a building block: `using PlainFunc = decltype(lengthCompare);` can be used to declare a *parameter* (`void f(PlainFunc cmp)`) because parameter types of function type are auto-converted to pointers — but the same alias cannot declare a local variable.

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
// (A): pf is a variable -- assign it a function, then call through it
bool (*pf)(const string &, const string &) = lengthCompare;
bool same = pf("hi", "there");                    // ordinary call through the pointer

// (B): pf is a function -- it needs a body, then you call it like any function
bool *pf(const string &s1, const string &s2) {
    static bool result;
    result = s1.size() < s2.size();
    return &result;
}
bool *r = pf("hi", "there");                      // ordinary function call, returns bool*
```

- (A): `*pf` in parens → **pointer to a function** returning `bool`. A variable — assign it, reassign it, call through it.
- (B): no parens → **function** returning `bool*`. Not a variable — define once, call.
- Parentheses decide which one you get.

---
### Q2 Demo: See Both Forms Fail Differently

- Declare `pf` as in (A), then try `pf = 0;` — compiles fine, `pf` is a pointer variable.
- Declare `pf` as in (B) (comment out (A)), then try the same `pf = 0;` — compile error, because `pf` is a function name, not a variable you can assign to.
- Ask the class to predict the error message *before* compiling.

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
### Q3 Demo: Toggle the Syntax

- Change `pf = lengthCompare;` to `pf = &lengthCompare;` — nothing changes.
- Change the call site from `pf("hi","there")` to `(*pf)("hi","there")` and back — same output either way.
- Ask: "if this always needs `&` and `*` like a normal pointer, would this feel more or less intuitive?"

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
### Q4 Demo: Add a Matching Overload

- Add `void ff(int);` as a third overload.
- Recompile line (2) — it now succeeds, and `pf2` binds to the new overload.
- Point out that adding *any* function with a matching signature elsewhere in the overload set is enough — the pointer doesn't care which overload it "should" match, only which one matches exactly.

---

### Q5: Simplifying with Type Aliases and `decltype`

```c++
void useBigger(const string &s1, const string &s2,
               bool (*pf)(const string&, const string&));
```

***Question***: Give two different ways to define a type alias for "pointer to a function that takes two `const string&` and returns `bool`," and explain the difference between them.

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
### Q5 Demo: Rewrite `useBigger` Two Ways

- Rewrite the parameter using `FuncP`, then again using `FuncP2` — both compile identically.
- Call `useBigger(s1, s2, lengthCompare)` — the bare function name still auto-converts, whichever alias is used.
- Try writing the parameter with the function type itself (`bool pf(const string&, const string&)`, no `*`, no alias) to show it's accepted too.

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
### Q6 Demo: Trigger the Error, Then Fix It

- Have a student try `decltype(sumLength) getFcn(const string &);` (no `*`) first, and read the actual compiler error out loud.
- Fix it by adding the `*`.
- Rewrite the fixed function using the trailing-return form and confirm it behaves identically.

---

## Code Snippet — `demo.cpp`

The file below ties all six questions together in one program: declaring a function pointer, calling through it both ways, passing one as a parameter (via a type alias), and returning one (via trailing return + `decltype`). Run it as-is first, then work through the suggested live changes.

```c++
#include <iostream>
#include <string>
using namespace std;

// Q3/Q4: candidate comparison functions -- same signature, different behavior
bool lengthCompare(const string &s1, const string &s2) {
    cout << "  lengthCompare(" << s1 << ", " << s2 << ")\n";
    return s1.size() < s2.size();
}

bool alphaCompare(const string &s1, const string &s2) {
    cout << "  alphaCompare(" << s1 << ", " << s2 << ")\n";
    return s1 < s2;
}

// Q5: a type alias for "pointer to function(const string&, const string&) -> bool"
using CompareFn = bool (*)(const string &, const string &);
// equivalent alternative: using CompareFn2 = decltype(lengthCompare) *;
// NOTE (Q1): decltype(lengthCompare) alone -- with no '*' -- names the function
// type, not a pointer. You cannot declare a plain variable with it.

// Q5: function pointer as a parameter
void useComparator(const string &s1, const string &s2, CompareFn cmp) {
    cout << (cmp(s1, s2) ? "true" : "false") << '\n';
}

// Q6: function returning a pointer to function, via trailing return
auto getComparator(bool byLength) -> CompareFn {
    if (byLength)
        return lengthCompare;   // auto-converted: function name -> pointer
    else
        return alphaCompare;
}

int main() {
    // Q2/Q3: declare, initialize, and call through a function pointer
    CompareFn pf = lengthCompare;                 // pf = &lengthCompare is equivalent
    cout << "Direct call:       " << pf("hi", "there") << '\n';
    cout << "Dereferenced call: " << (*pf)("hi", "there") << '\n';

    cout << "\n--- passing a function pointer as an argument ---\n";
    useComparator("hi", "there", lengthCompare);
    useComparator("hi", "there", alphaCompare);

    cout << "\n--- getting a function pointer back from a function ---\n";
    CompareFn chosen = getComparator(true);
    cout << "chosen(\"cat\", \"caterpillar\") = "
         << chosen("cat", "caterpillar") << '\n';
}
```

### Suggested Live Changes

- **Show the bare function type (Q1).** Add `using PlainFunc = decltype(lengthCompare);` then try `PlainFunc pf2 = lengthCompare;` — compile error, because `PlainFunc` names a function type, not a pointer. Fix it by changing the alias to `using PlainFunc = decltype(lengthCompare) *;` and rerun.
- **Bring back the Q2 gotcha.** Above `main`, temporarily add `bool *pf(const string&, const string&);` (no parentheses) right next to the `CompareFn pf = lengthCompare;` line and try to assign to it — show the compile error live, then remove it.
- **Break the overload match (Q4).** Add a second overload, e.g. `bool lengthCompare(int, int);`, then declare a raw `bool (*pf3)(int, int) = lengthCompare;` — show it binds to the *new* overload, not the string one.
- **Strip the alias (Q5).** Rewrite `useComparator`'s parameter by hand as `bool (*cmp)(const string&, const string&)` instead of `CompareFn cmp` — same behavior, more typing. Then try the plain function-type spelling `bool cmp(const string&, const string&)` (no `*`) to show it's accepted too.
- **Break the return type (Q6).** Change `auto getComparator(bool) -> CompareFn` to `auto getComparator(bool) -> decltype(lengthCompare)` (drop the `*`) and watch it fail to compile; put the `*` back to fix it.
- **Break a signature on purpose.** Try assigning a function with a mismatched parameter or return type to `CompareFn` and read the compiler's error message together — connect it back to Q4's "must match exactly" rule.
