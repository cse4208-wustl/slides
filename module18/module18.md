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

# Generic Algorithms

---

## Motivating Example: Searching a String

From Matt Austern, *Generic Programming and the STL*:

```c++
char* strchr(char* s, char c) {
    while (*s != 0 && *s != c)
        ++s;
    return *s == c ? s : (char*) 0;
}
```

- **Not very general:** the range always ends at `'\0'`
- Only works for **zero-terminated** C strings

---

## Generalizing Linear Search, Step by Step

```c++
// 1. Use a range [first, last)
char* find1(char* first, char* last, char c);

// 2. Parameterize the element type
template <typename T>
T* find2(T* first, T* last, const T& value);

// 3. Parameterize the iterator type
template <typename Iterator, typename T>
Iterator find(Iterator first, Iterator last, const T& value) {
    while (first != last && *first != value)
        ++first;
    return first;      // last means "not found"
}
```

- Same structure and complexity at every step, only **more abstract**
- `find` now works on a `list`, where `find2` could not

---

## What Makes an Algorithm Generic

- Works on **iterator ranges**, not on containers
- Generally **doesn't add or remove** elements from a container
  - **No container operations**: it only uses iterator operations
- **Container independent**: one `find` for arrays, `vector`, `list`, ...
- **Minimal requirements** on element types
  - `find` needs `==` (`!=`) on elements
  - `accumulate` needs `+`: must be able to **add** elements

```c++
string sum = accumulate(v.cbegin(), v.cend(), string(""));  // ok
string bad = accumulate(v.cbegin(), v.cend(), "");          // error
```

The **third argument's type** decides which `+` is used: `const char*` has no `+`.

---

## Iterator Categories

| Operation | Input | Forward | Bidirectional | Random Access | Output |
|---|---|---|---|---|---|
| Read `=*p` | ✓ | ✓ | ✓ | ✓ | |
| Write `*p=` | | ✓ | ✓ | ✓ | ✓ |
| Access `->` | ✓ | ✓ | ✓ | ✓ `[]` | |
| Iterate | `++` | `++` | `++ --` | `++ -- + - += -=` | `++` |
| Compare | `== !=` | `== !=` | `== !=` | `== != < > <= >=` | |
| Example | `istream` | `forward_list` | `list` | `vector`, `deque` | `ostream` |

- **Input → Forward → Bidirectional → Random Access:** each one supports everything the previous one does, plus more
- **Output** is a separate, **write-only** category (e.g., `ostream_iterator`, insert iterators)
- `find` needs an **input** iterator; `sort` needs **random access**

---

## Algorithms on Two Sequences

```c++
// Is roster2 the same as roster1?
equal(roster1.cbegin(), roster1.cend(), roster2.cbegin());
```

- The first sequence is a **range**: `[begin, end)`
- If the second sequence is designated by **only one iterator**:
  - It **must be at least as long** as the first
  - The algorithm can't check: it has no `end` for the second sequence
- C++14 adds a **four-iterator** version that checks both lengths

```c++
equal(roster1.cbegin(), roster1.cend(), roster2.cbegin(), roster2.cend());
```

---

## Algorithms That Write Elements

```c++
fill(vec.begin(), vec.end(), 0);       // write 0 to each element
fill_n(vec.begin(), vec.size(), 0);    // same, with a count
copy(begin(a1), end(a1), a2);          // a2 must hold at least as many as a1
```

- Algorithms that write to a **destination iterator** **assume** the destination is **large enough**
- An algorithm **can't** grow a container: that needs a container operation
- To add elements, give the algorithm an **insert iterator** as the destination

```c++
vector<int> vec;                          // empty
fill_n(back_inserter(vec), 10, 0);        // ok: 10 calls to push_back
```

---

## Algorithms That Reorder Elements

```c++
sort(words.begin(), words.end());                 // fox red red the the ...
auto end_unique = unique(words.begin(), words.end());
words.erase(end_unique, words.end());             // container operation
```

- `unique` **rearranges** so each word appears once at the front
- It **returns an iterator** to the new end of the unique range
- The container's **size is unchanged**: leftover elements are still there
- **You must use a container operation** (`erase`) to remove what you don't want

---

## Customizing Operations

Many algorithms use `<` or `==` by default. We can pass our own operation **instead**.

- **Predicate**: an expression that returns a value usable as a **condition**
  - **Unary**: one argument, e.g., `find_if`, `count_if`
  - **Binary**: two arguments, e.g., `sort`, `stable_sort`
- Algorithms call the predicate **on elements of the input range**
  - The element type **must convert** to the predicate's parameter type

```c++
bool isShorter(const string& s1, const string& s2) {
    return s1.size() < s2.size();
}
stable_sort(words.begin(), words.end(), isShorter);   // binary predicate
```

---

## Callable Types

```c++
// 1. A function (or function pointer)
bool isShorter(const string&, const string&);

// 2. A function object: a class with operator()
template <typename T>
struct odd {
    bool operator()(const T& t) const { return t % 2 != 0; }
};
count_if(v.begin(), v.end(), odd<int>());

// 3. A lambda expression: an unnamed, inline callable
auto wc = find_if(words.begin(), words.end(),
                  [sz](const string& a) { return a.size() >= sz; });
```

- A lambda's **capture list** `[sz]` makes local variables usable inside it
- `find_if` takes a **unary** predicate: the lambda lets us add the extra `sz`

---

## Revisiting Iterators

The `<iterator>` header adds iterators beyond the containers' own:

- **Insert iterators**: bound to a container, **add** elements to it
- **Stream iterators**: bound to a stream, read or write it
  ```c++
  copy(v.begin(), v.end(), ostream_iterator<int>(cout, " "));
  ```
- **Reverse iterators**: `++` moves **backward** (`rbegin()`, `rend()`)

---

## Insert Iterators

An **iterator adaptor** that takes a **container** and yields an iterator that **adds elements** to that container.

| Adaptor | Calls | Requires |
|---|---|---|
| `back_inserter(c)` | `c.push_back(val)` | `push_back` |
| `front_inserter(c)` | `c.push_front(val)` | `push_front` |
| `inserter(c, iter)` | `c.insert(iter, val)` | `insert` |

- `it = val` (or `*it = val`) **inserts** `val`
- `*it`, `++it`, `it++` **do nothing**: each returns `it`
  - They exist so the algorithms' `*dest++ = val` code compiles

---

## `inserter` in Detail: What `*it = val` Does

```c++
auto it = inserter(c, iter);   // it is an insert_iterator, NOT c's iterator
*it = val;
```

behaves as

```c++
iter = c.insert(iter, val);  // insert BEFORE iter; iter now -> the new element
++iter;                      // iter -> the ORIGINAL element again
```

- `inserter` **remembers** the container and a position inside it
- Each assignment inserts **in front of the element `iter` originally denoted**
- So the new elements come out **in their original order**

---

## `inserter` vs. `front_inserter`

```c++
list<int> lst = {1, 2, 3, 4};
list<int> lst2, lst3;                                          // empty
copy(lst.cbegin(), lst.cend(), front_inserter(lst2));          // 4 3 2 1
copy(lst.cbegin(), lst.cend(), inserter(lst3, lst3.begin()));  // 1 2 3 4
```

- `front_inserter` always inserts before the **current** first element
  - Each new element becomes the new front: the sequence is **reversed**
- `inserter(c, c.begin())` inserts before the **original** first element
  - Starting at `begin()` doesn't make it a front inserter
- `inserter` is the only one that works with a `set` (no `push_back`/`push_front`)
  ```c++
  copy(v.begin(), v.end(), inserter(s, s.end()));
  ```

---

## Self-Assessment Questions

---

### Q1: Comparing Two Sequences

```c++
vector<int> roster1 = {1, 2, 3, 4, 5};
vector<int> roster2 = {1, 2, 3};
bool same = equal(roster1.cbegin(), roster1.cend(), roster2.cbegin());
```

***Question***: What is the value of `same`? What does `equal` do once it reaches the end of `roster2`? How would you fix the call?

---

### Q1: Answer

- **Undefined behavior:** `equal` assumes `roster2` is **at least as long** as `roster1`
  - After comparing `1 2 3`, it reads `roster2[3]` and `roster2[4]`, **past the end**
  - `same` may be `true`, `false`, or the program may crash
  - AddressSanitizer: `heap-buffer-overflow`
- `equal` **can't check**: it was given **no end** for `roster2`
- **Fixes:**
  ```c++
  roster1.size() == roster2.size() && equal(roster1.cbegin(), roster1.cend(), roster2.cbegin());
  equal(roster1.cbegin(), roster1.cend(), roster2.cbegin(), roster2.cend());  // C++14
  ```
  Both give `false`.

---

### Q2: Writing to an Empty Vector

```c++
vector<int> vec;                      // empty
fill_n(vec.begin(), 10, 0);           // (1)

vector<int> vec2;
vec2.reserve(10);
fill_n(vec2.begin(), 10, 0);          // (2)
```

***Question***: What happens in (1)? Does `reserve` in (2) fix it? What is the right way to put ten `0`s into an empty vector with `fill_n`?

---

### Q2: Answer

- **(1) Undefined behavior:** `fill_n` assumes the destination has **10 elements**
  - `vec` has **none**: the writes go to memory the vector doesn't own
  - Typically a crash (AddressSanitizer: `SEGV`)
- **(2) Still wrong:** `reserve` allocates memory but creates **no elements**
  - It doesn't crash, but `vec2.size()` is **still 0**: the ten `0`s are invisible
- Algorithms **never** change a container's size
- **Fix:** let an insert iterator call the container operation
  ```c++
  fill_n(back_inserter(vec), 10, 0);    // 10 push_backs: size 10
  ```
  Or `vec.resize(10)` first, then `fill_n(vec.begin(), 10, 0)`

---

### Q3: Three Kinds of Inserters

```c++
list<int> src = {1, 2, 3, 4};
list<int> b = {10, 20}, f = {10, 20}, m = {10, 20};
vector<int> v = {10, 20};
copy(src.cbegin(), src.cend(), back_inserter(b));               // (1)
copy(src.cbegin(), src.cend(), front_inserter(f));              // (2)
copy(src.cbegin(), src.cend(), inserter(m, next(m.begin())));   // (3)
copy(src.cbegin(), src.cend(), front_inserter(v));              // (4)
```

***Question***: What does each container hold afterward?

---

### Q3: Answer

| | Calls | Result |
|---|---|---|
| (1) `back_inserter(b)` | `push_back` | `10 20 1 2 3 4` |
| (2) `front_inserter(f)` | `push_front` | `4 3 2 1 10 20`: **reversed** |
| (3) `inserter(m, →20)` | `insert` before `20` | `10 1 2 3 4 20`: **in order** |
| (4) `front_inserter(v)` | `push_front` | **compile error** |

```
error: 'class std::vector<int>' has no member named 'push_front'
```

- (3) inserts before the element `iter` **originally** denoted: `20` stays last

---

### Q4: Removing Duplicates

```c++
vector<string> words = {"the", "quick", "red", "fox", "jumps",
                        "over", "the", "slow", "red", "turtle"};
sort(words.begin(), words.end());
auto end_unique = unique(words.begin(), words.end());
cout << words.size() << endl;
```

***Question***: What is printed? Have the duplicates been removed? What else must you do?

---

### Q4: Answer

- Prints **`10`**: `unique` **can't** change the size
```
sorted      : fox jumps over quick red red slow the the turtle (size 10)
after unique: fox jumps over quick red slow the turtle the  (size 10)
after erase : fox jumps over quick red slow the turtle (size 8)
```
- `unique` moves the unique words to the **front**, returns the **new end**
- Elements past `end_unique` are **unspecified** (here a leftover `"the"` and a moved-from `""`)
- **Fix:** a **container operation** removes them
  ```c++
  words.erase(end_unique, words.end());
  ```

---

### Q5: Which Calls Compile?

```c++
bool isShorter(const string& s1, const string& s2);
vector<string> words = ...;
list<int> lst = ...;
string::size_type sz = 5;

sort(lst.begin(), lst.end());                                    // (1)
sort(words.begin(), words.end(), isShorter);                     // (2)
find_if(words.begin(), words.end(), isShorter);                  // (3)
find_if(words.begin(), words.end(),
        [](const string& a) { return a.size() >= sz; });         // (4)
```

***Question***: Which compile? Why not the others, and how do you fix them?

---

### Q5: Answer

| Call | Compiles? | Why |
|---|---|---|
| (1) | ✗ | `sort` needs **random access**: `list` iterators have no `-`. Use `lst.sort()` |
| (2) | ✓ | **binary** predicate, `string` converts to `const string&` |
| (3) | ✗ | `find_if` calls a **unary** predicate: `too few arguments to function` |
| (4) | ✗ | `'sz' is not captured`. Fix: `[sz](const string& a) {...}` |

- The **iterator category** and the predicate's **arity** are both requirements the algorithm imposes
- They're checked at **compile time**, deep inside the library's code

---

## Studio 18

- Apply different algorithms to iterators over ranges in **different containers**
- Compare **pointers into arrays** with iterators over random access containers
- Make algorithms more general by passing in **callable types**
