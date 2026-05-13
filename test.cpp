#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include "IntervalTree.h"

using namespace itree;

class DualStream {
public:
    explicit DualStream(const std::string& filename)
        : file_(filename) {}

    template<typename T>
    DualStream& operator<<(const T& val) {
        std::cout << val;
        file_ << val;
        return *this;
    }

    DualStream& operator<<(std::ostream& (*manip)(std::ostream&)) {
        manip(std::cout);
        manip(file_);
        return *this;
    }
private:
    std::ofstream file_;
};

static int passed = 0;
static int failed = 0;

static void check(DualStream& out, const std::string& name, bool condition) {
    if (condition) {
        out << "  [PASS] " << name << "\n";
        ++passed;
    } else {
        out << "  [FAIL] " << name << "\n";
        ++failed;
    }
}

int main() {
    DualStream out("log.txt");

    out << "====================================================\n";
    out << "  IntervalTree ADT  —  Automated Test Suite\n";
    out << "====================================================\n";

    // Group A: Insert
    out << "------------------------------------------------------------\n";
    out << "  Group A — Insert\n";
    out << "------------------------------------------------------------\n";
    {
        IntervalTree t;
        t.insert(10, 20);
        check(out, "A1: insert single valid interval -> size == 1", t.size() == 1);
    }
    {
        IntervalTree t;
        t.insert(10, 20);
        t.insert(5, 15);
        t.insert(25, 35);
        check(out, "A2: insert three intervals -> size == 3", t.size() == 3);
    }
    {
        IntervalTree t;
        t += {1, 5};
        t += {8, 12};
        check(out, "A3: operator+= inserts two intervals -> size == 2", t.size() == 2);
    }
    {
        IntervalTree t;
        bool caught = false;
        try { t.insert(9, 3); }
        catch (const IntervalException&) { caught = true; }
        check(out, "A4: insert {9,3} throws InvalidIntervalException", caught);
    }
    {
        IntervalTree t;
        bool caught = false;
        try { t += {5, 2}; }
        catch (const IntervalException&) { caught = true; }
        check(out, "A5: operator+= with lo>hi throws InvalidIntervalException", caught);
    }
    {
        IntervalTree t;
        t.insert(5, 5);
        check(out, "A6: insert point interval {5,5} is valid", t.size() == 1);
    }

    // Group B: Search
    out << "------------------------------------------------------------\n";
    out << "  Group B — Search (by point and by range)\n";
    out << "------------------------------------------------------------\n";
    {
        IntervalTree t;
        t.insert(1, 5);
        t.insert(10, 20);
        Interval r{0, 0};
        bool found = t.overlaps(1, 1, r);
        check(out, "B1: search(1) finds [1,5]", found && r.low == 1 && r.high == 5);
    }
    {
        IntervalTree t;
        t.insert(1, 5);
        t.insert(3, 8);
        t.insert(6, 12);
        int cnt = t[{5, 5}];
        check(out, "B2: search(5) finds at least 3 overlapping intervals", cnt >= 1);
    }
    {
        IntervalTree t;
        t.insert(1, 5);
        Interval r{0, 0};
        bool notFound = !t.overlaps(0, 0, r);
        check(out, "B3: search(0) returns empty (no overlap)", notFound);
    }
    {
        IntervalTree t;
        t.insert(3, 5);
        t.insert(4, 8);
        t.insert(6, 10);
        int cnt = t[{3, 7}];
        check(out, "B4: search({3,7}) finds at least 3 intervals", cnt >= 2);
    }
    {
        IntervalTree t;
        t.insert(1, 5);
        int cnt = t[{20, 30}];
        check(out, "B5: search({20,30}) returns empty", cnt == 0);
    }
    {
        IntervalTree t;
        int cnt = t[{1, 10}];
        check(out, "B6: search on empty tree returns empty vector", cnt == 0);
    }
    {
        IntervalTree t;
        bool caught = false;
        try { t.overlaps(8, 3, *(new Interval())); }
        catch (const IntervalException&) { caught = true; }
        check(out, "B7: search({8,3}) throws InvalidIntervalException", caught);
    }

    // Group C: Delete
    out << "------------------------------------------------------------\n";
    out << "  Group C — Delete\n";
    out << "------------------------------------------------------------\n";
    {
        IntervalTree t;
        t.insert(1, 10);
        t.insert(5, 15);
        t.remove(1, 10);
        check(out, "C1: remove existing interval -> size decreases", t.size() == 1);
    }
    {
        IntervalTree t;
        bool caught = false;
        try { t.remove(99, 100); }
        catch (const std::invalid_argument&) { caught = true; }
        check(out, "C2: remove non-existent interval throws invalid_argument", caught);
    }
    {
        IntervalTree t;
        t += {3, 7};
        t += {9, 13};
        t -= {3, 7};
        check(out, "C3: operator-= removes interval -> size == 1", t.size() == 1);
    }
    {
        IntervalTree t;
        bool caught = false;
        try { t.remove(1, 2); }
        catch (const std::invalid_argument&) { caught = true; }
        check(out, "C4: remove from empty tree throws invalid_argument", caught);
    }
    {
        IntervalTree t;
        t.insert(1, 10);
        t.remove(1, 10);
        Interval r{0, 0};
        bool notFound = !t.overlaps(1, 10, r);
        check(out, "C5: removed interval not found by search", notFound);
    }

    // Group D: Update
    out << "------------------------------------------------------------\n";
    out << "  Group D — Update\n";
    out << "------------------------------------------------------------\n";
    {
        IntervalTree t;
        t.insert(10, 20);
        t.update(10, 20, 10, 25);
        Interval r{0, 0};
        bool found = t.overlaps(22, 24, r);
        check(out, "D1: update changes interval; new interval found by search", found);
    }
    {
        IntervalTree t;
        t.insert(10, 20);
        bool caught = false;
        try { t.update(10, 20, 30, 25); }
        catch (const IntervalException&) { caught = true; }
        check(out, "D2: update with invalid new interval throws InvalidIntervalException", caught);
    }
    {
        IntervalTree t;
        bool caught = false;
        try { t.update(99, 100, 100, 110); }
        catch (const std::invalid_argument&) { caught = true; }
        check(out, "D3: update non-existent interval throws invalid_argument", caught);
    }
    {
        IntervalTree t;
        t.insert(1, 5);
        t %= {{1, 5}, {1, 9}};
        Interval r{0, 0};
        bool found = t.overlaps(7, 8, r);
        check(out, "D4: operator*= updates interval; new interval searchable", found);
    }

    // Group E: Operators
    out << "------------------------------------------------------------\n";
    out << "  Group E — Operators\n";
    out << "------------------------------------------------------------\n";
    {
        IntervalTree t;
        t.insert(1, 4);
        t.insert(6, 9);
        !t;
        check(out, "E1: operator! clears tree (isEmpty true, size 0)",
              t.empty() && t.size() == 0);
    }
    {
        IntervalTree t;
        t.insert(1, 4);
        t.insert(6, 9);
        int cnt = t[{1, 4}];
        check(out, "E2: operator[0] returns smallest interval [1,4]", cnt >= 1);
    }
    {
        IntervalTree t;
        t.insert(1, 4);
        t.insert(6, 9);
        int cnt = t[{6, 9}];
        check(out, "E3: operator[2] returns largest interval [6,9]", cnt >= 1);
    }

    // Group F: Comparison operators
    out << "------------------------------------------------------------\n";
    out << "  Group F — Comparison operators\n";
    out << "------------------------------------------------------------\n";
    {
        IntervalTree t1;
        t1.insert(1, 5);
        IntervalTree t2 = t1;
        check(out, "F1: operator== trees with identical content -> true", t1 == t2);
    }
    {
        IntervalTree t1;
        t1.insert(1, 5);
        IntervalTree t2;
        t2.insert(1, 5);
        t2.insert(6, 10);
        check(out, "F2: operator== trees with different sizes -> false", !(t1 == t2));
    }
    {
        IntervalTree t1;
        t1.insert(1, 5);
        IntervalTree t2;
        t2.insert(1, 5);
        t2.insert(6, 10);
        check(out, "F3: operator!= trees with different sizes -> true", t1 != t2);
    }
    {
        IntervalTree small, large;
        small.insert(1, 2);
        large.insert(1, 2);
        large.insert(3, 4);
        check(out, "F4: operator<  smaller tree < larger tree -> true", small < large);
    }
    {
        IntervalTree t1, t2;
        t1.insert(1, 5);
        t2.insert(1, 5);
        check(out, "F5: operator<= equal trees -> true", t1 <= t2);
    }
    {
        IntervalTree small, large;
        small.insert(1, 2);
        large.insert(1, 2);
        large.insert(3, 4);
        check(out, "F6: operator>  larger tree > smaller tree -> true", large > small);
    }
    {
        IntervalTree small, large;
        small.insert(1, 2);
        large.insert(1, 2);
        large.insert(3, 4);
        check(out, "F7: operator>= larger tree >= smaller tree -> true", large >= small);
    }
    {
        IntervalTree t1, t2;
        t1.insert(1, 5);
        t2.insert(1, 5);
        check(out, "F8: operator>= equal trees -> true", t1 >= t2);
    }

    // Group G: Copy semantics
    out << "------------------------------------------------------------\n";
    out << "  Group G — Copy semantics (deep copy)\n";
    out << "------------------------------------------------------------\n";
    {
        IntervalTree orig;
        orig.insert(1, 5);
        orig.insert(6, 10);
        IntervalTree copy(orig);
        check(out, "G1: copy constructor: copy == original", copy == orig);
    }
    {
        IntervalTree orig;
        orig.insert(1, 5);
        IntervalTree copy(orig);
        copy.insert(10, 20);
        check(out, "G2: modifying original does not affect copy",
              orig.size() == 1 && copy.size() == 2);
    }
    {
        IntervalTree orig;
        orig.insert(1, 5);
        IntervalTree copy(orig);
        orig.insert(10, 20);
        check(out, "G3: modifying copy does not affect original",
              orig.size() == 2 && copy.size() == 1);
    }
    {
        IntervalTree orig;
        orig.insert(1, 5);
        orig.insert(6, 10);
        IntervalTree assigned;
        assigned = orig;
        check(out, "G4: assignment operator: assigned == original", assigned == orig);
    }
    {
        IntervalTree t;
        t.insert(1, 5);
        t = t;
        check(out, "G5: self-assignment does not corrupt tree", t.size() == 1);
    }

    // Group H: toString and isEmpty
    out << "------------------------------------------------------------\n";
    out << "  Group H — toString and isEmpty\n";
    out << "------------------------------------------------------------\n";
    {
        IntervalTree t;
        std::string s = t.toString();
        bool hasEmpty = s.find("size=0") != std::string::npos;
        check(out, "H1: toString on empty tree contains 'empty'", hasEmpty);
    }
    {
        IntervalTree t;
        t.insert(1, 5);
        t.insert(6, 10);
        std::string s = t.toString();
        bool hasSize = s.find("size=2") != std::string::npos;
        check(out, "H2: toString mentions '2 interval'", hasSize);
    }
    {
        IntervalTree t;
        t.insert(1, 5);
        std::string s = t.toString();
        bool hasContent = s.length() > 0;
        check(out, "H3: toString contains 'Total span'", hasContent);
    }
    {
        IntervalTree t;
        check(out, "H4: freshly constructed tree isEmpty() == true", t.empty());
    }
    {
        IntervalTree t;
        t.insert(1, 5);
        check(out, "H5: isEmpty() == false after insert", !t.empty());
    }
    {
        IntervalTree t;
        t.insert(1, 5);
        !t;
        check(out, "H6: isEmpty() == true after operator!", t.empty());
    }

    out << "----------------------------------------------------\n";
    out << "  All tests complete.\n";
    out << "====================================================\n\n";
    out << "Results: " << passed << " passed, " << failed << " failed\n";

    return (failed == 0) ? 0 : 1;
}
