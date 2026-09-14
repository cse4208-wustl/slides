# Inheritance & Polymorphism
---

## Self-Assessment Questions
---
### Q1: The Derived-to-Base Conversion (True/False)

```c++
class Shape {
public:
    virtual double area() const = 0;
};

class Circle : public Shape {
public:
    Circle(double r) : radius_(r) {}
    double area() const override { return 3.14159 * radius_ * radius_; }
private:
    double radius_;
};

void report(const Shape &s) {
    cout << s.area() << endl;
}

int main() {
    Circle c(2.0);
    report(c);   // ???
}
```

***True or False***: This compiles and prints the circle's area, even though `report` only knows about `Shape`.

---
### Q1 Answer
**True.**

- `Circle : public Shape` means every `Circle` object *contains* a `Shape` subobject — that's what public inheritance buys you.
- Because that subobject exists, the compiler can implicitly bind a `const Shape&` to a `Circle` object. This is the **derived-to-base conversion**, and it only runs in this direction — there's no automatic `Shape → Circle`.
- `area()` is `virtual`, so even though `report` only ever sees a `Shape&`, the call is **dynamically bound**: at run time it runs `Circle::area()`.

---
### Q1 Demo: Reference vs. Value

- `report` takes its parameter **by reference**:  `s` is bound directly to `c`, no copy involved, so `s.area()` runs whichever `area()` belongs to the object `c` actually is.
- If we change the parameter to pass **by value** the code doesn't compile, because Shape is pure virtual - cannot make a copy of pure virtual object.
- Give `Shape::area()` a real body instead of `= 0`
    * Now `report(Shape s)` compiles
    * Always prints the `Shape` default, no matter what you pass it. 
    * Copying `c` into `s` only copies the `Shape` part of `c`; the `Circle`-specific pieces never make it in. 
    * That's *class slicing*: dynamic polymorphism depends on passing by reference or pointer.

---

### Q2: Virtual vs. Non-Virtual Dispatch

```c++
class Shape {
public:
    string label() const { return "Shape"; }          // NOT virtual
    virtual string kind() const { return "Shape"; }    // virtual
};
class Circle : public Shape {
public:
    string label() const { return "Circle"; }          // hides, does not override
    string kind() const override { return "Circle"; }
};

Circle c;
Shape *sp = &c;
cout << sp->label() << " " << sp->kind();   // ???
```

***Question***: What does the last line print?

---
### Q2 Answer
It prints `Shape Circle`.

- `label()` is **not** `virtual`, so calls through `sp` are resolved at **compile time**, using `sp`'s *static* type (`Shape*`) — hence `"Shape"`.
- `kind()` **is** `virtual`, so the call is resolved at **run time**, using the *dynamic* type of the object `sp` actually points to (`Circle`) — hence `"Circle"`.
- `Circle::label()` doesn't override `Shape::label()` at all — it just happens to share a name. Calling `c.label()` directly (on the named object, not through a pointer/reference) would print `"Circle"`, because that call is also resolved statically, using `c`'s own type.

---
### Q2 Demo: Object Call vs. Pointer Call

```c++
Circle c(2);
Shape *sp = &c;
cout << c.label() << endl;     // "Circle" -- resolved using c's own (declared) type
cout << sp->label() << endl;   // "Shape"  -- resolved using sp's declared type (Shape*)
```

- Same object in memory (`sp` points at `c`)
- `label()` is non-virtual, each call is resolved at compile time using whatever type is visible right at the call site.
- `kind()` is `virtual`: both `c.kind()` and `sp->kind()` print `"Circle"` — a virtual call looks at the object's actual (dynamic) type instead.

---

### Q3: Virtual Destructors

```c++
class Shape {
public:
    ~Shape() { cout << "~Shape "; }        // NOT virtual
    virtual double area() const = 0;
};
class Circle : public Shape {
public:
    Circle(double r) : radius_(r) {}
    ~Circle() { cout << "~Circle "; }
    double area() const override { return 3.14159 * radius_ * radius_; }
private:
    double radius_;
};

Shape *sp = new Circle(2.0);
delete sp;   // ???
```

***Question***: What does this code print?

---
### Q3 Answer
In practice, just `~Shape ` — `~Circle` never runs.

- Deleting a derived object through a base pointer whose destructor is **not** `virtual` is undefined behavior, so no compiler is *required* to behave any particular way here — but every mainstream compiler resolves `delete sp` the same way it resolves any non-virtual call through a pointer: statically, using `sp`'s type (`Shape*`). So only `~Shape()` runs — `~Circle()` is silently skipped, along with any cleanup it was responsible for.
- Fix: `virtual ~Shape() { ... }`. Once the base destructor is `virtual`, it's `virtual` in every derived class automatically, and `delete sp` correctly runs `~Circle()` *then* `~Shape()`, printing `~Circle ~Shape `.

---
### Q3 Demo: Fixing It with `virtual`

- Remove `virtual` from `~Shape` and remove `override` from other destructors
- Different compilers will behave differently
- Compile and run this code
---

### Q4: `protected` Access

```c++
class Shape {
protected:
    string name_ = "Shape";
public:
    virtual double area() const = 0;
};
class Circle : public Shape {
public:
    Circle(double r) : radius_(r) { name_ = "Circle"; }   // (1)
private:
    double radius_;
};

int main() {
    Circle c(2.0);
    c.name_ = "Sphere";                                   // (2)
}
```

***Question***: Line (1), (2), or both cause a compiler error. Which one(s) and why?

---
### Q4 Answer
Only line (2) fails to compile. Line (1) is fine.

- `protected` members are accessible from **member functions of the class itself and of any derived class** — that's exactly what `Circle`'s constructor is, so (1) compiles.
- `protected` does **not** open access to arbitrary outside code. `main` is not a member of `Shape` or `Circle`, so (2) fails to compile, the same as if `name_` were `private`.
- A base class uses `protected` for members it wants derived classes, and only derived classes, to use directly.

---
### Access Control — Quick Reference

| Specifier | Class itself | Derived classes | Outside code |
|---|---|---|---|
| `public` | yes | yes | yes |
| `protected` | yes | yes | **no** |
| `private` | yes | **no** | no |

---

### Q5: Abstract Base Classes (True/False)

```c++
class Shape {
public:
    virtual double area() const = 0;     // pure virtual
    virtual ~Shape() = default;
};

Shape s;                          // (1)
Shape *sp = new Circle(2.0);      // (2) — Circle overrides area()
```

***True or False***: Line (1) compiles as long as `Shape` otherwise looks like a perfectly normal class (valid destructor, no other errors).

---
### Q5 Answer
**False.**

- A class that declares (or inherits without overriding) **even one** pure virtual function is an **abstract base class**.
- We simply create objects of an abstract type, so `Shape s;` is a compile error.
- Line (1) attempts to create a `Shape` object - not possible because `Shape` is abstract.
- Line (2) is fine: we created a `Shape*` that *points to* a `Circle` — and pointers/references to an abstract type are exactly how you get polymorphism out of it.
- `Circle` is concrete only because it overrides *every* pure virtual it inherits. A class that overrides some but not all pure virtuals is still abstract.

---
### Q5 Demo: An Incomplete Override Stays Abstract

```c++
class Sphere : public Shape {     // forgets to override area()
public:
    double radius;
};

Sphere sph;   // ???
```

- Still a compile error — `Sphere` inherits `area() = 0` from `Shape` without overriding it, so `Sphere` is abstract too, exactly like `Shape` was.
- `Sphere` becomes concrete only once *it* supplies a real `area()`. Abstractness doesn't go away by adding a derived class — only by actually overriding every pure virtual.

---

### Q6: The `override` Specifier (True/False)

```c++
class Shape {
public:
    virtual double area() const = 0;
};
class Circle : public Shape {
public:
    Circle(double r) : radius_(r) {}
    double area() override  // note: no const!
    { return 3.14159 * radius_ * radius_; } 
private:
    double radius_;
};
```

***True or False***: This compiles — `override` is just documentation, and `Circle::area()` successfully overrides `Shape::area()`.

---
### Q6 Answer
**False — compile error.**

- `Shape::area()` is `const`; `Circle::area()` isn't. Different signatures ⟹ **not** an override, just an unrelated function that happens to share a name.
- Because we wrote `override`, the compiler checks our claim against every virtual in the base class, finds no match, and rejects the program right here, with a clear error pointing at `area()`.
- Had we left `override` off, this would **silently compile**. `Shape::area` would remain unimplemented, so `Shape` — and now `Circle` too — would still be abstract, and the real error wouldn't surface until something tried to instantiate a `Circle`, far away from the actual typo.

---
### Q6 Demo: Fixing the Signature

- remove const from `area()` function of `Circle`, but keep `override`
   - Compile and watch the error
- remove both `const` and `override` from `area()` function of `Circle`
   - Compile and watch a different error
- Keep `const` and remove `override`
   - Compile and watch a warning

---

### Q7: Default Arguments on a Virtual Function (True/False)

```c++
class Shape {
public:
    virtual void describe(double scale = 1.0) const {
        cout << "Shape scaled by " << scale;
    }
};
class Circle : public Shape {
public:
    void describe(double scale = 2.0) const override {
        cout << "Circle scaled by " << scale;
    }
};

Shape *sp = new Circle;
sp->describe();   // ???
```

***True or False***: This prints `Circle scaled by 2`.

---
### Q7 Answer
**False.** It prints `Circle scaled by 1`.

- Dynamic binding decides *which function body runs* — that part depends on `sp`'s dynamic type, so `Circle::describe` is what executes here.
- Default **argument** is filled in at the **call site**, based on the **static type** used to make the call (`Shape*`). Since `sp` is a `Shape*`, an omitted argument uses `Shape`'s default (`1.0`), even though `Circle`'s version is the one running.
- Best-practice: virtual functions with default arguments should use the same default in every class in the hierarchy.


---
### Q7 Demo: Forcing the Base Version

```c++
sp->describe();            // "Circle scaled by 1"  -- Circle's body, Shape's default
sp->Shape::describe();     // "Shape scaled by 1"    -- forced static binding
```

- The first call dynamically dispatches to `Circle::describe` — but since `sp`'s static type is `Shape*`, the missing argument is filled in with `Shape`'s default, `1.0`.
- The second call uses the scope operator to skip the virtual mechanism entirely: `Shape::describe` runs, and its own default (`1.0`) fills in too.

---
## Virtual Dispatch — Quick Reference

| The call... | is resolved... |
|---|---|
| directly on a named object (`obj.f()`) | at **compile time**, regardless of `virtual` |
| through a pointer/reference, `f` **not** `virtual` | at **compile time** — uses the *static* type |
| through a pointer/reference, `f` **is** `virtual` | at **run time** — uses the *dynamic* type |

