# Module 7 Demo — Function Pointers
## What to Try, Question by Question

**Q1 — Function type vs. pointer to function**
- Declare `decltype(lengthCompare) Func;` and try `Func = lengthCompare;` — compile error, `Func` isn't a variable.
- Declare `decltype(lengthCompare) *FuncP;`, try the same assignment, then call `FuncP("hi", "there")` — compiles and runs fine.
- In `demo.cpp`, add `using PlainFunc = decltype(lengthCompare);` then try `PlainFunc pf2 = lengthCompare;` — compile error, `PlainFunc` names a function type, not a pointer. Fix it by changing the alias to `using PlainFunc = decltype(lengthCompare) *;` and rerun.
- Bonus: show that `PlainFunc` (no `*`) *can* still declare a parameter (`void f(PlainFunc cmp)`), even though it can't declare a variable — parameters of function type auto-convert to pointers.

**Q2 — Why the parentheses matter**
- Declare `pf` as `bool (*pf)(const string&, const string&);`, then try `pf = 0;` — compiles fine.
- Declare `pf` as `bool *pf(const string&, const string&);` instead, try the same `pf = 0;` — compile error. Ask the class to predict the error message first.
- In `demo.cpp`, above `main`, temporarily add `bool *pf(const string&, const string&);` (no parentheses) right next to the `CompareFn pf = lengthCompare;` line and try to assign to it — show the compile error live, then remove it.

**Q3 — Calling through a function pointer**
- Change `pf = lengthCompare;` to `pf = &lengthCompare;` — nothing changes.
- Change the call site from `pf("hi","there")` to `(*pf)("hi","there")` and back — same output either way.
- Ask: "if this always needed `&` and `*` like a normal pointer, would this feel more or less intuitive?"

**Q4 — Pointers to overloaded functions**
- Add `void ff(int);` as a third overload to the board example; recompile the line that previously failed — it now succeeds, binding to the new overload.
- In `demo.cpp`, add a second overload, e.g. `bool lengthCompare(int, int);`, then declare `bool (*pf3)(int, int) = lengthCompare;` — show it binds to the *new* overload, not the string one.

**Q5 — Type aliases and `decltype`**
- Rewrite `useBigger`'s parameter using `FuncP`, then again using `FuncP2` — both compile identically.
- Call `useBigger(s1, s2, lengthCompare)` — the bare function name auto-converts either way.
- Try the plain function-type spelling (`bool pf(const string&, const string&)`, no `*`, no alias) to show it's accepted too.
- In `demo.cpp`, rewrite `useComparator`'s parameter by hand as `bool (*cmp)(const string&, const string&)` instead of `CompareFn cmp` — same behavior, more typing.

**Q6 — Returning a pointer to function**
- Have a student try `decltype(sumLength) getFcn(const string &);` (no `*`) first, and read the actual compiler error out loud.
- Fix it by adding the `*`; rewrite using the trailing-return form and confirm it behaves identically.
- In `demo.cpp`, change `auto getComparator(bool) -> CompareFn` to `auto getComparator(bool) -> decltype(lengthCompare)` (drop the `*`) and watch it fail to compile; put the `*` back to fix it.

**Wrap-up**
- Try assigning a function with a mismatched parameter or return type to `CompareFn` and read the compiler's error message together — ties back to Q4's "must match exactly" rule.
