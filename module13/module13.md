# Template Argument Deduction
---

## Self-Assessment Questions

---
### Q1: Conversions and Template Type Parameters

```c++
template <typename T> void fobj(T, T);                  // copies
template <typename T> void fref(const T&, const T&);    // references
template <typename T> int compare(const T&, const T&);

string s1("a value");  const string s2("another value");
int a[10], b[42];  long lng = 7;  ofstream out("out.txt");

fobj(s1, s2);              // (A)
fref(s1, s2);              // (B)
fobj(a, b);                // (C)
fref(a, b);                // (D)
compare(lng, 1024);        // (E)
compare("hi", "world");    // (F)
compare("bye", "dad");     // (G)
fref(out, cout);           // (H)
```

***Question***: Which calls compile? What is `T` for each one that does?

---
### Q1 Answer

```c++
fobj(s1, s2);              // (A) ok: T = string -- s2's top-level const ignored
fref(s1, s2);              // (B) ok: T = string -- s1 gets a const conversion
fobj(a, b);                // (C) ok: T = int*   -- arrays decay to pointers
fref(a, b);                // (D) error: int[10] vs. int[42] (no decay for &)
compare(lng, 1024);        // (E) error: T = long vs. T = int
compare("hi", "world");    // (F) error: T = char[3] vs. T = char[6]
compare("bye", "dad");     // (G) ok: T = char[4] ... but see next slide
fref(out, cout);           // (H) error: T = ofstream vs. T = ostream
```

---
### Q1 Answer (cont.)
- **Template argument deduction**: the compiler uses the types of the call's **arguments** to find the template arguments.
- Instead of converting arguments, the compiler **generates a new instantiation**. Only a few conversions are applied:
   - **Top-level `const`** is ignored (in the parameter and in the argument).
   - **`const` conversion**: a reference/pointer to `const` parameter accepts a reference/pointer to non-`const`.
   - **Array-to-pointer** and **function-to-pointer**, but only if the parameter is **not a reference**.
- **Not** performed: arithmetic conversions, derived-to-base (`ofstream` → `ostream`), user-defined conversions.

---
### Q1 Answer (cont.)
- Parameters that use the **same** template type must deduce the **same** type → (E), (F), (H) fail.
- (G) compiles because both literals happen to be `const char[4]`, but `v1 < v2` compares two **pointers** to unrelated arrays. The result has nothing to do with alphabetical order.
- Want different types? Use **two type parameters**:

```c++
template <typename A, typename B>
int flexibleCompare(const A &v1, const B &v2);   // A = long, B = int
flexibleCompare(lng, 1024);                      // ok: < does the normal conversions
```

- Parameters with an **ordinary** (non-template) type get **all normal conversions**:

```c++
template <typename T> ostream &print(ostream &os, const T &obj);
print(out, 42);     // ok: ofstream& -> ostream& (os is not a template type)
```

---
### Q2: Explicit Template Arguments

```c++
template <typename T1, typename T2, typename T3> T1 some_f(T2, T3);
template <typename T1, typename T2, typename T3> T3 other_f(T1, T2);
template <typename T> int compare(const T&, const T&);

int i = 1;  long lng = 2;
auto a = some_f<long long>(i, lng);               // (A)
auto b = other_f<long long>(i, lng);              // (B)
auto c = other_f<int, long, long long>(i, lng);   // (C)
compare<long>(lng, 1024);                         // (D)
compare<int>(lng, 1024);                          // (E)
```

***Question***: When would you want to give explicit template arguments? Which signature, `some_f` or `other_f`, is preferred, and why? Which calls compile?

---
### Q2 Answer

```c++
auto a = some_f<long long>(i, lng);               // (A) ok: T1 = long long (given),
                                                  //     T2 = int, T3 = long (deduced)
auto b = other_f<long long>(i, lng);              // (B) error: T1 = long long (given),
                                                  //     T2 = long, T3 = ??? (can't deduce)
auto c = other_f<int, long, long long>(i, lng);   // (C) ok: all three given
compare<long>(lng, 1024);                         // (D) ok: compare(const long&, const long&)
compare<int>(lng, 1024);                          // (E) ok: compare(const int&, const int&)
```

- Explicit arguments are matched to template parameters **left to right**.
- Only the **rightmost** ones can be omitted, and only if they can be deduced.
- **`some_f` is preferred**: the type the caller must give (`T1`, the return type) comes **first**. Callers write one type, not three.

---
### Q2 Answer (cont.): When to Use Explicit Arguments
- A template parameter appears **only in the return type**, so there is nothing to deduce it from:
   - `sum<long long>(i, lng)`: the caller picks a result type large enough.
   - `make_shared<T>(...)`, `make_unique<T>(...)`: the arguments are for `T`'s **constructor**, not the type `T` itself.
- The arguments **disagree** and you want to pick: `max<double>(1, 2.5)` (`std::max` has one type parameter).
- To **force a conversion**: `compare<string>("bye", "dad")` compares strings, not pointers.
- To choose an instantiation when nothing else can (function pointers, Q4).

---
### Q2 Answer (cont.): Normal Conversions Are Back
- Once a template parameter is **explicitly specified**, its arguments get **normal conversions**, as for an ordinary function:
   - (D) `1024` is converted `int` → `long`.
   - (E) `lng` is converted `long` → `int`: this may silently narrow.
- The caller now controls the conversions. If the type is too small, the result is wrong:

```c++
int big = INT_MAX;
sum<long long>(big, 1L);   // 2147483648
sum<short>(big, 1L);       // 0 -- the caller asked for it
```

---
### Q3: Trailing Return Types and Type Transformations

```c++
template <typename It>
decltype(*beg) fcn0(It beg, It end) { return *beg; }                        // (A)

template <typename It>
auto fcn(It beg, It end) -> decltype(*beg) { return *beg; }                 // (B)

template <typename It>
auto fcn2(It beg, It end) -> remove_reference<decltype(*beg)>::type         // (C)
{ return *beg; }
```

***Question***: Which definitions are legal? For `vector<int>` iterators, what does each return? Can you assign to the result?

---
### Q3 Answer

```c++
template <typename It>
decltype(*beg) fcn0(It beg, It end);              // (A) error: beg not declared yet

template <typename It>
auto fcn(It beg, It end) -> decltype(*beg);       // (B) int& -- assignable

template <typename It>
auto fcn2(It beg, It end)                         // (C) needs typename (before C++20)
    -> typename remove_reference<decltype(*beg)>::type;   // int -- a copy
```

```c++
vector<int> vi = {1, 2, 3};
fcn(vi.begin(), vi.end()) = 100;    // ok: changes vi[0]
fcn2(vi.begin(), vi.end()) = 200;   // error: can't assign to a temporary
```

---
### Q3 Answer (cont.)
- A **trailing return type** comes **after the parameter list**, so it can use the parameters: `decltype(*beg)`.
- `*beg` is an **lvalue**, so `decltype(*beg)` is a **reference**: `int&`, `string&`, etc.
- To return **by value**, strip the reference: `remove_reference<decltype(*beg)>::type`.
   - `::type` depends on a template parameter, so we need `typename` before C++20.
   - C++14 shorthand: `remove_reference_t<decltype(*beg)>`.

---
### Q3 Answer (cont.): `<type_traits>`

<style scoped>
table { font-size: 0.75em; }
table th, table td { padding: 3px 10px; }
</style>

| `Mod<T>::type` | Example | Result |
|---|---|---|
| `remove_reference` | `remove_reference<const string&&>` | `const string` |
| `add_const` | `add_const<int>` / `add_const<int&>` | `const int` / `int&` |
| `add_lvalue_reference` | `add_lvalue_reference<int&&>` | `int&` |
| `remove_pointer` | `remove_pointer<const char*>` | `const char` |
| `make_unsigned` | `make_unsigned<long>` | `unsigned long` |
| `remove_extent` | `remove_extent<int[3][4]>` | `int[4]` |
| `remove_all_extents` | `remove_all_extents<int[3][4]>` | `int` |

- If no transformation applies, `type` is just `T`.
- **Checking the result:** `typeid` ignores references and top-level `const`, so `typeid(int&) == typeid(int)`.

---
### Q4: Function Pointers and Argument Deduction

```c++
template <typename T> int compare(const T&, const T&);

int (*pf1)(const int&, const int&) = compare;      // (A)

void func(int (*)(const string&, const string&));  // overloaded func:
void func(int (*)(const int&, const int&));        // two pointer types

func(compare);           // (B)
func(compare<int>);      // (C)
auto pf2 = compare;      // (D)
```

***Question***: Which lines compile? Where a line compiles, what is `T`?

---
### Q4 Answer

```c++
int (*pf1)(const int&, const int&) = compare;      // (A) ok: T = int, from pf1's type

func(compare);           // (B) error: T = string or T = int? ambiguous
func(compare<int>);      // (C) ok: calls func(int (*)(const int&, const int&))
auto pf2 = compare;      // (D) error: no type to deduce T from
```

- When we take the address of a function template, the template arguments are deduced from the **function pointer type** we initialize or assign.
- The context must determine a **unique** type for every template parameter:
   - (B) Both `func` overloads accept *some* instantiation of `compare`.
   - (D) `auto` gives the compiler nothing to deduce from.
- **Fix**: name the instantiation with an explicit argument: `compare<int>`, `auto pf2 = compare<double>;`.

---
### Q5: Deduction and References

```c++
template <typename T> void f1(T&);
template <typename T> void f2(const T&);
template <typename T> void f3(T&&);

int i = 0;  const int ci = i;

f1(i);   f1(ci);   f1(5);                        // (A) (B) (C)
f2(i);   f2(ci);   f2(5);                        // (D) (E) (F)
f3(42);  f3(i);  f3(ci);  f3(i * ci);  f3(i = ci); // (G) (H) (I) (J) (K)
```

***Question***: For each call, what is `T`, and what is the type of the function parameter? Which calls don't compile?

---
### Q5 Answer

<style scoped>
table { font-size: 0.75em; }
table th, table td { padding: 2px 10px; }
</style>

| | Call | Argument | `T` | Parameter |
|---|---|---|---|---|
| (A) | `f1(i)` | lvalue `int` | `int` | `int&` |
| (B) | `f1(ci)` | lvalue `const int` | `const int` | `const int&` |
| (C) | `f1(5)` | rvalue | **error** | can't bind `T&` to an rvalue |
| (D)–(F) | `f2(i)`, `f2(ci)`, `f2(5)` | anything | `int` | `const int&` |
| (G) | `f3(42)` | rvalue `int` | `int` | `int&&` |
| (H) | `f3(i)` | lvalue `int` | **`int&`** | `int& &&` → `int&` |
| (I) | `f3(ci)` | lvalue `const int` | **`const int&`** | `const int&` |
| (J) | `f3(i * ci)` | rvalue `int` | `int` | `int&&` |
| (K) | `f3(i = ci)` | lvalue (`=` returns `i`) | **`int&`** | `int&` |

---
### Q5 Answer (cont.)
- `T&`: normal binding rules, so **lvalues only**. `const` in the argument becomes part of `T` (it is low-level).
- `const T&`: binds to **any** argument: a non-`const` lvalue, a `const` lvalue, or an rvalue (D)–(F).
   - `f2(ci)`: the argument is `const int`, and the parameter `const T&` already supplies the `const`. So `T = int`, and the parameter is `const int&`.
   - Compare with `f1(ci)`: `T&` has no `const` of its own, so the `const` has to go into `T` (`T = const int`).

---
### Q5 Answer (cont.): `T&&` Parameters
- `f3(42)`: an **rvalue** → `T = int`, parameter `int&&`. Nothing new.
- `f3(i)`: an **lvalue** → special rule: `T = int&`, so the parameter is `int& &&`.
- A reference to a reference **collapses**. If either one is `&`, the result is `&`:
   - `int& &&` → `int&`. The parameter is an ordinary lvalue reference bound to `i`.
- So a `T&&` parameter accepts **any** argument, and `T` records which kind it was:
   - rvalue → `T = int`
   - lvalue → `T = int&`
- This is exactly what `std::move` needs (next slide): accept anything, and then use `remove_reference<T>::type` to get back the plain `int`.

---
## How `std::move` Works

```c++
template <typename T>
typename remove_reference<T>::type&& move(T&& t) {
    return static_cast<typename remove_reference<T>::type&&>(t);
}
```

- `move(string("bye"))`: `T = string` → returns `string&&`
- `move(s1)`: `T = string&` → parameter `string&`, returns `string&&`
   - `static_cast` may turn an lvalue into an rvalue reference
   - `s1` may now be **moved from**: don't use it until reassigned
