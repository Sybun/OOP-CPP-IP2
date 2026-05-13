#ifndef INTERVALTREE_H
#define INTERVALTREE_H

#include <string>

namespace itree {

class IntervalTreeImpl;

// Exception for invalid intervals or duplicates
class IntervalException : public std::exception {
public:
    explicit IntervalException(const std::string& msg) : message_(msg) {}
    const char* what() const noexcept override { return message_.c_str(); }
private:
    std::string message_;
};

// Interval value type
struct Interval {
    int low;
    int high;
};

// Main IntervalTree class
class IntervalTree {
public:
    IntervalTree();
    ~IntervalTree();
    IntervalTree(const IntervalTree& other);
    IntervalTree& operator=(const IntervalTree& other);

    // CRUD operations
    void insert(int low, int high);
    void remove(int low, int high);
    void update(int oldLow, int oldHigh, int newLow, int newHigh);
    bool overlaps(int low, int high, Interval& result) const;

    // Utility
    int size() const;
    bool empty() const;
    void clear();
    std::string toString() const;

    // Operator aliases
    IntervalTree& operator+=(const Interval& iv);
    IntervalTree& operator-=(const Interval& iv);
    IntervalTree& operator%=(const std::pair<Interval, Interval>& edit);
    IntervalTree& operator!();
    int operator[](const Interval& query) const;

    // Comparison by size
    bool operator==(const IntervalTree& other) const;
    bool operator!=(const IntervalTree& other) const;
    bool operator< (const IntervalTree& other) const;
    bool operator<=(const IntervalTree& other) const;
    bool operator> (const IntervalTree& other) const;
    bool operator>=(const IntervalTree& other) const;

private:
    IntervalTreeImpl* pImpl_;
};

}

#endif
