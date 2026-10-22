---
marp: true
theme: default
paginate: false
size: 16:9
style: |
  section {
    font-size: 28px;
  }
  h1 {
    color: #1a3c6e;
  }
  h2 {
    color: #1a3c6e;
  }
  table {
    font-size: 24px;
  }
---

# CSE 428S: Multi-Paradigm Programming in C++
### Overloading and Templates

Department of Computer Science & Engineering
Washington University in St. Louis

Reading: C++ Primer, Section 16.3

---

## Overloading: A Quick Refresher

- Functions with the **same name** must differ in the **number or types** of their parameters
- Overloads can be non-member functions, member functions, or operators
  - A member `operator<` and a non-member `operator<` can both match the same call
- The compiler picks the **best match** by ranking the conversions each call needs (Section 6.4)
- If no single overload is best, the call is **ambiguous**
  - Usually an error, though some compilers only warn when they can still pick one

---

## Templates Join the Overload Set

Function templates can be overloaded by other templates **and** by ordinary functions.

- **Candidates** include every template instantiation for which argument deduction succeeds
- Template candidates are **always viable**: deduction already threw out the ones that don't fit
- All viable functions, template or not, are **ranked by conversions**
- Templates allow only a few conversions (Section 16.2.1), so they often need an **exact match**

---

## Breaking a Tie

If exactly one function is the best match, it wins. If several match **equally well**:

1. If exactly **one non-template** is among them, the non-template is called
2. If they're **all templates**, the **most specialized** template is called
3. Otherwise, the call is **ambiguous**

The rest of the lecture looks at each of these rules in turn.

---

## Rule: Non-Templates Win Ties

Overload set: `debug_rep(const T&)` and `debug_rep(const string&)`

- Calling `debug_rep(s)` with a `string` gives two **exact** matches
- The **non-template** version is chosen

This preference applies **only to ties**:

- If the template is an **exact** match and the non-template needs a **conversion** (for example, through a constructor), the template is the better match, and the **template wins**

---

## Rule: More Specialized Templates Win Ties

Overload set: `debug_rep(const T&)` and `debug_rep(T*)`

- `const T&` accepts **anything**, including pointers
- `T*` accepts **only pointers**, so it is **more specialized**
- Passing a `const string*` gives two exact matches, and `T*` is chosen

Without this rule, every call with a pointer to `const` would be **ambiguous**.

---

## Conversions Can Surprise You

Calling `debug_rep("hi world!")` with three overloads in play:

| Overload | Match |
|---|---|
| `debug_rep(const T&)` with `T = char[10]` | exact |
| `debug_rep(T*)` with `T = const char` | exact (array-to-pointer counts as exact) |
| `debug_rep(const string&)` | needs a **user-defined** conversion |

- The two templates tie, and `T*` is more specialized, so **`T*` wins**
- The `string` version you might expect is never called
- To treat C-style strings as strings, add **non-template** `char*` and `const char*` overloads

---

## Missing Declarations Can Misbehave

- Normally, calling a function that hasn't been declared is a **compile error**
- Not when a template with that name is visible: the compiler **silently instantiates the template** instead
- The program compiles and runs, but calls a **different function** than you intended

**Tip:** declare every function in an overload set **before** defining any of them.

---

## Scope and Explicit Choices

- Only overloads **visible at the call** are candidates
- Namespaces and the scope operator (`::`) let you pick a specific overload or avoid ambiguity
- **Explicit template arguments** (`name<Type>(...)`) skip the non-template and force the template version

---

## Predicting Which Overload Is Called

1. **Which functions are visible?** Non-templates, plus templates whose deduction succeeds
2. **What conversions does each need?** Exact matches beat conversions; user-defined conversions rank last
3. **Is there a tie?** Prefer the non-template, then the most specialized template

When the result surprises you, one of these three steps is usually why.

---

## Studio 14

- Member vs. non-member `operator<`: what happens when **both** exist?
- A function template vs. a non-template function with the **same name**
- Adding a more specialized **pointer** template
- C-style strings, `std::string`, and a **missing declaration**

Predict which function is called **before** you compile, then check your prediction against the output.
