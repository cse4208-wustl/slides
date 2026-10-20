// Module 13 -- Q2: Explicit template arguments
// Build & run:  make q2_demo
#include <iostream>
#include <string>
#include <memory>
#include <algorithm>
#include <climits>
using namespace std;

template <typename T> int compare(const T &v1, const T &v2) {
    if (v1 < v2) return -1;
    if (v2 < v1) return 1;
    return 0;
}

// T1 cannot be deduced: it appears only in the return type
template <typename T1, typename T2, typename T3>
T1 sum(T2 a, T3 b) { return a + b; }

// poor design: the type the caller must give is LAST
template <typename T1, typename T2, typename T3>
T3 alternative_sum(T1 a, T2 b) { return a + b; }

int main() {
    int i = INT_MAX;
    long lng = 1;

    // T1 is given; T2 = int and T3 = long are deduced
    cout << "sum<long long>(i, lng) = " << sum<long long>(i, lng) << endl;
    cout << "sum<short>(i, lng)     = " << sum<short>(i, lng) << endl;   // too small!

    // auto v = alternative_sum<long long>(i, lng);   // (A) error: T3 can't be deduced
    cout << "alternative_sum<int, long, long long>(i, lng) = "
         << alternative_sum<int, long, long long>(i, lng) << endl;

    // explicit argument: normal conversions apply again
    cout << "compare<long>(lng, 1024) = " << compare<long>(lng, 1024) << endl;
    double d = 2.9;
    cout << "compare<int>(d, 2)       = " << compare<int>(d, 2) << endl;   // 2.9 -> 2

    // std::max has ONE type parameter
    // cout << max(1, 2.5) << endl;                 // (B) error: int vs. double
    cout << "max<double>(1, 2.5) = " << max<double>(1, 2.5) << endl;

    // make_unique: the arguments are for the constructor, not the type
    unique_ptr<string> p = make_unique<string>(3, 'x');
    cout << "make_unique<string>(3, 'x') -> " << *p << endl;
    return 0;
}
