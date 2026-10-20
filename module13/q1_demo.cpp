// Module 13 -- Q1: Conversions and template type parameters
// Build & run:  make q1_demo
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

// arguments are copied
template <typename T> void fobj(T a, T b) {
    cout << "fobj: " << a << ", " << b << endl;
}

// arguments are passed by reference to const
template <typename T> void fref(const T &a, const T &b) {
    cout << "fref: " << a << ", " << b << endl;
}

template <typename T> int compare(const T &v1, const T &v2) {
    if (v1 < v2) return -1;
    if (v2 < v1) return 1;
    return 0;
}

// two type parameters: the argument types may differ
template <typename A, typename B> int flexibleCompare(const A &v1, const B &v2) {
    if (v1 < v2) return -1;
    if (v2 < v1) return 1;
    return 0;
}

// the first parameter has an ORDINARY type: normal conversions apply to it
template <typename T> ostream &print(ostream &os, const T &obj) {
    return os << obj << endl;
}

int main() {
    string s1("a value");
    const string s2("another value");
    fobj(s1, s2);        // ok: T = string (top-level const of s2 is ignored)
    fref(s1, s2);        // ok: T = string (s1 is converted to const)

    int a[10] = {1}, b[42] = {2};
    fobj(a, b);          // ok: T = int* (arrays become pointers: prints addresses)
    // fref(a, b);       // (A) error: int[10] vs. int[42] -- no array-to-pointer for &

    long lng = 7;
    // compare(lng, 1024);   // (B) error: T is long from lng, but int from 1024
    // fobj(3.14, 2);        // (C) error: T is double vs. int -- no arithmetic conversion
    cout << "flexibleCompare(lng, 1024) = " << flexibleCompare(lng, 1024) << endl;
    cout << "flexibleCompare(3.14, 2)   = " << flexibleCompare(3.14, 2) << endl;

    ofstream out("demo_output.txt");
    // fref(out, cout);  // (D) error: T is ofstream vs. ostream -- no derived-to-base
    print(out, 42);      // ok: ofstream is converted to ostream& (ordinary parameter)
    print(cout, 42);
    return 0;
}
