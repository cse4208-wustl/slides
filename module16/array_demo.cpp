#include <iostream>
#include <vector>
using namespace std;

// primary template: one T per element
template <typename T, size_t N>
class Array {
public:
    T get(size_t i) const { return data[i]; }
    void set(size_t i, T value) { data[i] = value; }
    size_t size() const { return N; }
private:
    T data[N] = {};
};

// partial specialization for bool: 8 elements per byte
template <size_t N>
class Array<bool, N> {
public:
    bool get(size_t i) const { return bits[i / 8] & (1 << (i % 8)); }
    void set(size_t i, bool value) {
        if (value)
            bits[i / 8] |= (1 << (i % 8));
        else
            bits[i / 8] &= ~(1 << (i % 8));
    }
    size_t size() const { return N; }
private:
    unsigned char bits[(N + 7) / 8] = {};
};

// generic code: works with either version, because the interface is the same
template <typename T, size_t N>
void print(const Array<T, N>& a) {
    for (size_t i = 0; i < a.size(); ++i)
        cout << a.get(i) << ' ';
    cout << endl;
}

int main() {
    cout << "--- Array<T, N> ---" << endl;
    Array<int, 10> nums;
    Array<bool, 10> flags;
    for (size_t i = 0; i < 10; ++i) {
        nums.set(i, i * i);
        flags.set(i, i % 3 == 0);
    }
    print(nums);
    print(flags);

    cout << "sizeof(Array<int, 64>):  " << sizeof(Array<int, 64>) << endl;
    cout << "sizeof(Array<char, 64>): " << sizeof(Array<char, 64>) << endl;
    cout << "sizeof(Array<bool, 64>): " << sizeof(Array<bool, 64>) << endl;

    cout << endl << "--- std::vector<bool> ---" << endl;
    vector<bool> v = {true, true, true};
    // bool& r = v[0];      // error: v[0] is not a bool
    auto x = v[0];          // x is a vector<bool>::reference, not a bool
    // cout << "v[0] has type " << typeid(x).name() << endl;
    x = false;              // ...so this changes v[0]
    cout << boolalpha << "v[0] = " << v[0] << endl;
}
