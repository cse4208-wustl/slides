# Copy Control and Move Semantics
---

## Self-Assessment Questions

---
### Q1: The Copy Constructor

```c++
explicit Buffer(const string& name, size_t n);  // ctor
Buffer(const Buffer& rhs);                      // copy ctor
Buffer& operator=(const Buffer& rhs);           // copy assignment
void by_value(Buffer b);  void by_ref(const Buffer& b);

Buffer a("a", 3);
Buffer b = a;              // (A)
Buffer c(a);               // (B)
by_value(a);               // (C)
by_ref(a);                 // (D)
b = c;                     // (E)
Buffer arr[] = {a, b};     // (F)
Buffer e = {"e", 2};       // (G)
```

***Question***: Which lines call the copy constructor, which call copy assignment, and which don't compile? Why must the copy constructor's parameter be a reference?

---
### Q1 Answer
- **Copy constructor**: (A), (B), (C), (F — twice).
   - (A) `=` in a *declaration* is **copy initialization**, not assignment.
   - (B) direct initialization picks the best-matching ctor — still the copy ctor.
   - (C) passing to a **non-reference** parameter copy-initializes it.
   - (F) brace-initializing an array (or an aggregate's members) copies each element.
- **Nothing**: (D) — a reference binds; no new object.
- **Copy assignment**: (E) — `b` already exists.
- **Error**: (G) — copy-list-initialization can't use an **`explicit`** ctor. Also why copy ctors are usually **not** `explicit`: they are used implicitly everywhere.

---
### Q1 Answer (cont.)
- Copy is also used when **returning** a non-reference type and when **`push_back`/`insert`** puts an lvalue into a container (`emplace` constructs in place).
- Why a reference? If the parameter were `Buffer rhs`, calling the copy ctor would require copy-initializing `rhs`… which calls the copy ctor… which needs a copy… — **infinite recursion**. It is almost always `const Buffer&`.
- Returns from functions are often **elided** (the copy/move is skipped entirely), so don't rely on side effects in a copy ctor.

---
### Q2: Synthesized Copy Control and the Rule of Three

```c++
struct Shallow {
    int *p;
    explicit Shallow(int v) : p(new int(v)) {}
    ~Shallow() { delete p; }
};

void f() {
    Shallow s1(42);
    Shallow s2 = s1;          // (A)
    *s2.p = 7;
    cout << *s1.p << endl;    // (B)
}                             // (C)
```

***Question***: What does the compiler generate for `Shallow`? What happens at (A), (B), (C)?

---
### Q2 Answer
- If we don't define them, the compiler **synthesizes** a copy ctor, copy assignment, and destructor.
   - Copy ctor/assignment: **memberwise** copy (class members use their own copy; built-ins are copied bit-for-bit; arrays element by element).
   - Destructor: empty body; members are destroyed **after** the body, in reverse order of declaration.
- (A) copies the **pointer**, not the `int` → `s1.p == s2.p`.
- (B) prints `7` — the two objects alias.
- (C) both destructors `delete` the same `int` → **double delete** (undefined behavior). Destroying a built-in pointer member never calls `delete` — *our* destructor did.

---
### Q2 Answer (cont.)
- **Rule of Three**: a class that needs a destructor almost surely needs a copy constructor *and* copy assignment (and vice versa: if it needs copy, it needs assignment).
- With move semantics this becomes the **Rule of Five** (+ move ctor, move assignment).
- **Rule of Zero**: prefer members (`vector`, `string`, `unique_ptr`, `shared_ptr`) that already manage their resources — then the synthesized members are correct.

---
### Q3: `= default` and `= delete`

```c++
struct NoCopy {
    NoCopy() = default;
    NoCopy(const NoCopy&) = delete;
    NoCopy& operator=(const NoCopy&) = delete;
    ~NoCopy() = default;
};
struct OnlyCopy { OnlyCopy(const OnlyCopy&) {} };
struct HasUnique { unique_ptr<int> p; };

NoCopy n1, n2(n1);                    // (A)
OnlyCopy o;                           // (B)
HasUnique h1, h2(h1);                 // (C)
HasUnique h3(std::move(h1));          // (D)
```

***Question***: Which lines compile? What would happen if `~NoCopy()` were deleted?

---
### Q3 Answer
- (A) **Error** — `use of deleted function NoCopy(const NoCopy&)`. A deleted function is *declared* (it takes part in overload resolution) but may not be *used*.
- (B) **Error** — declaring **any** constructor (even the copy ctor) stops the compiler from synthesizing the default ctor. Fix: `OnlyCopy() = default;`.
- (C) **Error** — the synthesized copy ctor is **implicitly deleted** because a member (`unique_ptr`) can't be copied.
- (D) **OK** — the synthesized move ctor moves `p`.

---
### Q3 Answer (cont.)
- Deleted destructor: we couldn't define variables or temporaries of that type, or `delete` a dynamically allocated one — **never delete the destructor**.
- `= default` asks for the synthesized version; `= delete` may be used on **any** function, `= default` only on members the compiler can synthesize.
- Before C++11: declare the copy members `private` and never define them → errors only at **link** time for members/friends. `= delete` reports at **compile** time.
- `unique_ptr` is the library example: copy ctor and copy assignment are `= delete`.

---
### Q4: Swap and Copy-and-Swap

```c++
class Buffer {
    string name_; size_t n_; int *data_;
public:
    Buffer(const Buffer& rhs);  // DEEP copy: new int[n_] + copy elements
    ~Buffer() { delete[] data_; }
    friend void swap(Buffer& a, Buffer& b) noexcept {
        using std::swap;
        swap(a.name_, b.name_); swap(a.n_, b.n_); swap(a.data_, b.data_);
    }
    Buffer& operator=(Buffer rhs) {     // by VALUE
        swap(*this, rhs);
        return *this;
    }
};
```

***Question***: Why define our own `swap`? Why is `rhs` passed by value?

---
### Q4 Answer
- Reordering algorithms (`sort`, `reverse`, …) swap elements. Without our own, `std::swap(a, b)` does `Buffer tmp = a; a = b; b = tmp;` → here that is **3 deep copies** (the copy ctor, plus a copy-constructed `rhs` in each assignment): 3 `new int[]`s + element copies just to exchange two pointers. (With move operations it's 3 moves: cheaper, still more work.)
- Our `swap` just exchanges **pointers**: no allocation, can't throw → `noexcept`.

---
### Q4 Answer (cont.): Why is `rhs` passed by value?
- `swap` **exchanges** contents: after `swap(*this, rhs)`, whatever `rhs` refers to holds `*this`'s **old** state.
   - `operator=(Buffer& rhs)`: `x = y` would hand `x`'s old contents to `y`. Assignment would **modify its right-hand operand**! (And `x = Buffer("t", 1)` wouldn't compile: a temporary can't bind to `Buffer&`.)
   - `operator=(const Buffer& rhs)`: can't swap with it at all (it's `const`).
- By value, `rhs` is our own private **copy**: the caller's object is untouched, and `rhs` destroys the old state at the `}`.
- No move ctor is synthesized (we declared a copy ctor and dtor) → `rhs` is **copied** even from an rvalue. Add `Buffer(Buffer&&) noexcept` → rvalues are **moved**, and this one operator does both copy- and move-assignment.

---
### Q4 Answer (cont.)
- **Valuelike** classes (e.g., `Buffer`, `string`): each object owns its *own* copy of the resource → deep copy in copy ctor/assignment, free in destructor.
- **Pointerlike** classes: copies *share* the resource → copy the pointer and keep a **reference count** (like `shared_ptr`); destroy the resource when the count reaches `0`.
- Pointerlike classes may instead be **move-only**: a single owner hands off ownership (`unique_ptr`, `thread`).

---
### Q5: Lvalues, Rvalues, and `std::move`

```c++
int i = 42;
int& r = i;                     // (A)
int& r2 = i * 42;               // (B)
const int& r3 = i * 42;         // (C)
int&& rr = i * 42;              // (D)
int&& rr2 = i;                  // (E)
int&& rr3 = rr;                 // (F)
int&& rr4 = std::move(i);       // (G)

void which(const Buffer& b);    // #1
void which(Buffer&& b);         // #2
Buffer a("a", 1);
which(a);  which(Buffer("t", 1));  which(std::move(a));  // (H)
```

***Question***: Which lines compile? Which `which` is called each time in (H)? What is `a` afterwards?

---
### Q5 Answer
- **Lvalue**: persistent — an object's identity (variables, `*p`, `++i`, `a[n]`). **Rvalue**: ephemeral — a value (literals, `i * 42`, `i++`, non-reference returns, temporaries).
- (A) OK. (B) **Error** — a non-`const` `&` can't bind an rvalue. (C) OK — a `const` `&` binds to anything.
- (D) OK — `&&` binds **only** to an object about to be destroyed.
- (E) **Error** — `i` is an lvalue.
- (F) **Error** — surprising: `rr` has rvalue-reference *type*, but a **variable is an lvalue**.
- (G) OK — `std::move` casts an lvalue to an rvalue reference.

---
### Q5 Answer (cont.)
- (H) `#1`, `#2`, `#2` — overload resolution prefers `&&` for rvalues.
- `a` is **unchanged**: `std::move` doesn't move anything — it's just a cast (`static_cast<Buffer&&>`). Moving happens only if someone *takes* the rvalue (a move ctor/assignment).
- After `std::move(x)` we promise to only **assign to** or **destroy** `x` — never rely on its value.

---
### Q6: Move Constructor and Move Assignment

```c++
Buffer(Buffer&& rhs) noexcept
    : name_(std::move(rhs.name_)), n_(rhs.n_), data_(rhs.data_) {
    rhs.data_ = nullptr;
    rhs.n_ = 0;
}
Buffer& operator=(Buffer&& rhs) noexcept {
    if (this != &rhs) {
        delete[] data_;
        data_ = rhs.data_;  n_ = rhs.n_;  name_ = std::move(rhs.name_);
        rhs.data_ = nullptr;  rhs.n_ = 0;
    }
    return *this;
}
```

***Question***: What breaks without `rhs.data_ = nullptr`? Why `noexcept`? Why test for self-assignment?

---
### Q6 Answer
- Moving **steals** the resource: no allocation, no element copies.
- Without `rhs.data_ = nullptr`, the moved-from object's destructor would `delete[]` the array the new object now owns → **double delete**. A moved-from object **must be destructible** and *valid* (assignable), but its value is unspecified.
- `noexcept`: `vector` guarantees that a failed reallocation leaves it unchanged. It can only move the old elements if the move **can't throw**; otherwise it **copies** them. Without `noexcept`, growing a `vector<Buffer>` deep-copies every element.
- Self-assignment check: the rvalue may come from `std::move(*this)` — don't free what we're about to take.
