#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <algorithm>
#include <functional>
using namespace std;
using namespace std::placeholders;

// ---------------------------------------------------------------------------
// Base / derived classes with public AND private data members of the same type
// ---------------------------------------------------------------------------
class Person {
public:
    Person(const string &f, const string &l, const string &e)
        : first(f), last(l), email_(e) {}
    virtual ~Person() = default;

    string first;                         // public data members
    string last;

    virtual string describe() const { return "Person  " + first + " " + last; }
    string greet(const string &hello) const { return hello + ", " + first + "!"; }

    // "Controlled violation of encapsulation": hands out a pointer to a
    // PRIVATE member without handing out any particular object's data.
    static const string Person::*emailMember() { return &Person::email_; }

private:
    string email_;
};

class Student : public Person {
public:
    Student(const string &f, const string &l, const string &e,
            const string &id, const string &m)
        : Person(f, l, e), major(m), id_(id) {}

    string major;                         // public
    string describe() const override {
        return "Student " + first + " " + last + " (" + major + ")";
    }
    static const string Student::*idMember() { return &Student::id_; }

private:
    string id_;
};

// ---------------------------------------------------------------------------
// A pointer-to-member-function table (same idea as Screen::Menu, LLM 19.4.2)
// ---------------------------------------------------------------------------
class Cursor {
public:
    using Action = Cursor &(Cursor::*)();          // type alias for readability
    enum Direction { HOME, FORWARD, BACK };

    Cursor &home()    { pos_ = 0; return *this; }
    Cursor &forward() { ++pos_;   return *this; }
    Cursor &back()    { if (pos_ > 0) --pos_; return *this; }

    Cursor &move(Direction d) { return (this->*Menu[d])(); }   // note the parens
    size_t where() const { return pos_; }

private:
    size_t pos_ = 0;
    static Action Menu[];
};

Cursor::Action Cursor::Menu[] = { &Cursor::home, &Cursor::forward, &Cursor::back };

int main() {
    vector<Student> roster = {
        {"Ada",   "Lovelace", "ada@wustl.edu",   "S001", "Math"},
        {"Alan",  "Turing",   "alan@wustl.edu",  "S002", "CSE"},
        {"Grace", "Hopper",   "grace@wustl.edu", "S003", "CSE"},
    };

    cout << "=== 1. Pointers to data members ===" << endl;
    // Each column is a pointer to a (const) string member of Student.
    // &Student::last is really a  string Person::*  -> converts to Student::*
    vector<const string Student::*> columns = {
        &Student::last,
        &Student::first,
        &Student::major,
        Student::idMember(),              // private member of Student
        Person::emailMember(),            // private member of the BASE class
    };
    for (const Student &s : roster) {
        for (auto col : columns) {
            cout << left << setw(17) << s.*col;          // .*  : object
        }
        cout << endl;
    }

    const Student *ps = &roster[1];
    auto pLast = &Student::last;
    cout << "via ->* : " << ps->*pLast << endl;         // ->* : pointer to object

    cout << "\n=== 2. Pointers to member functions ===" << endl;
    using Describe = string (Person::*)() const;        // alias
    Describe pd = &Person::describe;                    // & is REQUIRED
    Person  prof("Kate", "Holdener", "holdener@wustl.edu");
    Person &ref = roster[2];                            // Person& bound to a Student
    cout << (prof.*pd)() << endl;
    cout << (ref.*pd)()  << "   <- virtual dispatch still happens" << endl;

    auto pg = &Person::greet;       // string (Person::*)(const string&) const
    cout << (roster[0].*pg)("Hello") << endl;

    cout << "\n=== 3. Member function table ===" << endl;
    Cursor c;
    c.move(Cursor::FORWARD).move(Cursor::FORWARD).move(Cursor::FORWARD);
    cout << "after 3 x FORWARD: " << c.where() << endl;
    c.move(Cursor::BACK);
    cout << "after BACK:        " << c.where() << endl;
    c.move(Cursor::HOME);
    cout << "after HOME:        " << c.where() << endl;

    cout << "\n=== 4. Member functions as callable objects ===" << endl;
    vector<string> words = {"alpha", "beta", "", "delta"};

    // find_if(words.begin(), words.end(), &string::empty);   // won't compile

    function<bool (const string &)> fcn = &string::empty;
    auto it1 = find_if(words.begin(), words.end(), fcn);
    auto it2 = find_if(words.begin(), words.end(), mem_fn(&string::empty));
    auto it3 = find_if(words.begin(), words.end(), bind(&string::empty, _1));
    cout << "empty string at index " << (it1 - words.begin()) << ", "
         << (it2 - words.begin()) << ", " << (it3 - words.begin()) << endl;

    // mem_fn works on objects AND pointers, and on data members too
    auto describe = mem_fn(&Person::describe);
    vector<Person *> people = {&prof, &roster[0], &roster[1]};
    for_each(people.begin(), people.end(),
             [&](Person *p) { cout << describe(p) << endl; });

    auto lastName = mem_fn(&Person::last);
    cout << "lastName(roster[2]) = " << lastName(roster[2]) << endl;

    // bind can also fix the NON-object arguments of a member function
    auto sayHi = bind(&Person::greet, _1, "Hi");
    cout << sayHi(prof) << "  " << sayHi(&roster[1]) << endl;

    return 0;
}
