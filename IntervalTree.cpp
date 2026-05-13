#include "IntervalTree.h"
#include <stdexcept>
#include <sstream>
#include <algorithm>

namespace itree {

// Internal node structure
struct Node {
    int low;
    int high;
    int maxHigh;   // maximum high value in subtree
    Node* left;
    Node* right;

    Node(int lo, int hi)
        : low(lo), high(hi), maxHigh(hi), left(nullptr), right(nullptr) {}
};

// Private implementation class
class IntervalTreeImpl {
public:
    Node* root;
    int   count;

    IntervalTreeImpl() : root(nullptr), count(0) {}

    // Deep copy
    static Node* copyTree(const Node* src) {
        if (!src) return nullptr;
        Node* n    = new Node(src->low, src->high);
        n->maxHigh = src->maxHigh;
        n->left    = copyTree(src->left);
        n->right   = copyTree(src->right);
        return n;
    }

    // Delete tree
    static void destroyTree(Node* n) {
        if (!n) return;
        destroyTree(n->left);
        destroyTree(n->right);
        delete n;
    }

    // Update maxHigh from children
    static void fixMax(Node* n) {
        if (!n) return;
        n->maxHigh = n->high;
        if (n->left)  n->maxHigh = std::max(n->maxHigh, n->left->maxHigh);
        if (n->right) n->maxHigh = std::max(n->maxHigh, n->right->maxHigh);
    }

    // Insert interval
    Node* insert(Node* n, int lo, int hi) {
        if (!n) {
            ++count;
            return new Node(lo, hi);
        }
        if (lo == n->low && hi == n->high) {
            throw IntervalException(
                "Duplicate interval [" + std::to_string(lo) + ", " +
                std::to_string(hi) + "] — duplicates are not allowed.");
        }
        if (lo < n->low)
            n->left  = insert(n->left,  lo, hi);
        else
            n->right = insert(n->right, lo, hi);

        fixMax(n);
        return n;
    }

    // Find minimum node (leftmost)
    static Node* minNode(Node* n) {
        while (n->left) n = n->left;
        return n;
    }

    // Remove interval
    Node* remove(Node* n, int lo, int hi, bool& found) {
        if (!n) return nullptr;

        if (lo < n->low) {
            n->left  = remove(n->left,  lo, hi, found);
        } else if (lo > n->low) {
            n->right = remove(n->right, lo, hi, found);
        } else {
            if (hi == n->high) {
                found = true;
                --count;
                if (!n->left) {
                    Node* tmp = n->right;
                    delete n;
                    return tmp;
                }
                if (!n->right) {
                    Node* tmp = n->left;
                    delete n;
                    return tmp;
                }
                // Two children: replace with in-order successor
                Node* succ = minNode(n->right);
                n->low  = succ->low;
                n->high = succ->high;
                bool dummy = false;
                n->right = remove(n->right, succ->low, succ->high, dummy);
                ++count;
            } else {
                // Same low, different high
                n->left  = remove(n->left,  lo, hi, found);
                if (!found)
                    n->right = remove(n->right, lo, hi, found);
            }
        }
        fixMax(n);
        return n;
    }

    // Find one overlapping interval
    static Node* findOverlap(Node* n, int lo, int hi) {
        while (n) {
            if (n->low <= hi && lo <= n->high)
                return n;
            if (n->left && n->left->maxHigh >= lo)
                n = n->left;
            else
                n = n->right;
        }
        return nullptr;
    }

    // Count all overlapping intervals
    static int countOverlaps(Node* n, int lo, int hi) {
        if (!n) return 0;
        if (n->maxHigh < lo) return 0;
        int cnt = 0;
        if (n->low <= hi && lo <= n->high) cnt = 1;
        cnt += countOverlaps(n->left,  lo, hi);
        cnt += countOverlaps(n->right, lo, hi);
        return cnt;
    }

    // In-order traversal for toString
    static void inorder(Node* n, std::ostringstream& oss) {
        if (!n) return;
        inorder(n->left, oss);
        oss << "  [" << n->low << ", " << n->high
            << "] (maxHigh=" << n->maxHigh << ")\n";
        inorder(n->right, oss);
    }
};

// ─ IntervalTree public methods ─

IntervalTree::IntervalTree()
    : pImpl_(new IntervalTreeImpl()) {}

IntervalTree::~IntervalTree() {
    IntervalTreeImpl::destroyTree(pImpl_->root);
    delete pImpl_;
}

IntervalTree::IntervalTree(const IntervalTree& other)
    : pImpl_(new IntervalTreeImpl()) {
    pImpl_->root  = IntervalTreeImpl::copyTree(other.pImpl_->root);
    pImpl_->count = other.pImpl_->count;
}

IntervalTree& IntervalTree::operator=(const IntervalTree& other) {
    if (this == &other) return *this;
    IntervalTreeImpl::destroyTree(pImpl_->root);
    pImpl_->root  = IntervalTreeImpl::copyTree(other.pImpl_->root);
    pImpl_->count = other.pImpl_->count;
    return *this;
}

void IntervalTree::insert(int low, int high) {
    if (low > high)
        throw IntervalException(
            "Invalid interval: low (" + std::to_string(low) +
            ") > high (" + std::to_string(high) + ").");
    if (low < 0)
        throw IntervalException(
            "Invalid interval: low cannot be negative (" +
            std::to_string(low) + ").");
    pImpl_->root = pImpl_->insert(pImpl_->root, low, high);
}

void IntervalTree::remove(int low, int high) {
    bool found = false;
    pImpl_->root = pImpl_->remove(pImpl_->root, low, high, found);
    if (!found)
        throw std::invalid_argument(
            "Interval [" + std::to_string(low) + ", " +
            std::to_string(high) + "] not found in tree.");
}

void IntervalTree::update(int oldLow, int oldHigh, int newLow, int newHigh) {
    remove(oldLow, oldHigh);
    insert(newLow, newHigh);
}

bool IntervalTree::overlaps(int low, int high, Interval& result) const {
    Node* n = IntervalTreeImpl::findOverlap(pImpl_->root, low, high);
    if (!n) return false;
    result = {n->low, n->high};
    return true;
}

int IntervalTree::size() const {
    return pImpl_->count;
}

bool IntervalTree::empty() const {
    return pImpl_->count == 0;
}

void IntervalTree::clear() {
    IntervalTreeImpl::destroyTree(pImpl_->root);
    pImpl_->root  = nullptr;
    pImpl_->count = 0;
}

std::string IntervalTree::toString() const {
    std::ostringstream oss;
    oss << "IntervalTree { size=" << pImpl_->count << " }\n";
    if (pImpl_->root)
        oss << "  (in-order: low asc)\n";
    IntervalTreeImpl::inorder(pImpl_->root, oss);
    return oss.str();
}

IntervalTree& IntervalTree::operator+=(const Interval& iv) {
    insert(iv.low, iv.high);
    return *this;
}

IntervalTree& IntervalTree::operator-=(const Interval& iv) {
    remove(iv.low, iv.high);
    return *this;
}

IntervalTree& IntervalTree::operator%=(const std::pair<Interval, Interval>& edit) {
    const Interval& oldIv = edit.first;
    const Interval& newIv = edit.second;
    update(oldIv.low, oldIv.high, newIv.low, newIv.high);
    return *this;
}

IntervalTree& IntervalTree::operator!() {
    clear();
    return *this;
}

int IntervalTree::operator[](const Interval& query) const {
    return IntervalTreeImpl::countOverlaps(pImpl_->root, query.low, query.high);
}

bool IntervalTree::operator==(const IntervalTree& o) const { return size() == o.size(); }
bool IntervalTree::operator!=(const IntervalTree& o) const { return size() != o.size(); }
bool IntervalTree::operator< (const IntervalTree& o) const { return size() <  o.size(); }
bool IntervalTree::operator<=(const IntervalTree& o) const { return size() <= o.size(); }
bool IntervalTree::operator> (const IntervalTree& o) const { return size() >  o.size(); }
bool IntervalTree::operator>=(const IntervalTree& o) const { return size() >= o.size(); }

}
