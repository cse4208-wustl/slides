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

# Sequential Containers

---

## Container Interfaces

A container provides:

- **Iterators** over its elements: `begin()`, `end()`
- Functions to **add** elements: `push_back()`, `emplace_back()`
- **Container-specific** operations: `operator[]` only if random access

**Principle of least surprise:** no `operator[]` for `list`, because it would hide a linear walk.

---

## Containers Provide Their Own Iterators

```c++
vector<int>::iterator i;          // can modify elements
vector<int>::const_iterator ci;   // read-only

auto ci2 = v.cbegin();            // const_iterator
while (ci2 != v.cend()) {
    cout << *ci2 << endl;         // does not modify v
    ++ci2;
}
```

**Use `const_iterator`s whenever you can.**

---

## Iterators Generalize Pointers

- They define **left-inclusive** ranges: `[begin, end)`
- They're the interface between **algorithms** and **data structures**
  - Algorithms manipulate iterators, not containers
- An iterator is in one of **three states**:

| State | Meaning | Pointer example |
|---|---|---|
| **Dereferenceable** | points to a valid element | `int* p = arr;` |
| **Past the end** | just past the last element | `int* p = arr + N;` |
| **Singular** | points to nothing | `int* p;` or `nullptr` |

---

## Properties of Iterator Intervals

**Valid** intervals can be traversed safely with an iterator.

- An **empty** interval `[p, p)` is valid
- If `[first, last)` is valid and non-empty, then `[first + 1, last)` is valid
  - Proof: **iterative induction** on the interval
- If `[first, mid)` and `[mid, last)` are valid, then `[first, last)` is valid
  - Proof: **divide and conquer** induction on the interval
- **Corollary:** if `[first, last)` is valid, `mid` is reachable from `first`, and `last` is reachable from `mid`, then `[first, mid)` and `[mid, last)` are both valid

---

## Why These Properties Matter

**Iterative induction** makes this loop safe:

```c++
while (first != last) {   // empty interval: loop never runs
    process(*first);      // non-empty: *first is valid
    ++first;              // [first + 1, last) is still valid
}
```

**Divide and conquer** lets us split an interval at `mid`:

- Work on `[first, mid)` and `[mid, last)` separately, e.g., in merge sort or binary search
- The corollary guarantees both halves are valid

---

## Standard Sequential Containers

| Container | Grows efficiently at | Random access `[]` | Iteration |
|---|---|---|---|
| `vector` | back | ✓ | both ways |
| `deque` | front and back | ✓ | both ways |
| `list` | anywhere | ✗ | both ways |
| `forward_list` | anywhere | ✗ | forward only |

---

## Insertion vs. Emplacement

```c++
vector<string> courses;
courses.push_back("cse428");     // 2 constructor calls
courses.emplace_back("cse428");  // 1 constructor call
```

- `push_back` takes an **object**: builds a temporary, then moves/copies it in
- `emplace_back` takes **constructor arguments** and forwards them
  - The element is built **directly** in the container

---

## Iterator Operations Depend on the Container

| Container | Iterator kind | `*`, `!=`, `++` | `--` | `+=`, `<` | `c[n]` |
|---|---|---|---|---|---|
| `vector`, `deque` | random access | ✓ | ✓ | ✓ | ✓ |
| `list` | bidirectional | ✓ | ✓ | ✗ | ✗ |
| `forward_list` | forward | ✓ | ✗ | ✗ | ✗ |

- Each operation is offered **only if it's constant time**
- Element types have **few requirements**
  - e.g., a default constructor to **resize** a container

---

## Self-Assessment Questions

---

### Q1: Iterator States

```c++
int arr[3] = {1, 2, 3};
int* p = arr;
int* q = arr + 3;
int* r;
```

| State | Meaning |
|---|---|
| **Dereferenceable** | points to a valid element |
| **Past the end** | just past the last element |
| **Singular** | points to nothing |

***Question***: What state is each pointer in? Which of `*p`, `*q`, `*r`, and `p != q` are safe?

---

### Q1: Answer

| Pointer | State | Dereference? | Compare? |
|---|---|---|---|
| `p` | **dereferenceable** | ✓ `*p` is `1` | ✓ |
| `q` | **past the end** | ✗ undefined | ✓ `p != q` is how loops stop |
| `r` | **singular** | ✗ undefined | ✗ not even comparison |

- `[p, q)` is a valid range of **3** elements
- Container iterators work the same way: `v.end()` is like `q`

---

### Q2: Comparing Iterators

```c++
list<int> lst = {1, 2, 3};
auto first = lst.begin(), last = lst.end();
while (first < last) {
    cout << *first << endl;
    ++first;
}
```

| Container | Iterator kind | `*`, `!=`, `++` | `--` | `+=`, `<` | `c[n]` |
|---|---|---|---|---|---|
| `vector`, `deque` | random access | ✓ | ✓ | ✓ | ✓ |
| `list` | bidirectional | ✓ | ✓ | ✗ | ✗ |
| `forward_list` | forward | ✓ | ✗ | ✗ | ✗ |

***Question***: Does this compile? Would it with a `vector<int>`? How do you make it work with **both**?

---

### Q2: Answer

- **Compile error:** `list` iterators are **bidirectional**, with no `operator<`
  ```
  error: no match for 'operator<' (operand types are
      'std::_List_iterator<int>' and 'std::_List_iterator<int>')
  ```
- With `vector<int>` it **compiles**: random-access iterators support `<`
- **Fix:** use `!=`, which **every** iterator supports
  ```c++
  while (first != last)
  ```
- Loops written with `!=` and `++` work with **any** container

---

### Q3: Choosing a Container

For each task, which container fits best?

1. Read numbers, then access them by **index** in random order
2. A queue: add at the **back**, remove from the **front**, and sometimes look at element `[i]`
3. Frequently insert and remove in the **middle**, always walking front to back

---

### Q3: Answer

1. **`vector`**: fast `[]`, grows at the back, the default choice
2. **`deque`**: grows at **both** ends and still supports `[]`
   - `vector` has no `push_front`/`pop_front`
   - `list` has no `[]`
3. **`forward_list`** (or `list`): inserts anywhere in constant time
   - Forward-only walking is enough, and it uses less memory than `list`

---

### Q4: Counting Constructor Calls

`Course` prints a line in each of its constructors (normal, copy, move).

```c++
vector<Course> v;
v.reserve(4);
v.push_back(Course("cse428", 428));
v.emplace_back("cse4208", 4208);
```

***Question***: How many constructor calls does each line make, and which ones?

---

### Q4: Answer

```
  push_back(Course("cse428", 428)):
    ctor  cse428       ← the temporary
    move  cse428       ← into the vector
  emplace_back("cse4208", 4208):
    ctor  cse4208      ← built in place
```

- `push_back`: **2** calls, a constructor for the temporary plus a move
- `emplace_back`: **1** call, with its arguments forwarded to the constructor
- Without `reserve`, the vector **grows**, and existing elements are moved again

---

### Q5: Sizing a Container

`Course` has only one constructor: `Course(string, int)`.

```c++
vector<Course> a;                              // (1)
vector<Course> b(3);                           // (2)
vector<Course> c(3, Course("cse428", 428));    // (3)
vector<Course> d;
d.reserve(3);                                  // (4)
d.resize(3);                                   // (5)
```

***Question***: Which lines compile? For those that do, what's the size, and how many constructor calls are made?

---

### Q5: Answer

| Line | Compiles? | Size | Constructor calls |
|---|---|---|---|
| (1) `a` | ✓ | 0 | none: empty |
| (2) `b(3)` | ✗ | | needs `Course()` to build 3 elements |
| (3) `c(3, ...)` | ✓ | 3 | 1 `ctor` + 3 `copy` |
| (4) `reserve(3)` | ✓ | 0 | none: raw memory only |
| (5) `resize(3)` | ✗ | | needs `Course()` for new elements |

```
error: no matching function for call to 'Course::Course()'
```

---

### Q5 Answer (cont.): The Constraint

- Constructors and operations that **create elements from nothing** need a **no-argument constructor**
  - `vector<T> v(n)` and `v.resize(n)` **value-initialize** the new elements
- Supplying a value (`v(n, t)`, `resize(n, t)`) **copies** it instead
- `reserve(n)` creates **no** elements, so it has no requirement
- **Fix (2) and (5):** add `Course() : name("tbd"), number(0) {}`
  - `b(3)` then builds 3 elements, each with `Course()`

---
## Lab 3

- Add Uno cards and deck
- GoFishGame can be played with different decks, as long as there are 4 cards of each rank.
- Add GoFishGame class template, parameterized by
   - Rank
   - Suit
   - Deck
- Implement GoFishGame logic
- Apply standard algorithms to find and move cards
---

## Studio 17

- Iterate through a built-in **array with pointers**
  - `sizeof(arr)`, `sizeof(arr[0])`, and from those the **number of elements**
- Define iteration **generically** using a limited set of required operators
- Explore which containers support **`operator[]`**
