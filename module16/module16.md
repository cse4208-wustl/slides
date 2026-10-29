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

# Template Specializations

---

## Starting Point: `unordered_map<string, int>`

```c++
unordered_map<string, int> counts;
for (string word : {"how", "now", "brown", "cow", "how", "now"})
    ++counts[word];
for (const auto& p : counts)
    cout << p.first << ": " << p.second << endl;
```

```
cow: 1
brown: 1
now: 2
how: 2
```

- `counts[word]` creates a missing key with value `0`, then `++` increments it
- Iteration order is **unspecified**: it's a hash table, not a sorted tree

---

## How Does It Find a Key?

An `unordered_map` is an array of **buckets**. To look up `counts["how"]`:

1. **Hash** the key: `size_t h = hash<string>{}("how");`
2. Pick a **bucket** from `h` (roughly `h % bucket_count()`)
3. Walk that bucket and compare keys with **`==`**: other keys may share it

So the key type needs **two** things:

| Operation | Purpose | For `string` |
|---|---|---|
| a **hash** function | find the right bucket fast | library provides `hash<string>` |
| **`==`** | tell keys in the same bucket apart | `string` has `operator==` |

---

## Our Own Key Type

```c++
struct Student {
    string name;
    int id;

    bool operator==(const Student& other) const {
        return name == other.name && id == other.id;
    }
};

unordered_map<Student, int> scores;
scores[{"Kate", 101}] = 95;
scores[{"Alice", 102}] = 88;
```

- `==` is covered
- ***Question***: Will this compile?

---

## It Doesn't Compile

**71 lines** of output and **3 errors**. Here are the lines that matter:

```
unordered_map:653: error: call to implicitly-deleted default
    constructor of 'std::hash<Student>'
hash_demo.cpp:50:33: note: in instantiation of member function
    'std::unordered_map<Student, int>::unordered_map' requested here

__hash_table:644: error: static assertion failed ...:
    the specified hash does not meet the Hash requirements

unordered_map:660: error: type 'const std::hash<Student>'
    does not provide a call operator
hash_demo.cpp:51:11: note: in instantiation of member function
    'std::unordered_map<Student, int>::operator[]' requested here
```

```c++
50    unordered_map<Student, int> scores;
51    scores[{"Kate", 101}] = 95;
```

---

## Reading the Error

`unordered_map` has more template parameters than the two we wrote:

```c++
template <class Key, class T,
          class Hash = std::hash<Key>,          // ← default hasher
          class KeyEqual = std::equal_to<Key>,  // ← uses ==
          class Allocator = std::allocator<std::pair<const Key, T>>>
class unordered_map;
```

- `std::hash<T>` is a **class template**
- The library **specializes** it for built-in types, pointers, `string`, `string_view`, smart pointers, ...
- For any other `T`, `hash<T>` is **disabled**: it exists, but has no default constructor and no call operator
- Nobody wrote `hash<Student>`, so we get the disabled one

---

## What a Hash Must Do

A hasher `h` for a key type `Key` must:

1. Be **callable** as `h(key)` with a `const Key&` argument and return a **`size_t`**
2. **Not modify** the key
3. Be **deterministic**: the same value gives the same hash for the whole run of the program
4. **Agree with `==`**: if `k1 == k2`, then `h(k1) == h(k2)`

Different keys **may** share a hash (a **collision**); `==` sorts them out.

A good hash also **spreads** keys evenly across buckets. A hash that always returns `0` is *legal*, but every lookup becomes a linear search.

---

## Solution 1: Specialize `std::hash<Student>`

```c++
namespace std {
    template <>
    struct hash<Student> {
        size_t operator()(const Student& s) const {
            size_t h1 = hash<string>{}(s.name);
            size_t h2 = hash<int>{}(s.id);
            return h1 ^ (h2 << 1);
        }
    };
}

unordered_map<Student, int> scores;   // unchanged: now compiles
```

- `template <>`: a **full specialization** of the library's template
- Goes in `namespace std`: allowed for specializations **for your own types**
- Reuse the library's hashes for the **members** and combine them

---

## Solution 1: The Library Calls Our Code

With a print added to `operator()`:

```c++
scores[{"Kate", 101}] = 95;           // hash 1: not found, insert
scores[{"Alice", 102}] = 88;          // hash 2: not found, insert
int alice = scores[{"Alice", 102}];   // hash 3: found
```

```
--- Student -> int, std::hash<Student> ---
  size_t std::hash<Student>::operator()(const Student &) const
  size_t std::hash<Student>::operator()(const Student &) const
  size_t std::hash<Student>::operator()(const Student &) const
Alice: 88
```

- `unordered_map` was written long before `Student` existed, yet it calls our hash
- **Specialization changes what the library's default means for our type**

---

## Solution 2: Pass a Hash Functor

```c++
struct StudentHash {
    size_t operator()(const Student& s) const {
        size_t h1 = hash<string>{}(s.name);
        size_t h2 = hash<int>{}(s.id);
        return h1 ^ (h2 << 1);
    }
};

unordered_map<Student, int, StudentHash> scores2;   // 3rd argument
```

```
--- Student -> int, StudentHash ---
  size_t StudentHash::operator()(const Student &) const
  size_t StudentHash::operator()(const Student &) const
  size_t StudentHash::operator()(const Student &) const
Alice: 88
```

No specialization: we **replace** the default `Hash = std::hash<Key>`.

---

## Which One?

| | Specialize `std::hash<Student>` | Pass `StudentHash` |
|---|---|---|
| `unordered_set`, other containers | work automatically | must name `StudentHash` each time |
| Generic code that uses `std::hash<T>` | finds it | doesn't |
| Different hashes for different maps | one per type | as many as you like |
| Type you don't own (e.g., `pair<int, int>`) | not allowed | works |

**Rule of thumb:** specialize `std::hash` when your type has **one natural** hash; pass a functor when it doesn't, or when you don't own the type.

---

## Specialization Can Also Optimize

- `hash<Student>`: the primary template was **unusable**, and the specialization made it work
- A specialization can also **replace** a working version with a **better** one for a particular type
- Example: a `bool` holds **1 bit** of information but takes a whole **byte**
- 64 `bool`s take 64 bytes, but **8** would do

Let's build a small array class, then specialize it for `bool`.

---

## A Simple `Array<T, N>`

```c++
template <typename T, size_t N>
class Array {
public:
    T get(size_t i) const { return data[i]; }
    void set(size_t i, T value) { data[i] = value; }
    size_t size() const { return N; }
private:
    T data[N] = {};
};
```

- `N` is a **non-type** template parameter: a **value**, not a type
- `Array<int, 10>` and `Array<int, 20>` are **different types**
- One `T` per element: `Array<bool, 64>` would take 64 bytes

---

## Specializing for `bool`

```c++
template <size_t N>
class Array<bool, N> {
public:
    bool get(size_t i) const { return bits[i / 8] & (1 << (i % 8)); }
    void set(size_t i, bool value) {
        if (value) bits[i / 8] |= (1 << (i % 8));
        else       bits[i / 8] &= ~(1 << (i % 8));
    }
    size_t size() const { return N; }
private:
    unsigned char bits[(N + 7) / 8] = {};
};
```

- **Partial specialization**: `T` is fixed to `bool`, but `N` is still a parameter
- Element `i` lives in byte `i / 8`, bit `i % 8`
- **Same public interface**, completely different private data

---

## Same Interface, Different Storage

```c++
template <typename T, size_t N>
void print(const Array<T, N>& a) {
    for (size_t i = 0; i < a.size(); ++i)
        cout << a.get(i) << ' ';
    cout << endl;
}
```

```
0 1 4 9 16 25 36 49 64 81       ← print(nums),  Array<int, 10>
1 0 0 1 0 0 1 0 0 1             ← print(flags), Array<bool, 10>
sizeof(Array<int, 64>):  256
sizeof(Array<char, 64>): 64
sizeof(Array<bool, 64>): 8
```

`print` doesn't know (or care) which version it got.

---

## The Standard Library Did This: `vector<bool>`

`vector<bool>` is a library specialization that packs bits. But `vector<T>::operator[]` returns a `T&`, and **you can't refer to a single bit**. So `vector<bool>::operator[]` returns a **proxy object** that only acts like a reference:

```c++
vector<bool> v = {true, true, true};
bool& r = v[0];         // error
auto x = v[0];          // x is a vector<bool>::reference, not a bool
x = false;              // ...so this changes v[0]
cout << v[0];           // false
```

```
error: non-const lvalue reference to type 'bool' cannot bind to
    a temporary of type 'reference'
    (aka '__bit_reference<vector<bool, allocator<bool>>>')
```

---

## Lesson: A Specialization Is a Different Class

- It shares **nothing** with the primary template but the name: you write **every** member again
- Nothing forces it to keep the **same interface**
- `vector<bool>` breaks generic code that works for every other `vector<T>`:

```c++
template <typename T>
void reset_first(vector<T>& v) { T& first = v[0]; first = T{}; }

reset_first(ints);    // fine
reset_first(bools);   // error: same message as before
```

- Many consider `vector<bool>` a **design mistake**; our `Array` avoided it by never handing out references

---

## Self-Assessment Questions

---

### Q1: Breaking the Rules

Suppose `Student::operator==` compares **only** `id`, but `hash<Student>` still combines `name` **and** `id`.

```c++
scores[{"Kate", 101}] = 95;
cout << scores[{"Katherine", 101}] << endl;
```

***Question***: The two keys are equal according to `==`. What does this code most likely print, and which hash requirement is broken?

---

### Q1: Answer

- It most likely prints **`0`**, and the map now has **2** entries
- Broken rule: **equal keys must have equal hashes**
  - `{"Katherine", 101}` hashes differently from `{"Kate", 101}`, so it almost certainly lands in a **different bucket**
  - `==` is never even called on Kate's entry, so the lookup **misses**
  - `operator[]` then **inserts** `{"Katherine", 101}` with value `0`
- (If both keys happen to share a bucket, `==` finds Kate and it prints `95`: the bug is **unpredictable**)
- **Fix:** hash only what `==` compares. If `==` uses only `id`, hash only `id`

---

### Q2: A Key You Don't Own

```c++
unordered_map<pair<int, int>, int> grid;
grid[{0, 0}] = 1;
```

***Question***: Does this compile? If not, which solution can you use, and why is the other one off-limits?

---

### Q2: Answer

**No.** The library has no `hash<pair<int, int>>`, so we get the same error:

```
error: call to implicitly-deleted default constructor of 'std::hash<std::pair<int, int>>'
```

- **Solution 1 is off-limits:** you may only specialize `std` templates for **your own** types. `pair<int, int>` belongs to the library, so `std::hash<pair<int, int>>` would be undefined behavior
- **Solution 2 works:** a functor is our own type

```c++
struct PairHash {
    size_t operator()(const pair<int, int>& p) const {
        return hash<int>{}(p.first) ^ (hash<int>{}(p.second) << 1);
    }
};
unordered_map<pair<int, int>, int, PairHash> grid;
```
