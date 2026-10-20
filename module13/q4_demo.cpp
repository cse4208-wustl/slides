// Module 13 -- Q4: Function pointers and argument deduction
// Build & run:  make q4_demo
#include <iostream>
#include <string>
using namespace std;

template <typename T> int compare(const T &v1, const T &v2) {
    if (v1 < v2) return -1;
    if (v2 < v1) return 1;
    return 0;
}

// overloaded func: each version takes a different function pointer type
void func(int (*)(const string &, const string &)) {
    cout << "func(string comparator) called" << endl;
}
void func(int (*)(const int &, const int &)) {
    cout << "func(int comparator) called" << endl;
}

int main() {
    // T = int is deduced from the POINTER's type
    int (*pf1)(const int &, const int &) = compare;
    cout << "pf1(1, 2) = " << pf1(1, 2) << endl;

    // func(compare);       // (A) error: compare<int> or compare<string>?
    func(compare<int>);     // ok: the explicit argument picks the instantiation
    func(compare<string>);

    // auto pf2 = compare;  // (B) error: nothing to deduce T from
    auto pf3 = compare<double>;
    cout << "pf3(2.5, 1.5) = " << pf3(2.5, 1.5) << endl;
    return 0;
}
