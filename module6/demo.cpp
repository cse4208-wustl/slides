#include <iostream>
#include <vector>
#include <string>
using namespace std;

class Shape {
public:
    explicit Shape(string name) : name_(std::move(name)) {}
    virtual ~Shape() { cout << "  ~Shape(" << name_ << ")\n"; }

    string label() const { return "Shape"; }
    virtual string kind() const {return "Shape"; }
    virtual double area() const = 0;             // pure virtual -> Shape is abstract
    virtual void print(ostream &os) const {
        os << name_ << ": area = " << area();
    }

protected:
    string name_;
};

class Circle : public Shape {
public:
    Circle(double r) : Shape("Circle"), radius_(r) {}
    ~Circle() override { cout << "  ~Circle()\n"; }
    string label() const { return "Circle"; }
    virtual string kind() const override{return "Circle"; }
    double area() const override{ return 3.14159265 * radius_ * radius_; }
private:
    double radius_;
};


class Triangle : public Shape {
public:
    Triangle(double s1, double s2, double s3) : Shape("Triangle"), side1_(s1), side2_(s2), side3_(s3) {}
    ~Triangle() override { cout << "  ~Triangle()\n"; }
    string label() const { return "Triangle"; }
    virtual string kind() const override{return "Triangle"; }
    double area() const override { 
        double p = (side1_ + side2_ + side3_)/2;
        return sqrt(p * (p - side1_) * (p - side2_) * (p - side3_)); 
    }
private:
    double side1_, side2_, side3_;
};

class Rectangle : public Shape {
public:
    Rectangle(double w, double h) : Shape("Rectangle"), width_(w), height_(h) {}
    ~Rectangle() override { cout << "  ~Rectangle()\n"; }
    string label() const { return "Rectangle"; }
    virtual string kind() const override {return "Rectangle"; }
    double area() const override { return width_ * height_; }
private:
    double width_, height_;
};

void report(const Shape &s) {          // reference -> dynamic binding on area()
    s.print(cout);
    cout << '\n';
}

int main() {
    vector<Shape*> shapes;
    shapes.push_back(new Circle(2.0));
    shapes.push_back(new Rectangle(3.0, 4.0));

    for (auto s : shapes) {
        report(*s);                    // one call site, one function -- two behaviors
    }

    cout << "--- deleting shapes ---\n";
    for (auto s : shapes) {
        delete s;                      // watch the destructor order print out
    }
}

