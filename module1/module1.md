# Function & Class Templates
---

## Self-Assessment Questions
---
### Q1: Template Compilation (True/False)
When the compiler compiles 
```c++
template <typename T>
int compare(const T &v1, const T &v2) {
    if (v1 < v2) return -1;
    if (v2 < v1) return 1;
    return 0;
}
```
it generates machine code for the `compare` function.

---
### Q1 Answer
**False.** No code exists until instantiation. 

---

### Q1 Explanation

Compiling templates is a 3-stage process

---
### Stage 1: compiling the template itself

Does this compile?

```c++
template <typename T>
int compare(const T &v1, const T &v2) {
    if (v1 < v2) return -1
    if (v2 < v1) return 1;
    return 0;
}
```
<details>
<summary>Answer</summary>
Missing semicolon. Caught immediately — this is a pure syntax check, no type of <code>T</code> involved
at all.
</details>

---
### Stage 2: the call site, when template is used

Assuming the `compare` function template from Q1, does this compile?

```c++
compare(4);              // one argument, function needs two
compare(4, "hello");     // T deduced as int from arg1, but string literal for arg2 — mismatch
```
<details>
<summary>Answer</summary>
Both of these fail <b>before</b> the compiler has generated any code - it's just checking arity and
that the deduced <code>T</code> is consistent across parameters. No knowledge of what <code>T</code> supports yet.
</details>

---
### Stage 3 — instantiation

Assuming the `compare` function template from Q1, does this compile?

```c++
Sales_data d1, d2;
compare(d1, d2);   // compiles past stage 2 fine — two args, same type
```
<details>
<summary>Answer</summary>

This one *passes* stages 1 and 2 completely. It only fails when the compiler actually tries to
write out <code>compare(const Sales_data&, const Sales_data&)</code> and discovers <code>Sales_data</code> has no
<code>operator<</code>. 

<br>

> Who's at fault here — the person who wrote <code>compare</code>, or the person who called it with <code>Sales_data</code>?

</details>

---
### Q1 Takeaway

* The compiler can't check what it hasn't generated
* It doesn't generate until you ask it to (by using the template with real types)
* Compiler needs the entire template definition at instantiation (not just a declaration) 

---

### Q2: Design Judgment
Both compile, both give identical results for `int`, `double`, `string`.
Which one is the *better* template, and why?

```c++
// Version A
template <typename T>
int compare(const T &v1, const T &v2) {
    if (v1 < v2) return -1;
    if (v1 > v2) return 1;
    return 0;
}

// Version B
template <typename T>
int compare(const T &v1, const T &v2) {
    if (v1 < v2) return -1;
    if (v2 < v1) return 1;
    return 0;
}
```
---
### Q2 Answer
**Version B.** 
- It only requires that T needs `operator<`
- Version A requires both `operator<` and `operator>`

This is the **minimize requirements on the templated type** principle

---
### Q3: Static Members (True/False)
```c++
template <typename T> class Foo {
public:
    static std::size_t count() { return ctr; }
private:
    static std::size_t ctr;
};
template <typename T> std::size_t Foo<T>::ctr = 0;

Foo<int> a, b, c;
Foo<std::string> d;
```

***True or False***: `a`, `b`, `c`, and `d` all share the same `ctr`.

---
### Q3 Answer
**False.** `a`, `b`, `c` share one `Foo<int>::ctr`, `d` has its own, separate
`Foo<string>::ctr`. 

> A class template is a recipe, an instantiated class is a real class
> Every real class gets its own full set of statics.

---

## The Makefile and Templates

> In a Makefile we list source files to compile. Why don't we want a `.cpp` that *only* contains template definitions in that list?

---
## The Makefile and Templates Continued
* A `.cpp` file that only *defines* a template will compile to essentially an **empty object file**. Nothing gets instantiated because nothing in that file *uses* the template with a concrete type.
* Calls to the template (ex: `compare<int>(a, b)`) will generate their copy of that code, right there in its own object file.

---

## The Overhead

> When the book says explicit instantiation "avoids overhead," is that overhead
> (a) the program running slower, or 
> (b) something else?

---
## The Overhead Explained
The overhead is compile time and object-file/binary size, not runtime speed. 

* Suppose `Blob<string>` is used in 20 `.cpp` files, by default 
* All 20 object files each contain a full, identical copy of every used `Blob<string> member`
* The linker later throws away the duplicates
* On a large project that's real minutes-to-hours of build time
* Once linked, `Blob<string>::size()` runs exactly as fast as if it'd been instantiated once

---
## Overhead Fixed
```c++
// header, included everywhere
extern template class Blob<string>;   // "I promise a real copy exists elsewhere — don't build one here"

// exactly one .cpp file in the whole project
template class Blob<string>;          // the one real, non-extern instantiation
```

---
## Explicit vs On-demand (Implicit) instantiation

### Explicit instantiation
```c++
template class Blob<string>; // generates the code for all members of the class
```

### On-demand instantiation
```c++
Blob<string> b;
b.push_back("hello"); // only push_back gets generated
```

---

## Generic Programming

Raise your hand if you've used generic programming?

---

### Generic Programming Example

Raise your hand if you've used the std::sort function?

```c++
std::vector<int> nums = {5, 2, 8, 1};
std::sort(nums.begin(), nums.end());

std::vector<std::string> words = {"banana", "apple", "cherry"};
std::sort(words.begin(), words.end());
```

---
## Generic Programming Continued

```c++
template <typename RandomIt>
void sort(RandomIt first, RandomIt last);
```
> What are the requirements on `RandomIt`?

---
## Studio 1

- Make sure you are signed into Classroom 50
- Click on the assignment link (invite collaborators if working with others)
- Use the Instructor's Assistant Bot for your reflection (not a generic Gemini application)
- When you open Gemini, if it doesn't say Instructor's Assistant, you are in the wrong place
