# Module 6: Self-Assessment Questions

## Q1: The Derived-to-Base Conversion (True/False)

```c++
class Shape {
public:
    virtual double area() const = 0;
};

class Circle : public Shape {
public:
    Circle(double r) : radius_(r) {}
    double area() const override { return 3.14159 * radius_ * radius_; }
private:
    double radius_;
};

void report(const Shape &s) {
    cout << s.area() << endl;
}

int main() {
    Circle c(2.0);
    report(c);   // ???
}
```

***True or False***: This compiles and prints the circle's area, even though `report` only knows about `Shape`.

---

## Q2: Virtual vs. Non-Virtual Dispatch

```c++
class Shape {
public:
    string label() const { return "Shape"; }          // NOT virtual
    virtual string kind() const { return "Shape"; }    // virtual
};
class Circle : public Shape {
public:
    string label() const { return "Circle"; }          // hides, does not override
    string kind() const override { return "Circle"; }
};

Circle c;
Shape *sp = &c;
cout << sp->label() << " " << sp->kind();   // ???
```

***Question***: What does the last line print?

---

## Q3: Virtual Destructors

```c++
class Shape {
public:
    ~Shape() { cout << "~Shape "; }        // NOT virtual
    virtual double area() const = 0;
};
class Circle : public Shape {
public:
    Circle(double r) : radius_(r) {}
    ~Circle() { cout << "~Circle "; }
    double area() const override { return 3.14159 * radius_ * radius_; }
private:
    double radius_;
};

Shape *sp = new Circle(2.0);
delete sp;   // ???
```

***Question***: What does this code print?

---

## Q4: `protected` Access

```c++
class Shape {
protected:
    string name_ = "Shape";
public:
    virtual double area() const = 0;
};
class Circle : public Shape {
public:
    Circle(double r) : radius_(r) { name_ = "Circle"; }   // (1)
private:
    double radius_;
};

int main() {
    Circle c(2.0);
    c.name_ = "Sphere";                                   // (2)
}
```

***Question***: Line (1), (2), or both cause a compiler error. Which one(s) and why?

---

## Q5: Abstract Base Classes (True/False)

```c++
class Shape {
public:
    virtual double area() const = 0;     // pure virtual
    virtual ~Shape() = default;
};

Shape s;                          // (1)
Shape *sp = new Circle(2.0);      // (2) — Circle overrides area()
```

***True or False***: Line (1) compiles as long as `Shape` otherwise looks like a perfectly normal class (valid destructor, no other errors).

---

## Q6: The `override` Specifier (True/False)

```c++
class Shape {
public:
    virtual double area() const = 0;
};
class Circle : public Shape {
public:
    Circle(double r) : radius_(r) {}
    double area() override  // note: no const!
    { return 3.14159 * radius_ * radius_; } 
private:
    double radius_;
};
```

***True or False***: This compiles — `override` is just documentation, and `Circle::area()` successfully overrides `Shape::area()`.

---

## Q7: Default Arguments on a Virtual Function (True/False)

```c++
class Shape {
public:
    virtual void describe(double scale = 1.0) const {
        cout << "Shape scaled by " << scale;
    }
};
class Circle : public Shape {
public:
    void describe(double scale = 2.0) const override {
        cout << "Circle scaled by " << scale;
    }
};

Shape *sp = new Circle;
sp->describe();   // ???
```

***True or False***: This prints `Circle scaled by 2`.
