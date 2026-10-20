// Module 13 -- Q5: Template argument deduction and references
// Build & run:  make q5_demo
#include <iostream>
#include <type_traits>
using namespace std;

// is_const<T> and is_reference<T> (from <type_traits>) tell us what T was deduced as

template <typename T> void f1(T &p) {
    cout << "f1(" << p << "): T is const? " << is_const<T>::value << endl;
}

template <typename T> void f2(const T &p) {
    cout << "f2(" << p << "): T is const? " << is_const<T>::value << endl;
}

template <typename T> void f3(T &&p) {
    cout << "f3(" << p << "): T is a reference? " << is_reference<T>::value << endl;
}

int main() {
    cout << boolalpha;
    int i = 1;
    const int ci = 2;

    f1(i);           // T = int
    f1(ci);          // T = const int
    // f1(5);        // (A) error: T& can't bind to an rvalue

    f2(i);           // T = int
    f2(ci);          // T = int (the const is already in the parameter)
    f2(5);           // T = int

    f3(42);          // rvalue: T = int,  parameter int&&
    f3(i);           // lvalue: T = int&, parameter int& && -> int&
    f3(ci);          // lvalue: T = const int&
    f3(i * ci);      // rvalue: T = int
    f3(i = ci);      // lvalue (= returns i): T = int&

    // reference collapsing also happens through a type alias
    using IntRef = int &;
    IntRef &&r = i;  // int& && -> int&: r is just another name for i
    r = 10;
    cout << "after r = 10: i = " << i << endl;
    return 0;
}
