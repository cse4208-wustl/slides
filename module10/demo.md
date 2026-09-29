## Live Demo

Example code/scenarios in `demo.cpp` (`make run`). Every `Widget` prints `+` when constructed and `-` when destroyed — watch for missing `-` lines (leaks) and early `-` lines (dangling pointers).

1. Raw `new`/`delete` — leak, dangling pointer, double delete (Q1)
2. `shared_ptr` reference counting — `factory`, copies, `r = q`, a `vector` of copies, `reset` (Q2)
3. Mixing raw and smart pointers — `process(shared_ptr<Widget>(x))`, `get()` (Q3/Q4)
4. `unique_ptr` — `release`/`reset`, returning from a function (up the stack), `std::move` into a function (down the stack) (Q5)
5. `weak_ptr` — `lock`/`expired`, and breaking a `shared_ptr` cycle (Q6)

Extra targets: `make asan` (AddressSanitizer), `make leaks` (macOS leak checker).

## Live Changes

- **Spot the leak (Q1):** run as is and point out that `- Widget 1` never prints. Then `make leaks` (macOS) or `make asan` (Linux) → "1 leak" / "Direct leak of 4 bytes". Fix with `delete a;` before `a = new Widget(3);`.
- **Use after delete (Q1):** uncomment `c->id` in `raw_pointers`. With `make run` it may *print a plausible value* — that's the danger of UB. With `make asan` → `heap-use-after-free`.
- **Double delete (Q1):** uncomment `delete c;` → `make run` usually aborts (`pointer being freed was not allocated` / `double free detected`); `make asan` → `attempting double-free`.
- **Watch the count (Q2):** change `process` to take `const shared_ptr<Widget>&` → `use_count` inside `process` drops from 2 to 1 (no copy). Change `vector<shared_ptr<Widget>> v(3, p)` to `v(100, p)`.
- **Forgetting to erase:** move the `vector` out of its inner block → `vector destroyed` line now still shows 6, and `Widget 10` lives until the end of the function.
- **Implicit conversion (Q3):** uncomment `process(x);` → `could not convert 'x' from 'Widget*'`. Also try `shared_ptr<Widget> bad = new Widget(99);` → error; `shared_ptr<Widget> ok(new Widget(99));` compiles.
- **Dangling after a temporary (Q4):** uncomment `x->id` after `process(shared_ptr<Widget>(x))` → the `- Widget 21` line appears *before* we use `x`. `make asan` → heap-use-after-free.
- **Two control blocks (Q4):** uncomment `process(shared_ptr<Widget>(sp.get()));` → `- Widget 20` prints **twice** (and crashes / ASan double-free).
- **`delete` the result of `get()` (Q4):** uncomment `delete sp.get();` → same double-delete.
- **Copying a `unique_ptr` (Q5):** uncomment `unique_ptr<Widget> p2(p1);` → read the "use of deleted function" error. Then change `consume(std::move(p2))` to `consume(p2)` → same error.
- **`release()` leak (Q5):** uncomment `p4.release();` → `- Widget 32` disappears; `make leaks` / `make asan` report it.
- **`unique_ptr` observed after move (Q5):** after `consume(std::move(p2));` add `cout << p2->id;` → null dereference (crash).
- **Break the cycle (Q6):** in `Node`, change `weak_ptr<Node> prev;` to `shared_ptr<Node> prev;` → `a.use_count` becomes 2 and neither `- Node A` nor `- Node B` prints: a leak with no raw pointer in sight.
- **Dereference a `weak_ptr` (Q6):** uncomment `wp->id` → no `operator->` for `weak_ptr`.
- **Hold the lock:** in `weak_pointers`, declare `shared_ptr<Widget> keep;` before the inner block and set `keep = wp.lock();` inside → `Widget 40` survives the block; `expired` stays `false`.
