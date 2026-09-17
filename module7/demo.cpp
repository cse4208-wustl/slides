#include <iostream>
#include <string>
using namespace std;

bool lengthCompare(const string &s1, const string &s2) {
    cout << "  lengthCompare(" << s1 << ", " << s2 << ")\n";
    return s1.size() < s2.size();
}

bool alphaCompare(const string &s1, const string &s2) {
    cout << "  alphaCompare(" << s1 << ", " << s2 << ")\n";
    return s1 < s2;
}

// a type alias for "pointer to function(const string&, const string&) -> bool"
using CompareFn = bool (*)(const string &, const string &);
// equivalent alternative: using CompareFn2 = decltype(lengthCompare) *;
// NOTE: decltype(lengthCompare) alone -- with no '*' -- names the function
// type, not a pointer. You cannot declare a plain variable with it.

// function pointer as a parameter
void useComparator(const string &s1, const string &s2, CompareFn cmp) {
    cout << (cmp(s1, s2) ? "true" : "false") << '\n';
}

// function returning a pointer to function, via trailing return
auto getComparator(bool byLength) -> CompareFn {
    if (byLength)
        return lengthCompare;   // auto-converted: function name -> pointer
    else
        return alphaCompare;
}

int main() {
    // declare, initialize, and call through a function pointer
    // 1. without alias
    bool (*pf1)(const string &s1, const string &s2) = lengthCompare;
    cout << "Direct call:       " << pf1("hi", "there") << '\n';
    cout << "Dereferenced call: " << (*pf1)("hi", "there") << '\n';

    // 2. through CompareFn alias
    CompareFn pf = lengthCompare;                 // pf = &lengthCompare is equivalent
    cout << "Direct call:       " << pf("hi", "there") << '\n';
    cout << "Dereferenced call: " << (*pf)("hi", "there") << '\n';

    // pass function pointer as argument
    cout << "\n--- passing a function pointer as an argument ---\n";
    useComparator("hi", "there", lengthCompare);
    useComparator("hi", "there", alphaCompare);

    // have a function return a function pointer
    cout << "\n--- getting a function pointer back from a function ---\n";
    CompareFn chosen = getComparator(true);
    cout << "chosen(\"cat\", \"caterpillar\") = "
         << chosen("cat", "caterpillar") << '\n';
}
