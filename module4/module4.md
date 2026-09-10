# Overloaded Operators
---

## Self-Assessment Questions
---
### Q1: Member vs. Non-Member (True/False)
```c++
ostream &operator<<(ostream &os, const Sales_data &item);
```

***True or False***: This operator could also have been written as a member function of `Sales_data`.

---
### Q1 Answer
**False.**

If `operator<<` were a member of `Sales_data`, the left-hand operand would have to *be* a `Sales_data`:

```c++
data << cout;   // if << were a member — backwards!
cout << data;   // what we actually want to write
```
We can't add members to <code>ostream</code> — it's a library class we don't own. So the only way
to get <code>cout &lt;&lt; data</code> to work is a nonmember function (usually a <code>friend</code>, since it
needs the private data).

---

### Q1 Takeaway

* Member operator ⟹ left operand is bound to `this` ⟹ left operand must be of the class type
* `<<` / `>>`'s left operand is a *stream*, not our class → member is impossible
* Rule: **IO operators must be nonmember functions**

---

### Q2: Symmetric Operators
```c++
string s = "world";
string u = "hi" + s;
```

Assuming `string` defines `operator+` as a **member function** instead of a nonmember, would the second line still compile?

---
### Q2 Answer
**No.**

- If <code>operator+</code> were a member, <code>"hi" + s</code> would be equivalent to
<code>"hi".operator+(s)</code>. 
- But <code>"hi"</code> has type <code>const char*</code> — a built-in type with no member functions at all. Compile error.

> Because <code>+</code> is a <b>nonmember</b>, <code>"hi" + s</code> is <code>operator+("hi", s)</code> — an ordinary function call where <b>either</b> argument can be converted to <code>string</code>.

---
### Q2 Takeaway

* **Symmetric operators** (arithmetic, relational, equality, bitwise) should usually be **nonmembers**
* This is what lets *either* operand be implicitly converted
* Member operators lock the left-hand operand to exactly the class type — no conversions allowed there

---

### Q3: Efficiency
Both versions compile and give identical results. Which one is more efficient, and why?

```c++
// Version A: + calls +=
Sales_data operator+(const Sales_data &lhs, const Sales_data &rhs) {
    Sales_data sum = lhs;   // copy lhs
    sum += rhs;             // reuse compound assignment
    return sum;
}

// Version B: += calls +
Sales_data& Sales_data::operator+=(const Sales_data &rhs) {
    *this = *this + rhs;    // reuse arithmetic operator
    return *this;
}
```

---

### Q3 Answer
**Version A.**

* <code>operator+=</code> only ever needs to modify <i>one</i> existing object — no extra temporaries.
   * Version A takes advantage of that: it copies <code>lhs</code> once
   * then lets the lean <code>+=</code> do the actual work in place.
* Version B does it backwards. 
   * Every call to <code>+=</code> now has to construct a whole new <code>Sales_data</code> by value (via <code>operator+</code>)
   * Then it copies its contents back into `*this`
   * one extra object built and destroyed for nothing, on every single compound assignment.
* [Code experiment](https://github.com/cse4208-wustl/operator_experiment)

---

### Q3 Takeaway

* If a class has both an arithmetic operator and its compound-assignment counterpart, implement the arithmetic operator **using** the compound-assignment one — not the other way around
* `+` built from `+=` → one temporary
* `+=` built from `+` → wasted temporary

---

### Q4: Consistency of `==` and `<`

Assuming `Sales_data::operator==` compares `isbn`, `units_sold`, *and* `revenue`, how would you design `operator<`?

```c++
Sales_data a("0-201", 10, 25.00);
Sales_data b("0-201", 20, 25.00);
// a != b (different units_sold), but same isbn
```
---

### Q4 Answer
You would not define `operator<` for such a class.

- If we define <code>&lt;</code> using only <code>isbn</code>, then <code>a</code> and <code>b</code> would compare as neither <code>&lt;</code> the other <b>and</b> not <code>==</code> either —
inconsistent. 
- If instead we chain isbn → units_sold → revenue, the ordering works, but it's
arbitrary: why units_sold before revenue rather than the reverse? There's no single logical answer, so it's best to not define `operator<` at all.

---
### Q4 Takeaway

* Rule: if `a != b`, then one of `a < b` or `b < a` must be true
* If there's no single logical ordering that satisfies this alongside `==`, **don't provide `<`**
* Only add relational operators when there's one unambiguous, natural ordering

---

### Q5: Subscript Operator (True/False)

```c++
class StrVec {
public:
    std::string& operator[](std::size_t n) { return elements[n]; }
private:
    std::string *elements;
};

const StrVec cvec = svec;
cvec[0] = "Zip";
```

***True or False***: This compiles and modifies `cvec`'s first element.

---
### Q5 Answer
**False — compile error.**

- Only <b>one</b> version of <code>operator[]</code> was defined, and it's non-<code>const</code>. 
- Calling it on a <code>const StrVec</code> isn't allowed — a <code>const</code> object can only call
<code>const</code> member functions. 
- We need a second, <code>const</code>-qualified overload that returns a reference to <code>const std::string</code> so read access on <code>const</code> objects still works (just not assignment).

---
### Q5 Takeaway

* `operator[]` **must** be a member (no choice here)
* Provide **two overloads**: plain reference (mutable objects) and `const` reference (`const` objects)
* This is the same const-overload pattern you'll see everywhere in the standard library

---

### Q6: Prefix vs. Postfix (True/False)

```c++
class StrBlobPtr {
public:
    StrBlobPtr& operator++();      // prefix
    StrBlobPtr  operator++(int);   // postfix
};
```

***True or False***: The `int` parameter in the postfix version is used by the implementation to know how many positions to advance.

---
### Q6 Answer
**False.**

- The <code>int</code> parameter is <b>never used</b> for anything functional. 
- Prefix and postfix <code>++</code> have the exact same name, and both take zero "real" operands (the operand is <code>this</code>) — normal overload resolution has no way to tell them apart. 
- The extra, unnamed <code>int</code> exists purely as a signal to the compiler: "this is the postfix
version." 
- The compiler automatically supplies <code>0</code> as the argument whenever you write <code>p++</code>.

> Also note: prefix returns a **reference** (`*this`), postfix returns a **copy** of the *old* value. Why can't postfix return a reference too?

---
## Design Checklist: Member or Nonmember?

| Must be members | Usually members | Usually nonmembers |
|---|---|---|
| `=` | Compound assignment (`+=`) | Arithmetic (`+`, `-`) |
| `[]` | `++` / `--` | Relational (`<`, `>`) |
| `()` | `*` / `->` | Equality (`==`, `!=`) |
| `->` | | Bitwise |

> Quick check for any operator you're about to overload: 
**does it need to change the object's own state, or could either operand plausibly be converted?** 
State-changing / type-specific → member. 
Symmetric / convertible → nonmember.
