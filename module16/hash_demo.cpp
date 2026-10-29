#include <iostream>
#include <string>
#include <unordered_map>
using namespace std;

struct Student {
    string name;
    int id;

    // unordered containers need == to tell keys apart when hashes collide
    bool operator==(const Student& other) const {
        return name == other.name && id == other.id;
    }
};

// Solution 1: specialize std::hash for Student.
// unordered_map<Student, int> uses std::hash<Student> by default.
// (Comment out this block to see the compiler error.)
namespace std {
    template <>
    struct hash<Student> {
        size_t operator()(const Student& s) const {
            cout << "  " << __PRETTY_FUNCTION__ << endl;
            size_t h1 = hash<string>{}(s.name);
            size_t h2 = hash<int>{}(s.id);
            return h1 ^ (h2 << 1);
        }
    };
}

// Solution 2: a separate hash functor, passed as the 3rd template argument
struct StudentHash {
    size_t operator()(const Student& s) const {
        cout << "  " << __PRETTY_FUNCTION__ << endl;
        size_t h1 = hash<string>{}(s.name);
        size_t h2 = hash<int>{}(s.id);
        return h1 ^ (h2 << 1);
    }
};

int main() {
    cout << "--- string -> int ---" << endl;
    unordered_map<string, int> counts;
    for (string word : {"how", "now", "brown", "cow", "how", "now"})
        ++counts[word];
    for (const auto& p : counts)
        cout << p.first << ": " << p.second << endl;

    cout << endl << "--- Student -> int, std::hash<Student> ---" << endl;
    unordered_map<Student, int> scores;
    scores[{"Kate", 101}] = 95;
    scores[{"Alice", 102}] = 88;
    int alice = scores[{"Alice", 102}];
    cout << "Alice: " << alice << endl;

    cout << endl << "--- Student -> int, StudentHash ---" << endl;
    unordered_map<Student, int, StudentHash> scores2;
    scores2[{"Kate", 101}] = 95;
    scores2[{"Alice", 102}] = 88;
    int alice2 = scores2[{"Alice", 102}];
    cout << "Alice: " << alice2 << endl;
}
