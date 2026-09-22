#include <iostream>
#include <vector>
#include <algorithm>
#include <functional>
using namespace std;
using namespace std::placeholders;

// A hand-written function-object class, equivalent to a captureless lambda (§14.8.1)
class SquareFO {
public:
    int operator()(int x) const { return x * x; }
};

// Returns a lambda SAFELY -- factor is captured by value.
// Uses an explicit trailing return type (function<int(int)>) rather than
// deduced `auto`, so this compiles under C++11 as well as later standards.
auto makeMultiplier(int factor) -> function<int(int)> {
    return [factor](int x) { return x * factor; };
}

int main() {
    // 1. Basic lambda: capture list, parameter list, trailing return type
    auto square = [](int x) -> int { return x * x; };
    SquareFO fo = SquareFO();
    cout << "lambda: " << square(6) << "  functor: " << fo(6) << endl;
    auto square2 = [](int x) { 
       if (x > 0) {
          return x * x; 
       }
       else {
          return x - x;
       }
    };
    cout << square2(5) << endl;
    // 2. Capture by value vs. by reference (timing)
    int count = 0;
    auto byValue = [count]() { return count; };
    auto byRef   = [&count]() { return count; };
    count = 42;
    cout << "byValue(): " << byValue() << "  byRef(): " << byRef() << endl;

    // 3. mutable -- lambda keeps its own private, modifiable copy
    int base = 100;
    auto counter = [base]() mutable { return ++base; };
    cout << counter() << " " << counter() << "  base is still " << base << endl;

    // 4. Implicit capture mixed with an explicit reference capture
    int threshold = 5;
    vector<int> nums = {1, 6, 3, 9, 2, 8};
    for_each(nums.begin(), nums.end(), [=](int x) {
        if (x > threshold) cout << x << " ";
    });
    cout << endl;

    // 5. Safely returning a lambda (capture by value)
    auto triple = makeMultiplier(3);
    cout << "triple(7) = " << triple(7) << endl;

    // 6. bind with placeholders
    auto subtract = [](int a, int b) { return a - b; };
    auto reversedSubtract = bind(subtract, _2, _1);
    cout << "reversedSubtract(10, 3) = " << reversedSubtract(10, 3) << endl;

    auto subtractFrom100 = bind(subtract, 100, _1);
    cout << "subtractFrom100(30) = " << subtractFrom100(30) << endl;

    return 0;
}
