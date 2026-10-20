// Module 13 -- Q3: Trailing return types and type transformations
// Build & run:  make q3_demo
#include <iostream>
#include <string>
#include <vector>
#include <type_traits>
#include <typeinfo>
using namespace std;

// returns a REFERENCE to the first element: *beg is an lvalue, so
// decltype(*beg) is int& (for a vector<int>)
template <typename It>
auto fcn(It beg, It /* end */) -> decltype(*beg) {
    return *beg;
}

// returns a COPY of the first element: remove_reference turns int& into int
template <typename It>
auto fcn2(It beg, It /* end */) -> typename remove_reference<decltype(*beg)>::type {
    return *beg;
}

int main() {
    vector<int> vi = {1, 2, 3};

    fcn(vi.begin(), vi.end()) = 100;       // ok: changes vi[0]
    // fcn2(vi.begin(), vi.end()) = 200;   // (A) error: can't assign to a copy
    int x = fcn2(vi.begin(), vi.end());
    x = 7;                                 // changes x only
    cout << "vi[0] = " << vi[0] << ", x = " << x << endl;

    // a few type transformations from <type_traits>
    int n = 1;
    remove_reference<int &>::type copy = n;      // int: a separate copy
    copy = 99;
    cout << "n = " << n << ", copy = " << copy << endl;

    add_const<int>::type c = 5;                  // const int
    // c = 6;                                    // (B) error: c is const

    make_unsigned<int>::type u = -1;             // unsigned int
    cout << "make_unsigned<int>::type u = -1 -> " << u << endl;

    remove_extent<int[3][4]>::type row = {1, 2, 3, 4};   // int[4]
    cout << "row has " << sizeof(row) / sizeof(row[0]) << " elements, c = " << c << endl;

    // C++14 shorthand: _t instead of typename ... ::type
    remove_reference_t<int &> copy2 = n;
    cout << "copy2 = " << copy2 << endl;

    return 0;
}
