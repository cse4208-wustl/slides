// Module 11: Copy Control and Move Semantics -- live demo
// Build & run:  make run        Sanitizer build:  make asan
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <utility>
#include <algorithm>
using namespace std;

// Counters for section 4, where printing every call would be too noisy
int  copies  = 0, moves = 0;
bool verbose = true;

void step(const string& s) { cout << "  " << s << endl; }

// ---------------------------------------------------------------------------
// Buffer: a VALUELIKE class that owns a dynamic array (needs the Rule of 3/5)
// Every copy-control member prints a line, so we can SEE which one runs.
// ---------------------------------------------------------------------------
class Buffer {
    string name_;
    size_t n_;
    int   *data_;

    void log(const string& what) const {
        if (verbose) cout << "    " << what << " [" << name_ << ", n=" << n_ << "]" << endl;
    }

public:
    Buffer() : Buffer("empty", 0) {}
    Buffer(const string& name, size_t n)
        : name_(name), n_(n), data_(n ? new int[n]() : nullptr) {
        log("ctor       ");
    }

    // copy constructor
    Buffer(const Buffer& rhs)
        : name_(rhs.name_ + "-copy"), n_(rhs.n_), data_(rhs.n_ ? new int[rhs.n_] : nullptr) {
        std::copy(rhs.data_, rhs.data_ + n_, data_);       // DEEP copy
        ++copies; log("COPY ctor  ");
    }

    // copy assignment
    Buffer& operator=(const Buffer& rhs) {
        // allocate + copy FIRST, then free: correct even for self-assignment (x = x)
        int *newdata = rhs.n_ ? new int[rhs.n_] : nullptr;
        std::copy(rhs.data_, rhs.data_ + rhs.n_, newdata);
        delete[] data_;
        data_ = newdata;
        n_    = rhs.n_;
        name_ = rhs.name_ + "-copy";
        log("COPY assign");
        return *this;
    }

    // move constructor: steal the array, leave rhs destructible
    Buffer(Buffer&& rhs) noexcept
        : name_(std::move(rhs.name_)), n_(rhs.n_), data_(rhs.data_) {
        rhs.data_ = nullptr;
        rhs.n_    = 0;
        rhs.name_ = "moved-from";
        ++moves; log("MOVE ctor  ");
    }

    // move assignment
    Buffer& operator=(Buffer&& rhs) noexcept {
        if (this != &rhs) {                 // rhs could be std::move(*this)
            delete[] data_;                 // free our old array
            data_ = rhs.data_;              // take over rhs's array
            n_    = rhs.n_;
            name_ = std::move(rhs.name_);
            rhs.data_ = nullptr;            // leave rhs destructible
            rhs.n_    = 0;
            rhs.name_ = "moved-from";
        }
        log("MOVE assign");
        return *this;
    }

    // copy-and-swap: ONE operator= that replaces BOTH operator= above
    // (to try it: comment out the two above, uncomment this one)
    // Buffer& operator=(Buffer rhs) {
    //     log("assign/swap");
    //     swap(*this, rhs);            // *this gets rhs's resources
    //     return *this;                // rhs (holding our OLD array) is destroyed here
    // }

    ~Buffer() {
        log("dtor       ");
        delete[] data_;                     // members (name_) destroyed AFTER this body
    }

    // type-specific swap: exchange pointers, no allocation, no copying
    friend void swap(Buffer& a, Buffer& b) noexcept {
        using std::swap;
        swap(a.name_, b.name_);
        swap(a.n_,    b.n_);
        swap(a.data_, b.data_);
        if (verbose) cout << "    swap        (just pointers)" << endl;
    }

    const string& name() const { return name_; }
    size_t size() const { return n_; }
};

Buffer make_buffer(const string& name) {
    Buffer local(name, 2);
    return local;                           // copy/move usually ELIDED
}

// ---------------------------------------------------------------------------
// 1. swap and copy-and-swap
// ---------------------------------------------------------------------------
void swapping() {
    Buffer x("x", 1000), y("y", 5);
    step("using std::swap; swap(x, y);");
    using std::swap;
    swap(x, y);                                        // finds friend swap by ADL
    cout << "    x = " << x.name() << "(" << x.size() << "), y = "
         << y.name() << "(" << y.size() << ")" << endl;
    step("x = y;");
    x = y;
    step("x = x;   (self-assignment)");
    x = x;
    step("leaving scope");
}

// ---------------------------------------------------------------------------
// 2. lvalues, rvalues, and references to them
// ---------------------------------------------------------------------------
void which(const Buffer& b) { cout << "    which(const Buffer&) <- " << b.name() << endl; }
void which(Buffer&& b)      { cout << "    which(Buffer&&)      <- " << b.name() << endl; }

void rvalue_refs() {
    int i = 42;
    int& r = i;                     // lvalue ref binds to lvalue
    // int& r2 = i * 42;            // (A) error: rvalue to lvalue ref
    const int& r3 = i * 42;         // OK: const & binds to anything
    int&& rr = i * 42;              // rvalue ref binds to rvalue
    // int&& rr2 = i;               // (B) error: lvalue to rvalue ref
    // int&& rr3 = rr;              // (C) error: rr is a VARIABLE -> lvalue!
    int&& rr4 = std::move(i);       // OK: we promise not to use i's value
    cout << "    r=" << r << " r3=" << r3 << " rr=" << rr << " rr4=" << rr4 << endl;

    Buffer a("a", 1);
    step("which(a);");                       which(a);
    step("which(Buffer(\"temp\", 1));");     which(Buffer("temp", 1));
    step("which(make_buffer(\"ret\"));");    which(make_buffer("ret"));
    step("which(std::move(a));");            which(std::move(a));
    cout << "    a is still \"" << a.name() << "\": std::move is only a CAST" << endl;
    step("leaving scope");
}

// ---------------------------------------------------------------------------
// 3. Move constructor and move assignment
// ---------------------------------------------------------------------------
void move_semantics() {
    Buffer a("big", 1'000'000);
    step("Buffer b = std::move(a);");
    Buffer b = std::move(a);                           // steal, don't copy 4 MB
    cout << "    a = " << a.name() << "(" << a.size() << "), b = "
         << b.name() << "(" << b.size() << ")" << endl;
    step("a = Buffer(\"reborn\", 2);   // a moved-from object may be assigned...");
    a = Buffer("reborn", 2);                           // temporary -> move assignment
    step("Buffer c; c = std::move(b);");
    Buffer c;
    c = std::move(b);
    step("v.push_back(a);   v.push_back(Buffer(\"tmp\", 1));");
    vector<Buffer> v;
    v.reserve(2);
    v.push_back(a);                                    // lvalue: copied
    v.push_back(Buffer("tmp", 1));                     // rvalue: moved
    step("leaving scope   (... or destroyed)");
}

// ---------------------------------------------------------------------------
// 4. noexcept matters: vector reallocation
// ---------------------------------------------------------------------------
void vector_growth() {
    verbose = false;
    copies = moves = 0;
    {
        vector<Buffer> v;
        for (int i = 0; i < 17; ++i)
            v.emplace_back("v" + to_string(i), 100);   // constructed in place
        cout << "    17 emplace_backs, capacity " << v.capacity()
             << ": copies = " << copies << ", moves = " << moves << endl;
    }
    verbose = true;
}

// ---------------------------------------------------------------------------
// 5. Move-only types: unique_ptr
// ---------------------------------------------------------------------------
void unique_moves() {
    auto p = make_unique<Buffer>("u", 4);
    // auto q2 = p;                                    // (D) copy ctor is deleted
    auto q = std::move(p);                             // ownership moves; Buffer untouched
    cout << "    p is " << (p ? "non-null" : "null") << ", q owns " << q->name() << endl;
    vector<unique_ptr<Buffer>> owners;
    // owners.push_back(q);                            // (E) would copy -> error
    owners.push_back(std::move(q));
    step("leaving scope");
}

int main() {
    cout << "== 1. swap ==" << endl;                  swapping();
    cout << "\n== 2. lvalues and rvalues ==" << endl; rvalue_refs();
    cout << "\n== 3. move semantics ==" << endl;      move_semantics();
    cout << "\n== 4. noexcept and vector ==" << endl; vector_growth();
    cout << "\n== 5. unique_ptr ==" << endl;          unique_moves();
    return 0;
}
