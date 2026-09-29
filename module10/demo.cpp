// Module 10: Dynamic Memory Management and Smart Pointers -- live demo
// Build & run:  make run        Sanitizer build:  make asan
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <new>
#include <cstdlib>
#include <stdexcept>
#include <utility>
using namespace std;

// Every construction (+) and destruction (-) is printed,
// so we can SEE exactly when dynamic objects are created and freed.
struct Widget {
    int id;
    explicit Widget(int i) : id(i) { cout << "  + Widget " << id << endl; }
    ~Widget() { cout << "  - Widget " << id << endl; }
};

// ---------------------------------------------------------------------------
// 1. Managing memory directly with new / delete
// ---------------------------------------------------------------------------
void raw_pointers() {
    Widget *a = new Widget(1);
    Widget *b = new Widget(2);
    Widget *c = b;              // two pointers, ONE object
    cout << "  c->id = " << c->id << endl; 
    delete b;                   // Widget 2 freed -> c is now dangling
    b = nullptr;                // protects b only, NOT c
    // cout << "  c->id = " << c->id << endl;   // (1) use after delete: UB
    // delete c;                                // (2) double delete:    UB
    a = new Widget(3);          // (3) leak: nobody points to Widget 1 anymore
    delete a;
}   // Look at the output: "- Widget 1" is never printed

// ---------------------------------------------------------------------------
// 2. shared_ptr and reference counting
// ---------------------------------------------------------------------------
shared_ptr<Widget> factory(int id) {
    return make_shared<Widget>(id);        // caller shares ownership
}

void shared_pointers() {
    auto p = factory(10);
    cout << "  p.use_count()      = " << p.use_count() << endl;
    auto q(p);                                         // copy: count++
    cout << "  after auto q(p)    = " << p.use_count() << endl;
    auto r = make_shared<Widget>(11);
    cout << "  r = q ..." << endl;
    r = q;                                             // Widget 11 has no owners -> freed
    cout << "  after r = q        = " << p.use_count() << endl;
    {
        vector<shared_ptr<Widget>> v(3, p);            // 3 more owners
        cout << "  vector of 3 copies = " << p.use_count() << endl;
    }                                                  // vector destroyed: count -= 3
    cout << "  vector destroyed   = " << p.use_count() << endl;
    p.reset();                                         // p lets go
    cout << "  after p.reset()    = " << q.use_count()
         << "  (p is " << (p ? "non-null" : "null") << ")" << endl;
    cout << "  leaving scope ..." << endl;
}   // q and r destroyed -> count 0 -> Widget 10 freed

// ---------------------------------------------------------------------------
// 3. Don't mix ordinary pointers and smart pointers
// ---------------------------------------------------------------------------
void process(shared_ptr<Widget> ptr) {                 // by value: count++
    cout << "  process: Widget " << ptr->id
         << ", use_count = " << ptr.use_count() << endl;
}

void mixing() {
    auto sp = make_shared<Widget>(20);
    process(sp);
    cout << "  back in caller, use_count = " << sp.use_count() << endl;

    Widget *x = new Widget(21);
    // process(x);                         // error: no implicit int* -> shared_ptr
    process(shared_ptr<Widget>(x));        // temporary takes ownership ...
    cout << "  ... Widget 21 is already gone; x is dangling" << endl;
    // cout << "  x->id = " << x->id << endl;          // UB

    Widget *observer = sp.get();           // OK: only *observing*
    cout << "  observer sees Widget " << observer->id << endl;
    // process(shared_ptr<Widget>(sp.get()));  // 2nd control block -> double delete
    // delete sp.get();                        // double delete
}

// ---------------------------------------------------------------------------
// 4. unique_ptr: exclusive ownership, moved up/down the call stack
// ---------------------------------------------------------------------------
unique_ptr<Widget> make_widget(int id) {
    unique_ptr<Widget> ret(new Widget(id));   // C++14: make_unique<Widget>(id)
    return ret;                               // OK: ret is about to be destroyed
}                                             //     -> ownership moves UP

void consume(unique_ptr<Widget> w) {          // takes ownership
    cout << "  consume owns Widget " << w->id << endl;
}                                             // Widget destroyed here

void unique_pointers() {
    unique_ptr<Widget> p1 = make_widget(30);
    // unique_ptr<Widget> p2(p1);             // error: copy constructor is deleted
    unique_ptr<Widget> p2(p1.release());      // transfer: p1 becomes null
    cout << "  p1 is " << (p1 ? "non-null" : "null") << endl;

    unique_ptr<Widget> p3(new Widget(31));
    cout << "  p2.reset(p3.release()) ..." << endl;
    p2.reset(p3.release());                   // Widget 30 freed; p2 owns 31

    consume(std::move(p2));                   // ownership moves DOWN
    cout << "  after consume, p2 is " << (p2 ? "non-null" : "null") << endl;

    auto p4 = make_unique<Widget>(32);
    // p4.release();                          // leak: Widget 32 never freed
    cout << "  leaving scope ..." << endl;
}

// ---------------------------------------------------------------------------
// 5. weak_ptr: observe without owning; lock() before use
// ---------------------------------------------------------------------------
void weak_pointers() {
    weak_ptr<Widget> wp;
    {
        auto sp = make_shared<Widget>(40);
        wp = sp;                              // does NOT change the count
        cout << boolalpha;
        cout << "  use_count = " << wp.use_count()
             << ", expired = " << wp.expired() << endl;
        if (auto np = wp.lock())              // temporary owner
            cout << "  locked Widget " << np->id
                 << ", use_count = " << np.use_count() << endl;
    }                                         // last owner gone -> Widget 40 freed
    cout << "  use_count = " << wp.use_count()
         << ", expired = " << wp.expired() << endl;
    if (auto np = wp.lock())
        cout << "  still alive: " << np->id << endl;
    else
        cout << "  lock() returned null" << endl;
    // cout << wp->id << endl;               // error: weak_ptr has no -> or *
    cout << noboolalpha;
}

// A doubly-linked pair of nodes: owning link forward, weak link back
struct Node {
    string name;
    shared_ptr<Node> next;
    weak_ptr<Node>   prev;      // try: shared_ptr<Node> prev;  -> cycle -> leak
    explicit Node(string n) : name(std::move(n)) { cout << "  + Node " << name << endl; }
    ~Node() { cout << "  - Node " << name << endl; }
};

void cycle() {
    auto a = make_shared<Node>("A");
    auto b = make_shared<Node>("B");
    a->next = b;
    b->prev = a;
    cout << "  a.use_count = " << a.use_count()
         << ", b.use_count = " << b.use_count() << endl;
    cout << "  leaving scope ..." << endl;
}

int main() {
    cout << "=== 1. Raw new/delete ===" << endl;       raw_pointers();
    cout << "\n=== 2. shared_ptr ===" << endl;         shared_pointers();
    cout << "\n=== 3. Mixing raw & smart ===" << endl; mixing();
    cout << "\n=== 4. unique_ptr ===" << endl;         unique_pointers();
    cout << "\n=== 5a. weak_ptr ===" << endl;          weak_pointers();
    cout << "\n=== 5b. Breaking a cycle ===" << endl;  cycle();
    cout << "\n=== end of main ===" << endl;
    return 0;
}
