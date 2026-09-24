# Pointers to Class Members
---

## Self-Assessment Questions

---
### Q1: Pointer to Data Member vs. Ordinary Pointer

```c++
struct Student { string first, last, major; };

Student a{"Ada", "Lovelace", "Math"}, b{"Alan", "Turing", "CSE"};
string Student::*pm = &Student::last;

cout << a.*pm << " " << b.*pm << endl;   // ???
pm = &Student::major;
cout << a.*pm << " " << b.*pm << endl;   // ???
```

***Question***: What is printed? After `pm = &Student::last;`, what does `pm` actually point to?

---
### Q1 Answer
- Prints `Lovelace Turing`, then `Math CSE`.
- `pm` identifies a **member of the class** (`Student::last`), not a member of any object.
   - Think of it as "which column", not "which cell".
- Object is supplied when the pointer is used: `a.*pm` → "the `last` member **of `a`**".
- `string Student::*` means it can point to any `string` member of `Student`, but nothing else.
- Ordinary pointer: an address of one object in memory. Pointer to member: an identity of a member, applied later to *any* object.
- Use case: same-typed members (first/last/major) + many objects → print a roster table by iterating over "columns".
- Declaring it `const string Student::*` makes access through it read-only.

---
### Q2: Forming Pointers to Member Functions

```c++
class Screen {
public:
    using pos = string::size_type;
    char get_cursor() const;
    char get() const;
    char get(pos ht, pos wd) const;
};

auto pmf = &Screen::get_cursor;                                  // (A)
pmf = &Screen::get;                                              // (B)
auto pmf2 = &Screen::get;                                        // (C)
char (Screen::*pmf3)(Screen::pos, Screen::pos) const = &Screen::get; // (D)
pmf = Screen::get_cursor;                                        // (E)
```

***Question***: Which lines compile? Explain the ones that don't.

---
### Q2 Answer
- **(A) OK** — `pmf` is deduced as `char (Screen::*)() const`.
- **(B) OK** — `get` is overloaded, but the *target type* of `pmf` is already known, so the no-argument `const` version is selected.
- **(C) Error** — `unable to deduce 'auto' from '&Screen::get'`: `Screen` has two overloaded `get` functions. Overloaded members need an **explicit type**.
- **(D) OK** — explicit type picks the two-parameter version. The `const` is part of the type and must be written.
- **(E) Error** — `invalid use of non-static member function`. Unlike ordinary functions, there is **no automatic function-to-pointer conversion** for member functions: `&` is required.

---
### Q3: Calling Through a Pointer to Member Function

```c++
Screen myScreen, *pScreen = &myScreen;
auto pmf = &Screen::get_cursor;

char c1 = myScreen.*pmf();        // (A)
char c2 = (myScreen.*pmf)();      // (B)
char c3 = (pScreen->*pmf)();      // (C)
char Screen::*p(Screen::pos, Screen::pos) const;   // (D)
```

***Question***: Which lines are correct? What goes wrong with the others?

---
<style>
  .c-decl  { color: #1e88e5; }  /* char c1 =  */
  .c-obj   { color: #e53935; }  /* myScreen.* */
  .c-call  { color: #43a047; }  /* pmf()      */
</style>

### Q3 Answer
- **(A) Error**: without parantheses, this will be parsed as:
<code><span class="c-decl">char c1 = </span><span class="c-obj">myScreen.*</span><span class="c-call">pmf()</span>;</code>
- **(B) OK** — `.*` combines an **object** with the pointer to member; then call it.
- **(C) OK** — `->*` combines a **pointer to an object** with the pointer to member.
- **(D) Error** — without parentheses, this will be parsed as: 
<code><span class="c-decl">char Screen::*</span><span class="c-obj">p(Screen::pos, Screen::pos)</span> <span class="c-call">const</span>;</code>
Should be
<code><span class="c-decl">char</span><span class="c-obj">(Screen::*p)</span><span class="c-call">(Screen::pos, Screen::pos) const</span>;</code>
- Rule of thumb: parentheses in **both** places — `(C::*p)(params)` to declare, `(obj.*p)(args)` to call.
- Type aliases hide the declaration mess:
  `using Action = char (Screen::*)(Screen::pos, Screen::pos) const;`

---
### Q4: Member Pointers and Inheritance

```c++
struct Base { 
    string name; 
    virtual string who() const { return "Base"; } 
};
struct Derived : Base { 
    string tag; 
    string who() const override { return "Derived"; } 
};

string Derived::*pd = &Base::name;       // (A)
string Base::*pb    = &Derived::tag;     // (B)

auto pw = &Base::who;
Derived d;
Base &rb = d;
cout << (rb.*pw)() << endl;              // (C) ???
```

***Question***: Do (A) and (B) compile? What does (C) print?

---
### Q4 Answer
- **(A) OK** — every `Derived` object *has* a `name` (inherited), so a pointer to a `Base` member converts implicitly to a pointer to a `Derived` member.
   - Note the direction is **opposite** to ordinary pointers (`Derived*` → `Base*`).
- **(B) Error** — not every `Base` has a `tag`, so `string Derived::*` → `string Base::*` is not allowed implicitly.
- **(C) prints `Derived`** — a pointer to a **virtual** member function still dispatches dynamically based on the object's run-time type.
   - It names "the `who` slot", not "`Base::who`'s code" — implementations typically use the v-table (recall module 6).

---
### Q5: Pointers to Private Members

```c++
class Screen {
public:
    static const string Screen::*data() { return &Screen::contents; }
private:
    string contents;
};

const string Screen::*pdata = &Screen::contents;   // (A) in main()
const string Screen::*pdata2 = Screen::data();     // (B) in main()
Screen s;
cout << s.*pdata2 << endl;                         // (C)
s.*pdata2 = "hello";                               // (D)
```

***Question***: Which lines compile outside the class, and why is `data()` `static`?

---
### Q5 Answer
- **(A) Error** — normal access control applies: `contents` is private, so `&Screen::contents` is only legal in members/friends of `Screen`.
- **(B) OK** — the class itself chooses to hand out a pointer to its private member: a **controlled violation of encapsulation**.
- **(C) OK** — once you have the pointer, you apply it to any object; it acts as a *deferred accessor*.
- **(D) Error** — the pointer is to a `const string` member → read-only access.
- `data()` is `static` because it needs **no object**: the pointer identifies a member of the class, not of any object.
- Return type reads right to left: pointer to a member of `Screen` that is a `string` that is `const`.

---
### Q6: Member Functions as Callable Objects

```c++
vector<string> svec = {"alpha", "", "gamma"};
auto fp = &string::empty;
auto it = find_if(svec.begin(), svec.end(), fp);   // ???
```

***Question***: Why doesn't this compile? Give three ways to fix it.

---
### Q6 Answer
- A pointer to member function is **not a callable object** — it can't be invoked with `fp(x)`; `find_if` internally does `fp(*it)` (instead of `it->*fp()`).
- Fixes — each makes the implicit `this` parameter **explicit**:

```c++
function<bool (const string&)> fcn = &string::empty;   // (1) spell out signature
find_if(svec.begin(), svec.end(), fcn);

find_if(svec.begin(), svec.end(), mem_fn(&string::empty));      // (2) type deduced

find_if(svec.begin(), svec.end(), bind(&string::empty, _1));    // (3) _1 is the object
```

- `function`: you choose reference vs. pointer in the signature (`const string&` vs. `const string*`).
- `mem_fn` and `bind`: resulting callable accepts **either** an object or a pointer to one.

---
## Member Function Table

```c++
class Cursor {
public:
    using Action = Cursor& (Cursor::*)();
    enum Direction { HOME, FORWARD, BACK };
    Cursor& home();  Cursor &forward();  Cursor &back();
    Cursor& move(Direction d) { return (this->*Menu[d])(); }
private:
    static Action Menu[];
};
Cursor::Action Cursor::Menu[] = { &Cursor::home, &Cursor::forward, &Cursor::back };

c.move(Cursor::FORWARD).move(Cursor::BACK);
```

- `Menu` is `static` — one table shared by all objects; order must match the `enum`.
- `move` selects **which** member at run time; `this` supplies **which object**.
- Same idea compilers use for virtual functions (v-tables).

---
## Pointer to Member — Quick Reference

| Syntax | Meaning |
|---|---|
| `T C::*p = &C::m;` | Pointer to member `m` (type `T`) of class `C` |
| `R (C::*pf)(Args) const = &C::f;` | Pointer to `const` member function |
| `obj.*p` / `ptr->*p` | Apply member pointer to an object |
| `(obj.*pf)(args)` | Call through member function pointer |
| `using Action = R (C::*)(Args);` | Type alias for a member function pointer |
| `function<R (C&, Args)>` | Callable wrapper — signature adds the object |
| `mem_fn(&C::f)` | Callable, type deduced; takes object or pointer |
| `bind(&C::f, _1, ...)` | Callable; `_1` is the object; can fix other args |

