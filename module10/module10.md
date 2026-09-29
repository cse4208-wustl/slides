# Dynamic Memory Management and Smart Pointers
---

## Self-Assessment Questions

---
### Q1: What Goes Wrong with `new` and `delete`?

```c++
struct Widget { int id; /* ctor/dtor print "+/- Widget id" */ };

void f() {
    Widget *a = new Widget(1);
    Widget *b = new Widget(2);
    Widget *c = b;
    delete b;
    cout << c->id << endl;   // (A)
    delete c;                // (B)
    a = new Widget(3);       // (C)
}                            // (D)
```

***Question***: Which lines are errors, and which of these does the compiler catch?

---
### Q1 Answer
- The three classic errors of `new`/`delete`:
   - **(A) Use after delete** — `c` and `b` pointed to the *same* object; `c` is now a **dangling pointer**. Undefined behavior.
   - **(B) Double delete** — same memory freed twice; can corrupt the free store. Undefined behavior.
   - **(C)/(D) Memory leak** — overwriting `a` loses the only pointer to `Widget 1`; at (D) `a` goes out of scope and `Widget 3` is lost too. A pointer going out of scope does **nothing** to the object it points to.
- The compiler catches **none** of these — it can't tell static vs. dynamic memory, or whether memory was already freed.
- `b = nullptr;` after `delete b;` protects `b` only — not `c`, or any other alias.

---
### Q2: `shared_ptr` Reference Counting

```c++
auto p = make_shared<int>(42);
auto q(p);
auto r = make_shared<int>(100);
cout << p.use_count() << " " << r.use_count() << endl;       // (A)
r = q;                                                        // (B)
cout << p.use_count() << " " << r.use_count() << endl;       // (C)
{
    auto s = p;
    cout << s.use_count() << endl;                            // (D)
}
p.reset();
cout << q.use_count() << " " << (p ? "p" : "null") << endl;  // (E)
```

***Question***: What is printed? When is each `int` freed?

---
### Q2 Answer
- **(A)** `2 1` — `q(p)` is a copy: both share the `42`
- **(B)** `r = q` increments `42`'s count and decrements `100`'s count → `0` → **`100` is freed right here**.
- **(C)** `3 3` — `p`, `q`, `r` all share the `42`.
- **(D)** `4`; at the `}` `s` is destroyed → back to `3`.
- **(E)** `2 null` — `reset()` makes `p` null and decrements the count.
- `42` is freed when the last of `q`, `r` is destroyed.
- Count goes **up** on: copy-init, right operand of `=`, pass/return **by value**.
- Count goes **down** on: assigning a new value, `reset()`, destruction.

---
### Q3: Initializing Smart Pointers

```c++
shared_ptr<int> p1 = new int(1024);          // (A)
shared_ptr<int> p2(new int(1024));           // (B)
auto p3 = make_shared<int>(1024);            // (C)
p3 = new int(7);                             // (D)
p3.reset(new int(7));                        // (E)
int *p4 = new (nothrow) int(5);              // (F)

shared_ptr<int> clone(int v) {
    return new int(v);                       // (G)
}
```

***Question***: Which lines compile? What does `(nothrow)` do?

---
### Q3 Answer
- **(A) Error** — the raw-pointer constructor is **`explicit`** (ownership must never transfer by accident); `=` is copy-init and needs an *implicit* conversion.
- **(B) OK** — direct initialization.
- **(C) OK** — preferred: no raw pointer ever exists.
- **(D) Error** — no assignment from a raw pointer (`no match for 'operator='`).
- **(E) OK** — `reset` is the explicit way to re-point; frees the old `1024` if `p3` was its only owner.
- **(F) OK** — a *placement* `new`: `nullptr` on failure instead of throwing `bad_alloc`.
- **(G) Error** — `return` is also copy-initialization. Fix: `return shared_ptr<int>(new int(v));` or `return make_shared<int>(v);`

---
### Q4: Don't Mix Raw Pointers and Smart Pointers

```c++
void process(shared_ptr<int> ptr) { /* use ptr */ }

auto sp = make_shared<int>(42);
process(sp);                              // (A)
int *x = new int(1024);
process(x);                               // (B)
process(shared_ptr<int>(x));              // (C)
cout << *x << endl;                       // (D)
process(shared_ptr<int>(sp.get()));       // (E)
delete sp.get();                          // (F)
```

***Question***: What happens on each line?

---
### Q4 Answer
- **(A) OK** — by-value copy: count is `2` inside `process`, back to `1` after.
- **(B) Error** — no implicit `int*` → `shared_ptr<int>` conversion.
- **(C) Compiles, but…** the temporary `shared_ptr` becomes the *only* owner (count `1`); it's destroyed at the end of the statement → **the `int` is deleted**.
- **(D) Undefined** — `x` is now dangling.
- **(E) Undefined** — creates a **second, independent** `shared_ptr` (its own count of `1`) for memory that `sp` already owns → freed after the call; `sp` dangles and is deleted again later.
- **(F) Undefined** — double delete when `sp` is destroyed.
- Rules: once a smart pointer owns an object, don't use a built-in pointer to it; use `get()` only for code that will **not** `delete` it; **never** use `get()` to initialize or `reset` another smart pointer.

---
### Q5: `unique_ptr` — Exclusive Ownership

```c++
unique_ptr<string> p1(new string("Stegosaurus"));
unique_ptr<string> p2(p1);                    // (A)
unique_ptr<string> p3 = p1;                   // (B)
unique_ptr<string> p4(p1.release());          // (C)
unique_ptr<string> p5(new string("Trex"));
p4.reset(p5.release());                       // (D)
p4.release();                                 // (E)

unique_ptr<string> make(const string &s) {
    unique_ptr<string> ret(new string(s));
    return ret;                               // (F)
}
```

***Question***: Which lines compile? After each valid line, who owns what?

---
### Q5 Answer
- **(A), (B) Error** — `use of deleted function unique_ptr(const unique_ptr&)`: no copy, no assignment.
- **(C) OK** — `release()` returns the pointer and makes `p1` null; `p4` owns *Stegosaurus*.
- **(D) OK** — `p5` gives up *Trex*; `reset` **deletes** *Stegosaurus* and `p4` now owns *Trex*.
- **(E) Compiles, but leaks** — `release()` does not free anything; the returned pointer is dropped. Correct: `p4.reset()` or `auto p = p4.release(); ... delete p;`

---
### Q5 Answer (cont.)
- **(F) OK** — the one "copy" exception: `ret` is about to be destroyed, so ownership **moves** to the caller.
- Why doesn't `shared_ptr` have `release()`? Other owners may still exist — one owner can't just walk away with the pointer.

---
### Q6: `weak_ptr` — Observing Without Owning

```c++
weak_ptr<int> wp;
{
    auto sp = make_shared<int>(42);
    wp = sp;
    cout << sp.use_count() << " " << wp.expired() << endl;  // (A)
    if (auto np = wp.lock())
        cout << *np << " " << np.use_count() << endl;       // (B)
}
cout << wp.use_count() << " " << wp.expired() << endl;      // (C)
auto np = wp.lock();
cout << (np ? "alive" : "gone") << endl;                     // (D)
cout << *wp << endl;                                         // (E)
```

***Question***: What is printed? Why can't we dereference a `weak_ptr` directly?

---
### Q6 Answer
- **(A)** `1 0` — binding a `weak_ptr` does **not** change the count.
- **(B)** `42 2` — `lock()` returns a real `shared_ptr`, a *temporary owner* that keeps the object alive while we use it.
- **(C)** `0 1` — at the `}` the last owner died, so the `int` was freed even though `wp` still refers to it.
- **(D)** `gone` — `lock()` on an expired `weak_ptr` returns a null `shared_ptr`.
- **(E) Error** — `weak_ptr` has no `*` or `->`: the object might not exist, so access **must** go through `lock()` (check-then-use in one step).

---
### Q6 Answer (cont.)
- Uses of `weak_ptr`:
   - Checked "iterators" that must not keep data alive (`StrBlobPtr`).
   - Caches and observers.
   - **Breaking cycles**: if `A` owns `B` and `B` owns `A` with `shared_ptr`s, neither count ever reaches `0` → leak. Make the back-link a `weak_ptr`.

---
## Smart Pointers — Quick Reference

| Syntax | Meaning |
|---|---|
| `auto sp = make_shared<T>(args);` | Allocate + construct; one owner |
| `shared_ptr<T> sp(new T(args));` | Direct init only (constructor is `explicit`) |
| `sp.use_count()` / `sp.reset()` | Owners (debugging) / give up ownership |
| `shared_ptr<T> sp(q, d)` | Use callable `d` instead of `delete` |

---
## Smart Pointers — Quick Reference (cont.)

| Syntax | Meaning |
|---|---|
| `auto up = make_unique<T>(args);` | Exclusive owner (C++14) |
| `up.release()` / `up.reset(q)` | Give up without freeing / free and re-point |
| `unique_ptr<T, D> up(q, d)` | Deleter type `D` is part of the type |
| `weak_ptr<T> wp(sp);` | Non-owning; count unchanged |
| `wp.lock()` / `wp.expired()` | `shared_ptr` or null / is it gone? |
| `p.get()` | Raw pointer — never `delete` it, never give it to another smart pointer |
