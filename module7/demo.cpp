#include <iostream>
#include <string>
using namespace std;

// Q3/Q4: candidate comparison functions -- same signature, different behavior
bool lengthCompare(const string &s1, const string &s2) {
    cout << "  lengthCompare(" << s1 << ", " << s2 << ")\n";
    return s1.size() < s2.size();
}

bool alphaCompare(const string &s1, const string &s2) {
    cout << "  alphaCompare(" << s1 << ", " << s2 << ")\n";
    return s1 < s2;
}

// Q5: a type alias for "pointer to function(const string&, const string&) -> bool"
using CompareFn = bool (*)(const string &, const string &);
// equivalent alternative: using CompareFn2 = decltype(lengthCompare) *;
// NOTE (Q1): decltype(lengthCompare) alone -- with no '*' -- names the function
// type, not a pointer. You cannot declare a plain variable with it.

// Q5: function pointer as a parameter
void useComparator(const string &s1, const string &s2, CompareFn cmp) {
    cout << (cmp(s1, s2) ? "true" : "false") << '\n';
}

// Q6: function returning a pointer to function, via trailing return
auto getComparator(bool byLength) -> CompareFn {
    if (byLength)
        return lengthCompare;   // auto-converted: function name -> pointer
    else
        return alphaCompare;
}

int main() {
    // Q2/Q3: declare, initialize, and call through a function pointer
    CompareFn pf = lengthCompare;                 // pf = &lengthCompare is equivalent
    cout << "Direct call:       " << pf("hi", "there") << '\n';
    cout << "Dereferenced call: " << (*pf)("hi", "there") << '\n';

    cout << "\n--- passing a function pointer as an argument ---\n";
    useComparator("hi", "there", lengthCompare);
    useComparator("hi", "there", alphaCompare);

    cout << "\n--- getting a function pointer back from a function ---\n";
    CompareFn chosen = getComparator(true);
    cout << "chosen(\"cat\", \"caterpillar\") = "
         << chosen("cat", "caterpillar") << '\n';
}
