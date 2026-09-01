# Enumerations
---

## Self Assessment

---

### Q1: Does this compile?
```c++
enum color {red, yellow, green};
enum stoplight {red, yellow, green};
```

---

### Q1 Answer

- No — redefinition error. Unscoped enumerators don't live inside their enum — they spill out into the **enclosing scope**. 
- So `color` and `stoplight` are both trying to plant a name called `red` in
the same place at the same time. 

What if it was:
```c++
enum class color {red, yellow, green};
enum class stoplight {red, yellow, green};
```

Scoped enum's enumerators are locked inside the enum's own scope; you'd need
`stoplight::red` to reach them. 

---

### Q2 (True/False)
Given thse definitions:
```c++
enum color { red, yellow, green };
enum class peppers { red, yellow, green };
```

the following code will compile:

```c++
int i = color::red;
int j = peppers::red;
```

---
### Q2 Answer
* False — only the first line will compile. 
* Unscoped enumerators implicitly convert to `int`;
* Scoped ones don't

---
### Language Design Decision

>Why would the language designers deliberately make scoped enums *less* convenient here?

- Implicit int conversion can lead to accidental bugs.
- Passing a `Color` where a `Direction` was expected, because both silently became the number `2`. 

---

## State Change

State - values of variables in a program at a specific time

```c++
enum class Weekday { sun, mon, tue, wed, thu, fri, sat };
Weekday w = Weekday::sun;
++w;   // error: no match for 'operator++'
```
How should operator++ change state if 
- w is `fri`?
- w is `sat`?

What about operator--?

---

## State Change Continued
```c++
enum class Pressure {lo, med, hi, pop};
Pressure p = Pressure:lo;
```

How should operator++ change state if:
- p is `med`?
- p is `pop`?

What about operator--?

---
## Studio 2

State transitions with `enum class`
- Use link on Canvas to create starter studio repo
- Use Instructor's Assistant bot link for studio reflection
- Submit pdf transcript of your reflection
