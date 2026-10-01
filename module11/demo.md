## Live Demo

Example code/scenarios in `demo.cpp` (`make run`). `Buffer` owns a dynamic `int` array and prints every copy-control call: `ctor`, `COPY ctor`, `COPY assign`, `MOVE ctor`, `MOVE assign`, `swap`, `dtor`. Copies get a `-copy` suffix in their name; moved-from objects are renamed `moved-from` — so you can see who owns what.

1. `swap` and copy-and-swap (Q4)
2. Lvalues, rvalues, `std::move` — overloaded `which(const Buffer&)` / `which(Buffer&&)` (Q5)
3. Move constructor and move assignment; `push_back` lvalue vs. rvalue (Q6)
4. `noexcept` and `vector` reallocation — counts copies vs. moves for 17 `emplace_back`s (Q6)
5. Move-only types: `unique_ptr` (Q6)

The demo is plain C++: every live change below is a small edit (comment out a block, delete a word).
Extra targets: `make asan` (AddressSanitizer), `make noelide` (turn off copy elision), `make leaks` (macOS).

## Live Changes

- **Remove our `swap` (Q4):** comment out the `friend void swap(...)` function in `Buffer` → `swap(x, y)` now uses `std::swap` and prints `MOVE ctor`, `MOVE assign`, `MOVE assign`, `dtor`. Also comment out the move constructor and move assignment → three deep copies of a 1000-element array.
- **Copy-and-swap (Q4):** comment out both `operator=` (copy and move) and uncomment the `operator=(Buffer rhs)` block below them → `x = y` prints `COPY ctor` (the parameter), `swap`, `dtor` (old state destroyed). In section 3, `c = std::move(b)` now prints `MOVE ctor` + `swap` — one operator does both jobs. `x = x` stays correct. (Keep our `swap`: with `std::swap`, `operator=` and `swap` would call each other forever.)
- **Broken self-assignment (Q4):** in the copy assignment, move `delete[] data_;` to the **top** (free first, then allocate `newdata` and copy from `rhs.data_`) → still fine for `x = y`, but `x = x` copies from the array we just freed; `make asan` → `heap-use-after-free`.
- **Reference binding (Q5):** uncomment `(A)`, `(B)`, `(C)` one at a time. `(C)` surprises people: `rr` is an rvalue reference but a **variable** — an lvalue.
- **`std::move` is only a cast (Q5):** note `which(std::move(a))` prints nothing from `Buffer` and `a` keeps its name. Then change `which(Buffer&& b)` body to `Buffer stolen = std::move(b);` → now `a` really is `moved-from`.
- **The move hidden in `return` (Q6):** in section 2, `which(make_buffer("ret"))` prints only one `ctor`: the copy/move out of `return local;` was elided. `make noelide` → a `MOVE ctor` + `dtor [moved-from]` appear.
- **Remove the move operations (Q6):** comment out the move constructor and move assignment → every `std::move` in section 3 silently becomes a `COPY ctor` / `COPY assign` (the 1,000,000-element buffer is deep-copied), and section 4 reports 31 copies.
- **Forget `noexcept` (Q6):** delete `noexcept` from the move constructor → section 3 still moves, but section 4 changes from `copies = 0, moves = 31` to `copies = 31, moves = 0`. Then add `v.reserve(17);` before the loop → 0 copies, 0 moves.
- **Don't reset the moved-from pointer (Q6):** comment out `rhs.data_ = nullptr;` in the move ctor → `make asan` → `attempting double-free` (the moved-from destructor frees the stolen array).
- **Using a moved-from object (Q6):** after `Buffer b = std::move(a);` add `cout << a.size();` → prints `0` here, but that's *our* choice — for library types the value is unspecified. Assigning to it (`a = Buffer("reborn", 2)`) is always fine.
- **Move-only `unique_ptr` (Q6):** uncomment `(D)` / `(E)` → "use of deleted function `unique_ptr(const unique_ptr&)`". Note that `std::move(p)` moved the *pointer*: no `Buffer` ctor/dtor lines appear.
