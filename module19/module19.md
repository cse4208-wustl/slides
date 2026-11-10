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

# Associative Containers

---

## Sequential vs. Associative

- **Sequential** containers (`vector`, `list`, `deque`): elements are found by **position**
- **Associative** containers: elements are found by **key**
- A `map` is an **associative array**: the subscript doesn't have to be an integer
  ```c++
  phone["Kate"] = "314-555-0100";
  ```
- A `set` is just a collection of **keys**: "is this value present?"
  ```c++
  if (bad_checks.find(name) != bad_checks.end()) { /* reject */ }
  ```
- No position-specific operations: no `push_back`, `push_front`, `back`, ...

---

## Ordered Associative Containers

| Container | Elements | Duplicate keys? |
|---|---|---|
| `set<K>` | keys | no |
| `multiset<K>` | keys | yes |
| `map<K, V>` | key/value pairs | no |
| `multimap<K, V>` | key/value pairs | yes |

- A **balanced binary tree** that orders keys, by default with `<`
- Insert, find, and erase are all **O(log n)**
- Iteration yields elements in **ascending key order**
- In a `map`, keys are unique, but different keys **may share a value**

---

## Unordered Associative Containers

| Container | Elements | Duplicate keys? |
|---|---|---|
| `unordered_set<K>` | keys | no |
| `unordered_multiset<K>` | keys | yes |
| `unordered_map<K, V>` | key/value pairs | no |
| `unordered_multimap<K, V>` | key/value pairs | yes |

- A **hash table**: uses a **hash function** and `==`, not `<`
- Elements go into **buckets**; iteration order is **unspecified**
- Same `find`, `insert`, `erase`, ... as the ordered versions

---

## Using a `map` and a `set`

```c++
map<string, size_t> word_count;
set<string> exclude = {"the", "but", "and", "or", "an", "a"};
string word;
while (cin >> word)
    if (exclude.find(word) == exclude.end())   // not excluded?
        ++word_count[word];                    // fetch and increment
for (const auto& w : word_count)
    cout << w.first << " occurs " << w.second << " times" << endl;
```

- `word_count[word]` **creates** the element (value `0`) the first time a word is seen
- Each element of a `map` is a `pair`: `first` is the **key**, `second` is the **value**
- Output is in **alphabetical** order

---

## Associated Types

| | `set<int>` | `map<int, string>` |
|---|---|---|
| `key_type` | `int` | `int` |
| `mapped_type` | n/a | `string` |
| `value_type` | `int` | `pair<const int, string>` |

- For sets, key and value types are **the same**
- For maps, the `value_type` is a `pair` whose key is **`const`**
- Only the map types define `mapped_type`
- Changing a key would break the container's order: so keys **can't** be changed

---

## The `pair` Type

```c++
pair<string, int> p1;                   // {"", 0}: value initialized
pair<string, int> p2{"C", 5};
auto p3 = make_pair("C", 5);            // pair<const char*, int>
cout << p2.first << " " << p2.second;   // public data members
```

- Defined in `<utility>`
- `<`, `>`, ... compare **`first`**, then **`second`** (dictionary order)
- A function can return one: `return {v.back(), v.back().size()};`

---

## Requirements on Key Type (Ordered)

The comparison must be a **strict weak ordering** ("less than"):

- Two keys **can't both** be less than each other
- If `k1 < k2` and `k2 < k3` then `k1 < k3`
- If neither key is less than the other, they're **equivalent**
  - Equivalence must also be transitive

**Equivalent keys are treated as equal:**

- A `set` keeps **only one** of them
- Either key can be used to **find** that element

A type with a `<` that "behaves normally" just works.

---

## Supplying Your Own Ordering

The comparison type is **part of the container's type**:

```c++
bool shorter(const string& a, const string& b) {
    return a.size() < b.size();
}
// a function pointer: give its type, AND pass the function
set<string, decltype(shorter)*> by_len(shorter);

struct CaseInsensitiveLess {        // a function object
    bool operator()(const string& a, const string& b) const;
};
set<string, CaseInsensitiveLess> nocase;   // no argument needed
```

- `decltype(shorter)` is a function **type**: add `*` for a pointer
- With a function pointer, **forgetting** the constructor argument compiles, but the pointer is **null**

---

## Subscripting a `map` Adds Missing Keys

```c++
map<string, size_t> word_count = {{"the", 3}, {"red", 2}};
cout << word_count["cat"] << endl;   // prints 0
cout << word_count.size() << endl;   // prints 3: {"cat", 0} was ADDED
```

`m[k]` when `k` is **missing**:
1. searches for `k`: not found
2. **inserts** `{k, V()}`: the value is value initialized (`0`, `""`, ...)
3. returns a reference to the new value

- Even just **reading** `m[k]` changes the map
- Can't use `[]` on a **`const`** map: it might insert
- Only `map` and `unordered_map` have `[]`

---

## `[]`: Same Syntax as `vector`, Different Meaning

| | `vector` / `deque` / array | `map` |
|---|---|---|
| Subscript | position (`size_t`) | key (`key_type`) |
| Cost | **O(1)** indexing | **O(log n)** search |
| Missing index/key | **undefined behavior** | **inserts** a new element |
| On `const` | ok | **compile error** |

Inserting is exactly what we want for counting:

```c++
while (cin >> word)
    ++word_count[word];   // a new word starts at 0, then becomes 1
```

---

## Looking Up a Key Without Adding It

| Call | If `k` is missing | Returns |
|---|---|---|
| `m.find(k)` | returns `m.end()` | iterator to the element |
| `m.count(k)` | returns `0` | number of elements (0 or 1) |
| `m.at(k)` | throws `out_of_range` | reference to the value |
| `m.contains(k)` (C++20) | returns `false` | `bool` |

```c++
auto it = word_count.find("cat");          // one search...
if (it != word_count.end())
    cout << it->second << endl;            // ...and we have the value
```

- All of these work on a **`const`** map
- **`[]`**: insert-if-missing is what you want
- **`find`**: check **and** use the value; **`at`**: a missing key is an **error**

---

## Unordered Containers: Buckets

- A **hash function** maps each key to a **bucket**; `==` finds it within the bucket
- Equal keys always land in the **same bucket**
- Performance depends on the **hash quality** and the **bucket sizes**

---

## When to Use an Unordered Container

- When the key type has **no obvious ordering**
- When **profiling** shows that keeping keys in order is too costly
- Otherwise an ordered container is usually **easier**, and often just as fast

```c++
unordered_map<string, size_t> word_count;   // the only change
while (cin >> word) ++word_count[word];     // same counts,
                                            // but NOT alphabetical
```

- Swaped `map` with `unordered_map`: same interface, **different order**

---

## Requirements on Key Type (Unordered)

- `==` on the key type, and `hash<key_type>`
- The library provides `hash` for **built-in** types, pointers, `string`, smart pointers
- For **your own** type, define `==` and **specialize** `std::hash`:

```c++
struct Point { int x, y; };
bool operator==(const Point& a, const Point& b) {
    return a.x == b.x && a.y == b.y;
}
namespace std {
template <> struct hash<Point> {          // hash<Point> is now defined
    size_t operator()(const Point& p) const {
        return hash<int>()(p.x) ^ (hash<int>()(p.y) << 1);
    }
};
}
unordered_set<Point> pts;                 // just works, like set<int>
```
---

## Associative Containers and Algorithms

- Keys are `const`: **no** algorithms that **write** or **reorder** (`sort`, `remove`, ...)
- Generic `find` works, but it's a **linear** search: use the **member** `find`

```c++
find(big.begin(), big.end(), 999999);   // O(n):     ~12,000 µs
big.find(999999);                       // O(log n): ~2 µs
```

- In practice, use them as a **source** or a **destination**:

```c++
copy(s.begin(), s.end(), back_inserter(v));        // set -> vector
copy(v.begin(), v.end(), inserter(s, s.end()));    // vector -> set
```

- For a `set`, the `inserter` position is only a **hint**

---

## Self-Assessment Questions

---

### Q1: Types and `const` Keys

```c++
map<string, int> m = {{"A", 3}, {"B", 2}};
set<int> s = {1, 2, 3};
auto it = m.begin();
it->first = "Z";              // (1)
it->second = 7;               // (2)
*s.begin() = 0;               // (3)
pair<string, int>& r = *it;   // (4)
```

***Question***: What are `key_type`, `mapped_type`, and `value_type` of `m`? Which of (1)-(4) compile, and why not the others?

---

### Q1: Answer

- `key_type` is `string`, `mapped_type` is `int`, `value_type` is **`pair<const string, int>`**

| Line | Compiles? | Why |
|---|---|---|
| (1) | ✗ | the key is `const`: `no match for 'operator='` |
| (2) | ✓ | the mapped value is **not** `const` |
| (3) | ✗ | `set` elements are keys: `assignment of read-only location` |
| (4) | ✗ | `*it` is a `pair<const string, int>`, a **different type**: `cannot bind non-const lvalue reference` |

- Changing a key in place would **break the tree's order**
- To "change" a key: `erase` the old element, `insert` a new one
- Fix (4): `auto& r = *it;` or `pair<const string, int>& r = *it;`

---

### Q2: Asking Whether a Key Is There

```c++
map<string, size_t> word_count = {{"the", 3}, {"red", 2}};
if (word_count["cat"] == 0)
    cout << "no cat" << endl;
cout << word_count.size() << endl;

void report(const map<string, size_t>& wc) {
    cout << wc["the"] << endl;
}
```

***Question***: What is printed? Does `report` compile? How should you check for `"cat"`?

---

### Q2: Answer

- Prints **`no cat`**, then **`3`**: `word_count["cat"]` **inserted** `{"cat", 0}`
- `report` does **not** compile: `[]` might insert, so it's not a `const` member
  ```
  error: passing 'const std::map<...>' as 'this' argument discards qualifiers
  ```
- **Fixes**: these never add an element
  ```c++
  if (word_count.find("cat") == word_count.end())  // not present
  if (word_count.count("cat") == 0)                // not present
  cout << wc.at("the") << endl;                    // throws out_of_range if missing
  if (word_count.contains("cat"))                  // C++20
  ```
- Use `[]` when you **want** "insert if missing" (word counting)

---

### Q3: A Custom Ordering

```c++
bool shorter(const string& a, const string& b) {
    return a.size() < b.size();
}
set<string, decltype(shorter)*> by_len(shorter);
for (auto w : {"the", "quick", "red", "fox", "jumps", "over"})
    by_len.insert(w);
```

***Question***: What does `by_len` hold? What does `by_len.find("cat")` return? What if `shorter` used `<=`? What if we wrote `by_len;` with no argument?

---

### Q3: Answer

- `by_len` holds **`the over quick`** (size 3)
  - Strings of the same length are **equivalent**: neither is "less than" the other
  - `"red"`, `"fox"`, `"jumps"` are **rejected** as duplicates
- `by_len.find("cat")` → **`"the"`**: equivalent keys are treated as **equal**
- With `<=`: **not** a strict weak ordering (`a <= a` is `true`): **undefined behavior**
  - In practice: all 6 strings are kept, and `find("the")` returns **`end()`**!
- No constructor argument: compiles, but the pointer is **null**: **segfault** on the first insert
- Want all the words, ordered by length? Use a **`multiset`**

---

### Q4: User-Defined Keys

```c++
struct Point { int x, y; };

set<Point> ps;             // (1)
ps.insert({1, 2});         // (2)
unordered_set<Point> pts;  // (3)
```

***Question***: Which lines compile? What does each container need from `Point`? How do you fix them?

---

### Q4: Answer

- (1) compiles: `less<Point>` isn't **used** until something compares
- (2) fails: `no match for 'operator<' (operand types are 'const Point' and 'const Point')`
- (3) fails: `use of deleted function 'unordered_set<...>::unordered_set()'`
  - There is no `hash<Point>`, so the container can't even be **constructed**
- **Ordered** containers need a strict weak ordering: `operator<` or a comparator
  ```c++
  bool operator<(const Point& a, const Point& b) {
      return a.x < b.x || (a.x == b.x && a.y < b.y);
  }
  ```
- **Unordered** containers need **`==`** and a **hash**
  - Define `operator==` and specialize `std::hash<Point>`
  - Or pass one in: `unordered_set<Point, decltype(hasher)*> pts(10, hasher);`

---

## Studio 19

- Look at the **ordered** associative containers
- See how different **ordering semantics** arise from the types they contain, or from explicit **callable types** given to them
- Explore how container **member functions** and **standard algorithms** work with them
