## Live Demo

Example code/scenarios in `demo.cpp` (`make run`).

1. Pointers to data members — roster table built from a vector of "column" pointers (including private members of base and derived via static functions)
2. Pointers to member functions — `.*` / `->*`, type alias, virtual dispatch
3. Member function table — `Cursor::move`
4. Callable objects — `function`, `mem_fn`, `bind`

## Live Changes

- **Break access control (Q5):** in `main`, replace `Student::idMember()` with `&Student::id_` → "`id_` is private" error. The static accessor is the *only* way out.
- **Write through a `const` member pointer (Q1/Q5):** add `roster[0].*columns[0] = "X";` → assignment to read-only error. Then change `columns`' element type to `string Student::*` → now the private-member accessors fail to convert (they return `const string ...::*`).
- **Wrong conversion direction (Q4):** declare `string Person::*bad = &Student::major;` → error; `string Student::*ok = &Person::last;` compiles.
- **Remove the parentheses (Q3):** change `(prof.*pd)()` to `prof.*pd()` and read the compiler's "must use `.*` or `->*`" message.
- **Drop the `&` (Q2):** `Describe pd = Person::describe;` → "invalid use of non-static member function".
- **`auto` on an overload (Q2):** add an overload `string greet() const;` to `Person` → `auto pg = &Person::greet;` stops compiling; fix with an explicit type.
- **Make `describe` non-virtual (Q4):** remove `virtual` (and `override`) → `(ref.*pd)()` now prints `Person  Grace Hopper`.
- **Reorder the table:** swap `&Cursor::home` and `&Cursor::back` in `Menu` → `move(HOME)` now moves back; the enum/table link is only by position.
- **Uncomment the `find_if(..., &string::empty)` line (Q6)** and show the error from inside `<algorithm>`.
- **`function` with pointers:** change `words` to `vector<string*>` → `fcn` must become `function<bool (const string*)>`, while the `mem_fn` and `bind` versions still work unchanged.
