# Classes
---

## Self-Assessment Questions
---
### Q1: Functions vs. Classes

A plain C++ function encapsulates **behavior** — data comes in via parameters, a result goes out via `return`. A class (or struct) encapsulates something more.

What else does a class encapsulate that a function alone cannot?

---
### Q1 Answer
**State.**

- A function's parameters and locals disappear the moment it returns — it has no memory of anything between calls.
- A class bundles **member variables** (state that persists across calls) together with **member functions and operators** that have direct access to that state on whatever object they're called on.
- Call `insert` on a `Bag` twice, and the second call sees what the first one left behind.

---

### Q2: `struct` vs. `class` (True/False)

```c++
struct Point2D {
    Point2D(int x, int y);
    bool operator<(const Point2D &) const;
    int x;
    int y;
};
```
```c++
int main(){
    Point2D p(0,0);
    p.x = 3;
    p.y = 5;
    return 0;
}
```
***True or False***: If we changed the keyword `struct` to `class` and changed nothing else, we would still be abble to access `p.x` and `p.y` in `main`.

---
### Q2 Answer
**False.**

- The *only* difference between `struct` and `class` in C++ is the default access level.
- `struct` members default to `public`; `class` members default to `private`.
- Swap the keyword with nothing else changed, and `x`/`y` silently become `private` — any outside code reading `p.x` stops compiling.

---

### Q3: `const` Member Functions (True/False)

```c++
bool Point2D::operator<(const Point2D &p2d) const
{
    return (x < p2d.x) ||
           ((x == p2d.x) && (y < p2d.y));
}
```
```c++
const Point2D p1(0,0);
Point2D p2(3,3);
if (p1 < p2) {
    cout <<"p1 is less than p2" << endl;
}
```

***True or False***: In the `operator<` function, the trailing `const` is optional, it's just documentation, and removing it wouldn't change what code compiles.

---
### Q3 Answer
**False.**

- The trailing `const` is a real promise, enforced by the compiler: this function will not modify the object it's called on.
- That's what makes it legal to call `operator<` on a `const Point2D` or through a `const Point2D&`.
- Drop the `const`, and any code that only has a `const Point2D` (or a const reference) would no longer be able to call it.

---
### Q3 Takeaway

* Trailing `const` on a member function → promises not to modify the object it's called on
* This is what lets the function be called on `const` objects / through `const` references
* `operator<`, accessors, and most comparison/query operations should be `const`

---

### Q4: Member Initialization Lists (True/False)

```c++
class Date {
public:
    Date(int d, int m, int y);
private:
    int day, month, year;
};

Date::Date(int d, int m, int y)
    : year(y), month(m), day(d)   // note the order here!
{}
```

***True or False***: `year` is guaranteed to be initialized before `month` and `day`, because it's listed first in the initialization list.

---
### Q4 Answer
**False.**

- Members are always constructed in the order they were **declared** in the class (`day`, then `month`, then `year`) — **not** the order they appear in the initialization list.
- Writing the list in a different order doesn't change construction order; it just makes the code confusing (and some compilers will warn you about exactly this).
- Best practice: always write your initialization list in declaration order.

---
### Q4 Takeaway — Rules for the Init List

* Members are constructed in **declaration order**, always — write your list to match
* You **must** initialize `const` members and reference members there (can't assign to them in the constructor body — too late)
* Avoid putting initializations there that might fail — save those for the constructor body, where you have normal exception handling

---

### Q5: Accessors and Mutators (True/False)

```c++
class Date {
public:
    int day() const;      // accessor
    void day(int);         // mutator
private:
    int day_;
};
```

***True or False***: This is a compile error — you can't have two member functions both named `day`.

---
### Q5 Answer
**False — this compiles fine.**

- This is ordinary function **overloading**: `int day() const` and `void day(int)` have different parameter lists (zero parameters vs. one `int`), so the compiler treats them as two distinct functions that happen to share a name.
- Callers disambiguate automatically based on how they call it — `someDate.day()` reads the day, `someDate.day(15)` sets it.

> Why must the accessor (`day()`) be `const` but the mutator (`day(int)`) must **not** be?

---

### Q6: Default Constructor Synthesis (True/False)

```c++
class Date {
public:
    Date(int d, int m, int y);   // only constructor declared
private:
    int day, month, year;
};

Date today;   // ???
```

***True or False***: This compiles, using a compiler-synthesized default constructor.

---
### Q6 Answer
**False — compile error.**

- The compiler only synthesizes a default constructor when the class declares **no** constructors at all.
- The moment you declare *any* constructor — here, `Date(int, int, int)` — synthesis of the default constructor turns off completely.
- `Date today;` now has no matching constructor to call.

---

### Q7 Copy Constructor Synthesis (True/False)

```c++
class Date {
public:
    Date(int d, int m, int y);   // only constructor declared
private:
    int day, month, year;
};

Date d1(15, 6, 2024);
Date d2(d1);   // ???
```
***True or False***: This compiles, using a compiler-synthesized copy constructor.

---

### Q7 Answer

Q7 Answer

**True**

- Declaring a non-copy constructor (like Date(int, int, int)) suppresses default-constructor synthesis, but does not suppress copy-constructor synthesis.
- The compiler still writes a member-wise copy constructor for Date unless you declare your own copy constructor (or a move constructor / move-assignment operator).
- So Date d2(d1); compiles fine here — `day`, `month`, and `year` are copied field-by-field automatically.

>If Date instead held a raw pointer to dynamically allocated memory, would that synthesized member-wise copy still be a good idea?

---
## Access Specifications — Quick Reference

| Specifier | Visible from... |
|---|---|
| `private` | only within the class itself |
| `protected` | the class **and** derived classes |
| `public` | everywhere |

* Access control in a `class` is `private` by **default** — but best style is to write the specifier explicitly anyway, even when it matches the default
* A `struct` is identical to a `class`, except its default is `public`

---

## Class vs Struct

- The only difference is default access specification
- Both exist for historical reason
  - C++ started as "C with classes" (structs had not functions or access control)
  - Supported structs to keep existing C codd compiling
  - Added class features to struct, but make public by default (for compatibility)
- Common convention - use structs only if you have to encapsulate data (no functions)
