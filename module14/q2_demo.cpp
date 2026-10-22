// Module 14 -- Q2: Which overloaded template runs?
// Build & run:  make q2_demo
// __PRETTY_FUNCTION__ (GCC/Clang) prints the chosen function and its deduced T.
#include <iostream>
using namespace std;

template <typename T> void g(const T &) { cout << "(A) " << __PRETTY_FUNCTION__ << endl; }
template <typename T> void g(T *)       { cout << "(B) " << __PRETTY_FUNCTION__ << endl; }

int main() {
    int x = 42;
    int *p = &x;
    const int *cp = &x;

    g(p);    // both exact; (B) is more specialized
    g(cp);   // same, with T = const int
    g(x);    // not a pointer: only (A) is viable
    return 0;
}
