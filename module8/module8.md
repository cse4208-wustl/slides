# Lambda Expressions & Callable Objects
---

## Self-Assessment Questions

### Q1: Capture by Value — Timing

```c++
int count = 0;
auto byValue = [count]() { return count; };
count = 42;
cout << byValue() << endl;   // ???
```

***Question***: What does this print?

---
### Q1 Answer
- Prints `0`, not `42`.
- `[count]` captures `count` **by value** — copies its current value into the lambda's closure **at the moment the lambda is created**, not when it's later called.
- `count = 42` happens *after* the lambda was created, so it only changes the outer `count`.
- The lambda's private copy is untouched.
- Same as ordinary pass-by-value semantics, just shifted in time: the "copy" happens at lambda-construction time instead of call time.

---

### Q2: Returning a Lambda That Captures by Reference

```c++
auto makeAdder(int base) {
    int local = base;
    return [&local](int x) { return local + x; };  // captures local BY REFERENCE
}

auto addFive = makeAdder(5);
cout << addFive(10) << endl;   // ???
```

***Question***: What's wrong with this code, and what actually happens when `addFive(10)` runs?

---
### Q2 Answer
- **Undefined behavior**, not a predictable wrong answer.
- `local` is a local variable of `makeAdder`; the returned lambda captures it **by reference**.
- When `makeAdder` returns, `local` goes out of scope and is destroyed — but the lambda still holds a reference to that now-dead stack location: a **dangling reference**.
- Calling `addFive(10)` reads that dangling reference — the result is undefined: 
   - might look plausible
   - might print garbage
   - might crash
   - can vary across compilers, optimization levels, or runs.
- **Fix:** capture by value instead 
   — `return [local](int x) { return local + x; };`. 
---

### Q3: Mixing Implicit and Explicit Captures

```c++
void report(ostream &os, const vector<int> &v, int threshold) {
    for_each(v.begin(), v.end(),
        [=, &os](int x) {
            if (x > threshold) os << x << " ";
        });
}
```

***Question***: For the lambda passed to `for_each`, what is captured, how, and why doesn't `v` need to be captured at all?

---
### Q3 Answer
- `[=, &os]` sets the **default capture mode to by-value** (`=`), then explicitly overrides `os` to be captured **by reference** (`&os`).
- `threshold` is used in the body but not listed explicitly → captured **implicitly by value** under the `=` default.
- `os` is an `ostream&` — streams can't be copied — so it must be captured by reference; that's why it's called out explicitly.
- `v` is **not captured at all**: the lambda's body never mentions `v` directly. `v.begin()`/`v.end()` are evaluated at the *call site*, in `report`'s own scope, producing the iterator range passed to `for_each`.
- Syntax rule: when mixing implicit and explicit capture, the **first item must be `&` or `=`** (sets the default); every explicitly named variable must use the *opposite* form.

---

### Q4: `bind` and Placeholders

```c++
auto subtract = [](int a, int b) { return a - b; };
auto g = bind(subtract, _2, _1);
cout << g(10, 3) << endl;   // ???
```

***Question***: What does this print?

---
### Q4 Answer
- Prints `-7`.
- `bind(subtract, _2, _1)` builds a new callable `g` whose argument list is rearranged before being forwarded to `subtract`.
- `_1` and `_2` stand for `g`'s **own** parameters, in call order: `_1` = `g`'s 1st argument, `_2` = `g`'s 2nd argument.
- The *position* of each placeholder in `bind`'s argument list determines *which parameter of `subtract`* it fills: `subtract`'s 1st parameter is bound to `_2`, its 2nd parameter to `_1`.
- Calling `g(10, 3)`: `g`'s 1st argument is `10` (→ `_1`), `g`'s 2nd argument is `3` (→ `_2`) → `subtract(_2, _1)` → `subtract(3, 10)` → `3 - 10` → `-7`.

---

### Q5: Lambdas Are Function Objects — `mutable`

```c++
int base = 100;
auto counter = [base]() mutable { return ++base; };
cout << counter() << " " << counter() << " " << base << endl;   // ???
```

***Question***: What's printed, and why doesn't the outer `base` change?

---
### Q5 Answer
- Prints `101 102 100`.
- Compiler translates the lambda into an unnamed class with:
   - a data member for each by-value capture
   - a constructor that initializes it from the captured variable
- The default generated class's `operator()` is `const`
   -a by-value-captured member can't be modified through it.
   - `mutable` removes that `const`, so `++base` inside the lambda is legal and modifies the lambda's **own private copy**.
- `counter` is a single object, so its private copy persists and accumulates across calls: `100 → 101 → 102`.
- The outer `base` was only used to *initialize* the private copy at creation time (same timing rule as Q2) — the lambda never touches the real `base`, it remains as 100.

---
## Lambda Capture List — Quick Reference

| Capture | Meaning |
|---|---|
| `[]` | No captures; body may not use enclosing local variables. |
| `[x, y]` | `x`, `y` captured **by value** (copied at lambda-creation time). |
| `[&x, &y]` | `x`, `y` captured **by reference**. |
| `[&]` | Implicit: everything used from the enclosing scope is captured by reference. |
| `[=]` | Implicit: everything used from the enclosing scope is captured by value. |
| `[&, x]` | Default by-reference; `x` explicitly by value. |
| `[=, &x]` | Default by-value; `x` explicitly by reference. |
