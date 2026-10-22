# Module 14 Reading Self-Assessment Questions

### Ranking Conversions

| Rank | Kind | Example |
|---|---|---|
| 1 | **Exact match** | same type; array → pointer; top-level `const` |
| 2 | **Const conversion** | `int*` → `const int*` |
| 3 | **Promotion** | `char` → `int`, `float` → `double` |
| 4 | **Arithmetic / pointer conversion** | `char` → `long`, `int` → `double`; `Derived*` → `Base*` |
| 5 | **Class-type conversion** | `const char*` → `std::string` |

### Q1: Overloaded Functions

```c++
void f(int x);                   // (1)
void f(double x);                // (2)
void f(long x);                  // (3)
void f(const char* s);           // (4)
void f(const std::string& s);    // (5)

f('a');
```

***Question***: Which overloads are viable? Which one runs?

### Q2: Overloaded Templates

```c++
template <typename T> void g(const T& t);   // (A)
template <typename T> void g(T* p);         // (B)

int x = 42;
int* p = &x;

g(p);
```

***Question***: Which templates are viable? What is `T` in each? Which is called?

### Q3: Templates and a Non-Template

```c++
template <typename T> void g(const T& t);   // (A)
template <typename T> void g(T* p);         // (B)
void g(std::string* p);                      // (C) non-template

std::string s = "hi";
std::string* sp = &s;
int i = 7;
const int* cip = &i;

g(sp);    // call 1
g(i);     // call 2
g(cip);   // call 3
```

***Question***: Which function does each call resolve to?
